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

#ifndef UTILS_H
#define UTILS_H

int send_data(int sock_fd, const void *buf, size_t size);
int receive_data(int sock_fd, void *buf, size_t size, int is_ceed, int *done);
void send_args(int sock_fd, char *params[]);
void receive_args(int sock_fd, char *params[]);
struct addrinfo* connect_to_socket(char *ip_in, char *port_in, int* sock_fd_out);
struct addrinfo* bind_to_socket(char *ip_in, const char *port_in, int* sock_fd_out);

#endif
