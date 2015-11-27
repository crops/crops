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

#ifndef CODI_DB_H
#define CODI_DB_H

#define CODI_DB      "codi.db"

int init_db(void);
int db_insert_node(char *id, char *ip, char *port, char *date);
int exec_db_query(const char *sql, int (*callback)(void*, int, char**, char**), \
  void *callback_arg, char **errmsg);
int get_db_nodes(char *id);

#endif
