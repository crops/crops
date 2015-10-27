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
#include "utils.h"
#include "globals.h"
#include "turff_api.h"

#define PORT_RANGE 100
extern char *turff_ops[];
extern char *codi_params[];
static int sock_fd;

/*TODO - close sockets on CTRL+C. Get rid of this when running as a daemon */
void close_sockets(int dummy) {
  close(sock_fd);
  INFO("\nClosed the sockets. Exiting!\n") ;
  exit(0);
}

int main(int argc, char *argv[]) {

  struct addrinfo *addr_p;
  struct sockaddr cli_addr;
  socklen_t cli_len;
  int i, ceed_sock_fd, saved_out, saved_err, bound;
  const char *port;

  parse_turff_params(argc, argv);

  bound = 0;
  port = turff_ops[KEY('s')];
  while(!bound && (int) *port > (int) *turff_ops[KEY('s')] - PORT_RANGE) {

    addr_p = bind_to_socket(NULL, port, &sock_fd);

    if (addr_p == NULL) {
      asprintf((char**)&port, "%d", atoi(port)-1) ;
    } else {
      turff_ops[KEY('s')] = (char*) port;
      register_agent(turff_ops);
      bound = 1;
      INFO("TURFF listening on port: %s\n", port);
    }
  }

  if (addr_p == NULL) {
    ERROR("Could not bind agent to socket\n");
    exit(EXIT_FAILURE);
  }

  listen(sock_fd,1);
  cli_len = sizeof(cli_addr);

  signal(SIGINT, close_sockets);

  while(1) {
    ceed_sock_fd = accept(sock_fd, &cli_addr, &cli_len);

    if (ceed_sock_fd < 0) {
      ERROR("ERROR on accept");
    }

    /* receive parameters from CODI */
    receive_args(ceed_sock_fd, codi_params);

    if (!strcmp(CODI_NAME, codi_params[KEY('z')])) {
      /* save stdout and stderr file descriptors */
      dup2(2, saved_err);
      dup2(1, saved_out);

      /* redirect stdout & stderr to ceed socket */
      dup2(ceed_sock_fd, 2);
      dup2(ceed_sock_fd, 1);

      /* make sure ceed and turff APIs match*/
      if (!strcmp(turff_ops[KEY('v')], codi_params[KEY('v')])) {
        /* process ceed requests */
        if (process_params(codi_params))
          ERROR("ERROR processing ceed request\n");
      } else {
        INFO("Incompatible versions: TURFF[%s] - CODI[%s]\n",
        turff_ops[KEY('v')], codi_params[KEY('v')]);
      }

      /* delay EOM - send as separate buffer */
      usleep(500000);
      send_data(ceed_sock_fd, TURFF_EOM, sizeof(TURFF_EOM));

      /* restore stdout and stderr */
      dup2(saved_err, 2);
      dup2(saved_out, 1);

      /* clear parameters and wait for a new service request */
      for (i = 0; i< KEY_ARR_SZ; i++){
        if (codi_params[i] != NULL) {
#ifdef DBG
          DEBUG("Received parameter [%c] : %s\n", i+'a', codi_params[i] );
#endif
          free(codi_params[i]);
          codi_params[i] = NULL ;
        }
      }
    } else {
      /* ceed connected to turff directly. send it EOM*/
      INFO("Request did not originate from CODI!\n");
      codi_params[KEY('e')] = "set";
      send_args(ceed_sock_fd, codi_params);
    }
    close(ceed_sock_fd);
  }
}

