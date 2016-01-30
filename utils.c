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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/select.h>
#include "utils.h"


/* TODO - add error checks and return error codes*/
int redirect_sockets(int in_sock, int out_sock) {
  int ready, received = 0;
  fd_set read_fd_set ;
  int size = BUFSIZ;
  void *buf = calloc(1, size);

  do {
    FD_ZERO(&read_fd_set);
    FD_SET(in_sock, &read_fd_set);
    ready = select(in_sock+1, &read_fd_set, NULL, NULL, NULL);
  } while (ready==-1 && errno == EINTR);

  if (ready > 0 ){
    if (FD_ISSET(in_sock, &read_fd_set)) {
      while (size > 0) {
        received = recv(in_sock, buf, size, 0);
        if (received < 0 ){
          ERROR("Error : %s\n", strerror(errno));
        } else if (received == 0) {
          break ;
        } else {
          size -= received;
          if (strstr((char*) buf, TURFF_EOM)) {
            send_data(out_sock, (const void *) buf,received);
            break;
          } else {
            send_data(out_sock, (const void *) buf,received);
          }

        }
      }
    }
  } else if (ready < 0){
    free(buf);
    ERROR("Select error\n");
  }
  free(buf);
  return 0;
}

/* TODO - add error checks and return error codes*/
int send_data(int sock_fd, const void *buf, size_t size) {
  int n, ready, sent = 0;
  fd_set write_fd_set ;
  struct timeval timeout;

  timeout.tv_sec = (long) 0.3;
  timeout.tv_usec = 0;

  FD_ZERO(&write_fd_set);
  FD_SET(sock_fd, &write_fd_set);
  n = sock_fd +1;

  ready = select(n, NULL, &write_fd_set, NULL, &timeout);

  if (ready) {
    while (size > 0) {
      sent = send(sock_fd, buf, size, 0);
      if (sent == -1){
        ERROR("Unable to send data. Error: [%d] %s", errno, strerror(errno));
      }
      else {
        size -= sent;
#ifdef DBG
        DEBUG("Bytes sent: %d. Bytes remaining: %zu\n", sent, size);
#endif
      }
    }
  } else if (ready == -1){
      ERROR("Select timeout\n");
  }
  return 0;
}

/* TODO - add error checks and return error codes*/
int receive_data(int sock_fd, void *buf, size_t size, int is_ceed, int *done) {
  int ready, received = 0;
  fd_set read_fd_set ;

  do {
    FD_ZERO(&read_fd_set);
    FD_SET(sock_fd, &read_fd_set);
    ready = select(sock_fd+1, &read_fd_set, NULL, NULL, NULL);
  } while (ready==-1 && errno == EINTR);

  if (ready > 0 ){
    if (FD_ISSET(sock_fd, &read_fd_set)) {
      while (size > 0) {
        received = recv(sock_fd, buf, size, 0);
        if (received < 0 ){
          ERROR("Error : %s\n", strerror(errno));
        } else if (received == 0) {
          if (is_ceed)
            *done = 1;
          break ;
        } else {
          size -= received;
          if (strstr((char*) buf, TURFF_EOM) && done != NULL ) {
            memset(strstr((char*) buf, TURFF_EOM), 0, sizeof(TURFF_EOM));
            printf("%s", (char*) buf);
            *done = 1;
            break;
          } else if (is_ceed) {
            printf("%s", (char*) buf);
          }
          buf = (char*)buf + received;
#ifdef DBG
          DEBUG("\nBytes read: %d Bytes remaining: %zu\n", received, size);
#endif
        }
      }
    }
  } else if (ready < 0){
    ERROR("Select error : %s\n", strerror(errno));
  }

  return 0;
}

void send_chunk(int sock_fd, msg_chunk *chunk) {
  msg_chunk *head;
  head = chunk ;
  while (chunk != NULL) {
    send_data(sock_fd, chunk, sizeof(msg_chunk));
#ifdef DBG
    DEBUG("Chunk op_code: %d , Chunk payload: %s\n", (*chunk).op_code,
      (*chunk).arg);
#endif
    chunk = chunk->next ;
  }

  /* done with this parameter - free all chunks*/
  while (head != NULL) {
    chunk = head;
    head = chunk->next;
    free(chunk);
  }
}

int receive_chunk(int sock_fd, msg_chunk *chunk) {
  return receive_data(sock_fd, chunk, sizeof(msg_chunk), 0, NULL);
}

