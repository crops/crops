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

#ifndef CODI_API_H
#define CODI_API_H

#include "codi_list.h"

int process_ceed_cmd(turff_node *t_node, int ceed_sock_fd, char *ceed_params[]);
void parse_codi_params(int argc, char *argv[]);

#endif
