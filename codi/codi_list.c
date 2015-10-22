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
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "codi_list.h"

turff_node *head = NULL;

turff_node *find_turff_node(char *id) {
  turff_node *cur = head;

  while (cur != NULL) {
    if(!strcmp(cur->id, id)) {
      return cur;
    } else {
      cur = cur->next;
    }
  }
  return NULL;
}

void add_turff_node(char *ip, char *port, char *hostname) {
  int node_exists = 0;
  turff_node *node;
  node = find_turff_node(hostname);

  /* if a node with this port is alredy in the list just update it */
  if ( node == NULL) {
    node = calloc(1, sizeof(turff_node));
  } else {
    node_exists = 1;
  }

  asprintf(&(node->ip), "%s", ip);
  asprintf(&(node->port), "%s",  port);
  asprintf(&(node->id), "%s", hostname);

  if(!node_exists) {
    node->next = head;
    head = node;
  }
}


/* TODO - we will be sending these to ceed*/
void return_turff_nodes(int sock_fd) {
  int i;
  char *tmp_node[KEY_ARR_SZ];
  turff_node *cur = head;

  for (i = 0; i <  KEY_ARR_SZ; i++)
    tmp_node[i] = NULL;

  while (cur != NULL) {
    tmp_node[KEY('d')] = cur->id;

#ifdef DBG
    DEBUG("Sent turff node id : %s ip: %s port %s\n", cur->id, cur->ip, cur->port);
#endif

    send_args(sock_fd, tmp_node);
    cur = cur->next;
  }

  tmp_node[KEY('e')] = "set";
  send_args(sock_fd, tmp_node);
}
