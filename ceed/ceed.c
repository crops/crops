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

#include <stdlib.h>
#include "globals.h"
#include "ceed_api.h"
#include "utils.h"

extern char *ceed_ops[];

int main(int argc, char *argv[]) {
  int i, done, sock_fd;
  char *turff_nodes[KEY_ARR_SZ];
  struct addrinfo *addr_p;
  void *ceed_out;

  parse_ceed_params(argc, argv);

  addr_p = connect_to_socket(ceed_ops[KEY('i')], ceed_ops[KEY('s')], &sock_fd);

  if(addr_p == NULL) {
    ERR("Could not connect to CODI\n");
  } else {
    INFO("Connected to CODI on %s port : %s\n",
      ceed_ops[KEY('i')], ceed_ops[KEY('s')]);
  }

  INFO("Host Architecture : %s\n", ceed_ops[KEY('a')]);
  send_args(sock_fd, ceed_ops);

  if(ceed_ops[KEY('l')] == NULL) { /* this was a service request */
    INFO(" ==========  CEED OUTPUT BEGIN  ==========\n");
    ceed_out = calloc(1, BUFSIZ);
    done = 0;

    while(!done) {
      memset(ceed_out, 0, BUFSIZ);
      receive_data(sock_fd, ceed_out, BUFSIZ, 1, &done);
    }
    free(ceed_out);

    INFO(" ==========  CEED OUTPUT END  ==========\n");
  } else { /* this was nodes request, receive turff nodes */
    do {
      for(i = 0; i <  KEY_ARR_SZ; i++)
        turff_nodes[i] = NULL;

      receive_args(sock_fd, turff_nodes);
      if(turff_nodes[KEY('e')] == NULL) {
        INFO("TURFF Node ID: %s \n", turff_nodes[KEY('d')]);
      }
    }while (turff_nodes[KEY('e')] == NULL);
  }

  close(sock_fd);
  return 0;
}
