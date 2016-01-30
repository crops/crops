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
#include <ctype.h>
#include "utils.h"
#include "codi_api.h"
#include "codi_list.h"
#include "codi_launcher.h"

char *codi_ops[KEY_ARR_SZ];

void print_codi_usage(int argc, char *argv[])
{
  printf("Usage: %s \n", argv[0]);
  printf(" -s port number on which CODI is listening [default: 10000]\n");
  printf(" -u unix socket path of Docker engine [default: /var/lib/docker.sock]\n");
  printf(" -i ip address of Docker engine [default: 127.0.0.1]\n");
  printf(" -p port number of Docker engine [default: 2376]\n");
  printf(" -h print this help menu\n");
  printf(" -v show CODI version\n");
}

void init_codi_params()
{
  int i;

  for (i = 0; i <  KEY_ARR_SZ; i++)
    codi_ops[i] = NULL;

  codi_ops[KEY('s')] = CODI_PORT;
  codi_ops[KEY('v')] = VERSION;
  codi_ops[KEY('i')] = NULL;
  codi_ops[KEY('p')] = NULL;
  codi_ops[KEY('u')] = DOCKER_UNIX_SOCKET;
  codi_ops[KEY('z')] = CODI_NAME;
}

void parse_codi_params(int argc, char *argv[])
{
  int c, i_flg = 0, p_flg = 0, u_flg = 0;

  init_codi_params() ;

  while ((c = getopt(argc, argv, "hvs:i:p:u:")) != -1) {
    switch (c) {
    case 'h':
      print_codi_usage(argc, argv);
      exit(EXIT_SUCCESS);
      break;
    case 'v':
      INFO("CODI version : %s\n", VERSION);
      exit(EXIT_SUCCESS);
      break;
    case 's':
      codi_ops[KEY('s')]  = optarg;
      break;
    case 'i':
      if(u_flg) {
        print_codi_usage(argc, argv);
      } else {
        i_flg++;
        codi_ops[KEY('i')]  = optarg;
      }
      break;
    case 'p':
      if(u_flg) {
        print_codi_usage(argc, argv);
      } else {
        p_flg++;
        codi_ops[KEY('p')]  = optarg;
      }
      break;
    case 'u':
      if(i_flg || p_flg) {
        print_codi_usage(argc, argv);
      } else {
        u_flg++;
        codi_ops[KEY('u')]  = optarg;
      }
      break;
    case '?':
      if (optopt == 's' || optopt == 'i' || optopt == 'p' || optopt == 'u')
        INFO("Option -%c requires an argument.\n", optopt);
      else if (isprint (optopt))
        INFO("Unknown option `-%c'.\n", optopt);
      else
        INFO("Unknown option character `\\x%x'.\n", optopt);
    default:
      print_codi_usage(argc, argv);
      exit(EXIT_FAILURE);
    }
  }
}

int process_ceed_cmd(turff_node *t_node, int ceed_sock_fd, char *ceed_params[])
{
  struct addrinfo *addr_p;
  int turff_sock_fd;

  addr_p = connect_to_socket(t_node->ip, t_node->port, &turff_sock_fd);

  if (addr_p == NULL) {
    INFO("Could not connect to node id: %s ip: %s port: %s\n",
    t_node->id, t_node->ip, t_node->port);
    return 1;
  } else {
    INFO("Connected to node id: %s ip: %s port: %s\n",
      t_node->id, t_node->ip, t_node->port);

    /* change the source signature of the param array */
    asprintf(&(ceed_params[KEY('z')]), "%s", CODI_NAME);

    /* forward parameters to turff */
    send_args(turff_sock_fd, ceed_params);
    redirect_sockets(turff_sock_fd, ceed_sock_fd);
  }
  close(turff_sock_fd);
  return 0;
}
