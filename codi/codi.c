/*
 * Copyright (C) 2015 Intel Corporation
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

#include <stdlib.h>
#include <signal.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "utils.h"
#include "globals.h"
#include "codi.h"
#include "codi_list.h"

static int codi_sock_fd;

/*TODO - close sockets on CTRL+C. Get rid of this when running as a daemon */
void close_sockets(int dummy) {
  close(codi_sock_fd);
  INFO("\nClosed CODI socket. Exiting!\n") ;
  exit(0);
}


int main(int argc, char *argv[]) {

  struct addrinfo *addr_p;
  struct sockaddr cli_addr;
  struct sockaddr_in *cli_ip;
  socklen_t cli_len;
  int i, cli_sock_fd, node_sock_fd;
  const char *codi_port = (const char*) CODI_PORT;
  char *ip, *cli_params[KEY_ARR_SZ];
  turff_node *req_node;

  for (i = 0; i <  KEY_ARR_SZ; i++)
    cli_params[i] = NULL;

  addr_p = bind_to_socket(NULL, codi_port, &codi_sock_fd);

  if (addr_p == NULL) {
    ERROR("Could not bind CODI to socket\n");
    exit(EXIT_FAILURE);
  } else {
    INFO("CODI listening on port: %s\n", codi_port);

  }

  listen(codi_sock_fd,1);
  cli_len = sizeof(cli_addr);

  signal(SIGINT, close_sockets);

  while(1) {
    cli_sock_fd = accept(codi_sock_fd, &cli_addr, &cli_len);

    if (cli_sock_fd < 0) {
      ERROR("ERROR on accept");
    }

    receive_args(cli_sock_fd, cli_params);

    if(!strcmp(cli_params[KEY('z')], TURFF_NAME)) {
      /* registration from turff */
      cli_ip = (struct sockaddr_in*) &cli_addr;
      asprintf(&ip, "%s", inet_ntoa(cli_ip->sin_addr));
      cli_params[KEY('c')] = ip;
      db_insert_node(cli_params[KEY('n')], cli_params[KEY('c')],  cli_params[KEY('s')], "some date");
    } else if (!strcmp(cli_params[KEY('z')], CEED_NAME) && (cli_params[KEY('l')] != NULL )) {
      /* ceed request for available toolchains */
      return_turff_nodes(cli_sock_fd);
    } else if (!strcmp(cli_params[KEY('z')], CEED_NAME) && (cli_params[KEY('d')] != NULL )) {
      /* must be a command from ceed*/
      /* TODO - check if toolchain is up and try to start it if it is not*/
      req_node = find_turff_node(cli_params[KEY('d')]);
      if(req_node != NULL) {

        addr_p = connect_to_socket(req_node->ip, req_node->port, &node_sock_fd);

        if(addr_p == NULL) {
          INFO("Could not connect to node id: %s ip: %s port: %s\n",
          req_node->id, req_node->ip, req_node->port);
        } else {
          INFO("Connected to node id: %s ip: %s port: %s\n",
          req_node->id, req_node->ip, req_node->port);

          /* change the source signature of the param array */
          asprintf(&(cli_params[KEY('z')]), "%s", CODI_NAME);

          /* forward parameters to turff */
          send_args(node_sock_fd, cli_params);
          redirect_sockets(node_sock_fd, cli_sock_fd);
        }
      } else {
        INFO("Container id: %s not found in CODI's table\n", cli_params[KEY('d')]);
      }
    }

    /* clear parameters and wait for a new service request */
    for (i = 0; i< KEY_ARR_SZ; i++){
      if (cli_params[i] != NULL) {
#ifdef DBG
        DEBUG("Received parameter [%c] : %s\n", i+'a', cli_params[i] );
#endif
        free(cli_params[i]);
        cli_params[i] = NULL ;
      }
    }
    close(cli_sock_fd);
  }
}
