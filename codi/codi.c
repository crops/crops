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

extern char *codi_ops[];
int codi_sock_fd;
/*
pthread_mutex_t lock;
*/

/*TODO - close sockets on CTRL+C. Get rid of this when running as a daemon */
void close_sockets(int dummy) {
  close(codi_sock_fd);
  INFO("\nClosed CODI socket. Exiting!\n") ;
  exit(0);
}

int main(int argc, char *argv[]) {
  struct addrinfo *addr_p;
  int i, cli_sock_fd;
  const char *codi_port = (const char*) CODI_PORT;
  turff_node *req_node = NULL;
  char *cli_params[KEY_ARR_SZ], *ip, *docker_url, *tmp_str;
  socklen_t cli_len;
  struct sockaddr cli_addr;
  struct sockaddr_in *cli_ip;

  parse_codi_params(argc, argv);

  for (i = 0; i <  KEY_ARR_SZ; i++)
    cli_params[i] = NULL;

  addr_p = bind_to_socket(NULL, codi_port, &codi_sock_fd);

  if (addr_p == NULL) {
    ERR("Could not bind CODI to socket\n");
  } else {
    INFO("CODI listening on port: %s\n", codi_port);

  }

  listen(codi_sock_fd,1);
  cli_len = sizeof(cli_addr);

  signal(SIGINT, close_sockets);

  while(1) {
    cli_sock_fd = accept(codi_sock_fd, &cli_addr, &cli_len);

    if (cli_sock_fd < 0)
      ERR("ERROR on accept");

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
    } else if (!strcmp(cli_params[KEY('z')], CEED_NAME) && (cli_params[KEY('l')] != NULL )) {
      /* ceed request for available toolchains */
      return_turff_nodes(cli_sock_fd);
    } else if (!strcmp(cli_params[KEY('z')], CEED_NAME) && (cli_params[KEY('d')] != NULL )) {
      /* must be a command from ceed*/
      req_node = find_turff_node(cli_params[KEY('d')]);
      if (req_node != NULL) {

        /* check if docker engine is listening on a unix socket or tcp*/
        if (codi_ops[KEY('i')] == NULL || codi_ops[KEY('p')] == NULL) {
          asprintf(&docker_url, "%s", codi_ops[KEY('u')]);
        } else {
          asprintf(&docker_url, "%s:%s", codi_ops[KEY('i')], codi_ops[KEY('p')]);
        }

        if (is_container_running(docker_url, cli_params[KEY('d')])) {
          process_ceed_cmd(req_node, cli_sock_fd, cli_params);
        } else {
          asprintf(&tmp_str, "Container %s is not running\n%s",
            cli_params[KEY('d')], TURFF_EOM);
          send_data(cli_sock_fd, tmp_str, strlen(tmp_str));
          free(tmp_str);
        }
        free(docker_url);
      } else {
        asprintf(&tmp_str, "Container %s not found in database\n%s",
          cli_params[KEY('d')], TURFF_EOM);
        send_data(cli_sock_fd, tmp_str, strlen(tmp_str));
        free(tmp_str);
      }
    } else {
      asprintf(&tmp_str, "Invalid command sent to CODI\n%s", TURFF_EOM);
      send_data(cli_sock_fd, tmp_str, strlen(tmp_str));
      free(tmp_str);
    }

    free_params(cli_params);
  }
}
