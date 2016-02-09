/*
 * Copyright (C) 2016 Intel Corporation
 *
 * Author: Todor Minchev <todor.minchev@linux.intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, or (at your option) any later version, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#include "globals.h"
#include <stdlib.h>
#include <signal.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "utils.h"
#include "codi_db.h"
#include "codi_api.h"
#include "codi_list.h"
#include "codi_launcher.h"

int codi_sock_fd;
extern char *codi_ops[];
static int cli_sock_fd;
struct sockaddr cli_addr;
socklen_t cli_len;
static struct sockaddr_in *cli_ip;
static char *cli_params[KEY_ARR_SZ], *ip;
pthread_mutex_t lock;

/*TODO - close sockets on CTRL+C. Get rid of this when running as a daemon */
void close_sockets(int dummy) {
  close(codi_sock_fd);
  INFO("\nClosed CODI socket. Exiting!\n") ;
  exit(0);
}

static void *listener_thread(void *args)
{
  int i;
  while (1) {
    cli_sock_fd = accept(codi_sock_fd, &cli_addr, &cli_len);

    if (cli_sock_fd < 0)
      ERR("ERROR on accept");

    pthread_mutex_lock(&lock);
    free_params(cli_params);
    receive_args(cli_sock_fd, cli_params);

    /*turff registration*/
    if (!strcmp(cli_params[KEY('z')], TURFF_NAME)) {
      cli_ip = (struct sockaddr_in*) &cli_addr;
      asprintf(&ip, "%s", inet_ntoa(cli_ip->sin_addr));
      cli_params[KEY('c')] = ip;
      db_insert_node(cli_params[KEY('n')], cli_params[KEY('c')],  cli_params[KEY('s')]);
      INFO("\nToolchain registration received: %s : %s : %s \n", cli_params[KEY('n')],
        cli_params[KEY('c')], cli_params[KEY('s')]) ;
      close(cli_sock_fd);
    } else if (!strcmp(cli_params[KEY('z')], CEED_NAME) && (cli_params[KEY('l')] != NULL )) {
      /* ceed request for available toolchains */
      return_turff_nodes(cli_sock_fd);
      close(cli_sock_fd);
    }

    pthread_mutex_unlock(&lock);
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  struct addrinfo *addr_p;
  int i, cli_sock_fd_tmp, process_ok;
  const char *codi_port = (const char*) CODI_PORT;
  char *ceed_params_tmp[KEY_ARR_SZ];
  char *docker_url;
  pthread_t reg_toolchain_thread;
  turff_node *req_node = NULL;

  parse_codi_params(argc, argv);
  for (i = 0; i <  KEY_ARR_SZ; i++)
    cli_params[i] = NULL;

  if (pthread_mutex_init(&lock, NULL) != 0)
    ERR("Mutex initialization failed\n");

  addr_p = bind_to_socket(NULL, codi_port, &codi_sock_fd);

  if (addr_p == NULL) {
    ERR("Could not bind CODI to socket\n");
  } else {
    INFO("CODI listening on port: %s\n", codi_port);

  }

  listen(codi_sock_fd,1);
  cli_len = sizeof(cli_addr);

  signal(SIGINT, close_sockets);

  /*start toolchain registration thread*/
  if (pthread_create(&reg_toolchain_thread, NULL, listener_thread, NULL)) {
    ERR("Unable to start socket listener thread\n");
  }

  process_ok = 0;
  for (i = 0; i <  KEY_ARR_SZ; i++)
    ceed_params_tmp[i] = NULL;

  while(1) {

    pthread_mutex_lock(&lock);

    if (cli_params[KEY('z')] != NULL && !process_ok && !strcmp(cli_params[KEY('z')], CEED_NAME)
      && cli_params[KEY('d')] != NULL) {

      if (ceed_params_tmp[KEY('z')] == NULL) {
        copy_params(cli_params, ceed_params_tmp);
        cli_sock_fd_tmp = dup(cli_sock_fd);
      }

      /* must be a command from ceed*/
      req_node = find_turff_node(cli_params[KEY('d')]);

      /* check if docker engine is listening on a unix socket or tcp*/
      if (codi_ops[KEY('i')] == NULL || codi_ops[KEY('p')] == NULL) {
        asprintf(&docker_url, "%s", codi_ops[KEY('u')]);
      } else {
        asprintf(&docker_url, "%s:%s", codi_ops[KEY('i')], codi_ops[KEY('p')]);
      }

      if (req_node != NULL) {
        if (is_container_running(docker_url, cli_params[KEY('d')])) {
          process_ok = 1;
        } else if (start_container(docker_url, cli_params[KEY('d')])) {
          process_ok = 1;
        } else {
          INFO("Container %s does not exists\n", cli_params[KEY('d')]);
        }
      } else if (start_container(docker_url, cli_params[KEY('d')])) {
        process_ok = 1;
      } else {
        INFO("Container %s not running\n", cli_params[KEY('d')]);
      }
      free(docker_url);
    }

    pthread_mutex_unlock(&lock);

    if (cli_params[KEY('z')] != NULL && process_ok) {
      req_node = find_turff_node(ceed_params_tmp[KEY('d')]);
      if (req_node != NULL) {
        process_ceed_cmd(req_node, cli_sock_fd_tmp, ceed_params_tmp);
        process_ok = 0;
        free_params(ceed_params_tmp);
        free_params(cli_params);
        close(cli_sock_fd_tmp);
        close(cli_sock_fd);
      }
    }
  }
}