void send_args(int sock_fd, char *params[]) {
  int  i, j, num_chunks;
  msg_chunk *new_chunk, *tail_chunk, *head_chunk ;

  /* send all params in chunks of 20 chars*/
  for(i = 0; i < KEY_ARR_SZ; i++) {
    if (params[i] != NULL) {
      new_chunk = calloc(1, sizeof(msg_chunk));
      head_chunk = new_chunk;
      tail_chunk = new_chunk;
      num_chunks =  ceil((double) strlen(params[i]) /
        (sizeof((*new_chunk).arg)-1));

      for(j = 0; j < num_chunks; j++) {
        (*tail_chunk).op_code = i;
        if (j != 0) {
          strncpy((*tail_chunk).arg, params[i]+(j*sizeof((*tail_chunk).arg)-j),
            sizeof((*tail_chunk).arg)-1);
        } else {
          strncpy((*tail_chunk).arg, params[i], sizeof((*tail_chunk).arg)-1);
        }

        /* was this the last chunk*/
        if ((j+1) != num_chunks) {
          new_chunk = calloc(1, sizeof(msg_chunk));
          tail_chunk->next = new_chunk;
          tail_chunk = new_chunk;
        }
      }
      /* this param is ready - send head chunk*/
      send_chunk(sock_fd, head_chunk);
    }
  }

  /* we are done with all params. Send EOM as an individual chunk*/
  new_chunk = calloc(1, sizeof(msg_chunk));
  (*new_chunk).op_code = -1;
  strcpy((*new_chunk).arg, MSG_TERM);
  send_chunk(sock_fd, new_chunk);
}

/* receive all params in chunks of 20 chars including terminator*/
void receive_args(int sock_fd, char *params[]) {
  msg_chunk param_chunk;
  char *tmp;
  do {
    memset(&param_chunk, 0, sizeof(msg_chunk));
    receive_chunk(sock_fd, &param_chunk);

#ifdef DBG
    DEBUG("Chunk op_code: %d , Chunk payload: %s\n", param_chunk.op_code,
      &param_chunk.arg);
#endif
    if (param_chunk.op_code != -1) {
      if (params[param_chunk.op_code] == NULL) {
        tmp = calloc(1, sizeof(param_chunk.arg));
      } else {
        tmp = realloc(params[param_chunk.op_code], strlen(params[param_chunk.op_code])+
          sizeof(param_chunk.arg));
      }

      params[param_chunk.op_code] = tmp;
      strcpy(params[param_chunk.op_code] + strlen(params[param_chunk.op_code]),
        param_chunk.arg);
#ifdef DBG
      DEBUG("Chunk op_code: %d , parameter: %s\n", param_chunk.op_code,
        params[param_chunk.op_code]);
#endif
    }
  }while(param_chunk.op_code != -1); /* not EOM chunk*/
}

struct addrinfo* connect_to_socket(char *ip_in, char *port_in, int* sock_fd_out) {
  int sock_fd, result;
  struct addrinfo criteria, *srv_addr, *addr_p;

  /* connect to agent */
  bzero(&criteria, sizeof(criteria));
  criteria.ai_family = AF_INET;
  criteria.ai_socktype = SOCK_STREAM;
  criteria.ai_flags = 0;
  criteria.ai_protocol = 0;

  result = getaddrinfo(ip_in, port_in, &criteria,&srv_addr);

  if(result != 0) {
    ERROR("Could not get address info list\n");
    exit(EXIT_FAILURE);
  }

  for(addr_p = srv_addr; addr_p != NULL; addr_p = addr_p->ai_next) {
    *sock_fd_out = socket(addr_p->ai_family, addr_p->ai_socktype, addr_p->ai_protocol);

    if (*sock_fd_out == -1)

      continue;

    if (connect(*sock_fd_out, addr_p->ai_addr, addr_p->ai_addrlen) != -1)
      break;  /* Connected */

    close(*sock_fd_out);
  }

  return addr_p ;
}


struct addrinfo* bind_to_socket(char *ip_in, const char *port_in, int* sock_fd_out) {
  struct addrinfo criteria, *srv_addr;
  struct addrinfo *addr_p;
  int result, opt = 1;

  bzero(&criteria, sizeof(criteria));
  criteria.ai_family = AF_INET;
  criteria.ai_socktype = SOCK_STREAM;
  criteria.ai_flags = AI_PASSIVE;
  criteria.ai_protocol = 0;
  criteria.ai_canonname = NULL;
  criteria.ai_addr = NULL;
  criteria.ai_next = NULL;

  result = getaddrinfo(NULL, port_in, &criteria, &srv_addr);

  if (result != 0) {
    ERROR("Could not get address info list\n");
  }

  for (addr_p = srv_addr; addr_p != NULL; addr_p = addr_p->ai_next) {
    *sock_fd_out = socket(addr_p->ai_family, addr_p->ai_socktype, addr_p->ai_protocol);

    if (*sock_fd_out == -1)
      continue;

    if (setsockopt(*sock_fd_out, SOL_SOCKET, SO_REUSEADDR,
      (char *)&opt, sizeof(opt)) < 0) {

      close(*sock_fd_out);
      ERROR("setsockopt failed\n");
    }

    if (bind(*sock_fd_out, addr_p->ai_addr, addr_p->ai_addrlen) == 0)
      break;                  /* Success */

      close(*sock_fd_out);
  }

  freeaddrinfo(srv_addr);
  return addr_p;
}

void copy_params(char *in[], char *out[]) {
  int i;
  for (i = 0; i< KEY_ARR_SZ; i++){
    if (in[i] != NULL) {
      asprintf(&out[i],"%s",in[i]);
    }
  }
}

void free_params(char *params[]) {
  int i;
  for (i = 0; i< KEY_ARR_SZ; i++){
    if (params[i] != NULL) {
      free(params[i]);
      params[i] = NULL ;
    }
  }
}
