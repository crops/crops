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
#include <sqlite3.h>
#include "globals.h"
#include "codi_db.h"
#include "codi_list.h"

static sqlite3 *codi_db;

/* connect */
int db_connect(void)
{
  int result;
  result = sqlite3_open(CODI_DB, &codi_db);

  if (result != SQLITE_OK ) {
    INFO("SQL error: %s\n", sqlite3_errmsg(codi_db));
    sqlite3_close(codi_db);
    return -1;
  }

  return init_db();
}

int init_db(void)
{
  int result;
  char *err = 0;
  char *qry = "CREATE TABLE IF NOT EXISTS nodes(id TEXT UNIQUE NOT NULL CHECK(id <> ''), \
    ip TEXT NOT NULL CHECK(ip <> ''), port TEXT NOT NULL CHECK(port <> ''), \
    date TEXT NOT NULL CHECK(date <> ''));";

  result = sqlite3_exec(codi_db, qry, 0, 0, &err);

  if (result != SQLITE_OK) {
    INFO("SQL error: %s\n", err);
    sqlite3_free(err);
    sqlite3_close(codi_db);
    return 1;
  }

  return 0;
}

/* insert node */
int db_insert_node(char *id, char *ip, char *port, char *date)
{
  int result;
  char *qry, *err = 0;
  turff_node *node;

  node = find_turff_node(id);

  /* if a node with this id exists just update it*/
  if (node != NULL) {
    asprintf (&qry, "UPDATE nodes SET ip = '%s', port = '%s', date = '%s' WHERE ID = '%s';",
    ip, port, date, id);
  }
  else {
    asprintf (&qry, "INSERT INTO nodes (id, ip, port, date) VALUES ('%s','%s','%s', '%s');",
    id, ip, port, date);
  }

  result = exec_db_query(qry, 0, 0, &err);
  free(qry);

  return result;
}

int exec_db_query(const char *sql, int (*callback)(void*, int, char**, char**), \
  void *callback_arg, char **errmsg)
{
  int result;

  if (db_connect() != 0)
    return -1;

  result = sqlite3_exec(codi_db, sql, callback, callback_arg, errmsg);

  if (result != SQLITE_OK) {
    INFO("SQL insert error: %s\n", *errmsg);
    sqlite3_free(*errmsg);
    sqlite3_close(codi_db);
    return result;
  }
  sqlite3_close(codi_db);
  return SQLITE_OK;
}

static int find_node_callback(void *dummy, int argc, char **argv, char **col_name)
{
  int i;

  /* no records were found */
  if(argc == 0)
    return 1;

  add_turff_node(argv[0], argv[1], argv[2], argv[3]);

  return 0;
}

/* find node(s) - gets all nodes if id is NULL */
int get_db_nodes(char *id)
{
  int result;
  char *qry, *err = 0;

  /* remove all nodes from the list before reading them from the db*/
  free_turff_nodes_list();

  /* get all nodes */
  if (id == NULL)
    asprintf (&qry, "SELECT * FROM nodes;");
  else
    asprintf (&qry, "SELECT * from nodes WHERE id='%s';", id);

  result = exec_db_query(qry, find_node_callback, 0, &err);
  free(qry);

  return result;
}
