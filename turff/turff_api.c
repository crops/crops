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
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include "utils.h"

const char workspace_root[] = "/crops";
char *turff_ops[KEY_ARR_SZ];
char *codi_params[KEY_ARR_SZ];

void print_turff_usage(int argc, char *argv[]) {
  printf("Usage: %s \n", argv[0]);
  printf(" -s port number on which the agent is listening [default: 9999]\n");
  printf(" -i CODI IP address [default: 127.0.0.1]\n");
  printf(" -d CODI port number [default: 10000]\n");
  printf(" -n node_id to be sent to CODI [default: TURFFID env variable]\n");
  printf(" -h print this help menu\n");
  printf(" -v show turff version\n");
}

void init_turff_params(){
  int i;

  for (i = 0; i <  KEY_ARR_SZ; i++)
    codi_params[i] = NULL;

  turff_ops[KEY('s')] = TURFF_PORT;
  turff_ops[KEY('v')] = VERSION;
  turff_ops[KEY('i')] = CODI_IP_NATIVE;
  turff_ops[KEY('d')] = CODI_PORT;
  turff_ops[KEY('z')] = TURFF_NAME;
}

void parse_turff_params(int argc, char *argv[]) {
  int c ;

  init_turff_params() ;

  while ((c = getopt(argc, argv, "hvi:d:s:i:n:")) != -1) {
    switch (c) {
    case 'h':
      print_turff_usage(argc, argv);
      exit(EXIT_SUCCESS);
      break;
    case 'v':
      INFO("turff version : %s\n", VERSION);
      exit(EXIT_SUCCESS);
      break;
    case 's':
      turff_ops[KEY('s')]  = optarg;
      break;
    case 'i':
      turff_ops[KEY('i')]  = optarg;
      break;
    case 'd':
      turff_ops[KEY('d')]  = optarg;
      break;
    case 'n':
      turff_ops[KEY('n')]  = optarg;
      break;
    case '?':
      if (optopt == 's')
        INFO("Option -%c requires an argument.\n", optopt);
      else if (isprint (optopt))
        INFO("Unknown option `-%c'.\n", optopt);
      else
        INFO("Unknown option character `\\x%x'.\n", optopt);
    default:
      print_turff_usage(argc, argv);
      exit(EXIT_FAILURE);
    }
  }
}

/* TODO - add error checks and return error codes*/
int exec_command(char* cmd) {
  system(cmd);
  free(cmd);
  return 0;
}

/* TODO - add error checks and return error codes*/
int build_at_prj(char *platform, char *prj_dir)
{
  char cmd[] = "cd %s/%s/%s; chmod -R 777 *;./autogen.sh $CONFIGURE_FLAGS; make;";
  char *build_cmd;

  asprintf(&build_cmd, cmd, workspace_root, platform, prj_dir);
  exec_command(build_cmd);
  return 0;
}

/* TODO - add error checks and return error codes*/
int clean_at_prj(char *platform, char *prj_dir) {
  char cmd[] = "make distclean -C %s/%s/%s/";
  char *clean_cmd;

  asprintf(&clean_cmd, cmd, workspace_root, platform, prj_dir);
  exec_command(clean_cmd);
  return 0;
}

/* TODO - add error checks and return error codes*/
int build_cm_prj(char *platform, char *prj_dir)
{
  char cmd[] = "cd %s/%s/%s; mkdir -p build; cd build; cmake ../; make;";
  char *build_cmd;

  asprintf(&build_cmd, cmd, workspace_root, platform, prj_dir);
  exec_command(build_cmd);
  return 0;
}

/* TODO - add error checks and return error codes*/
int clean_cm_prj(char *platform, char *prj_dir)
{
  char cmd[] = "make clean -C %s/%s/%s/build;";
  char *clean_cmd;

  asprintf(&clean_cmd, cmd, workspace_root, platform, prj_dir);
  exec_command(clean_cmd);
  return 0;
}

/* TODO - add error checks and return error codes*/
int build_m_prj(char *platform, char *prj_dir, char *make_target)
{
  char cmd[] = "make %s -C %s/%s/%s";
  char *build_cmd;

  asprintf(&build_cmd, cmd, make_target, workspace_root, platform, prj_dir);
  exec_command(build_cmd);
  return 0;
}

/* TODO - add error checks and return error codes*/
int clean_m_prj(char *platform, char *prj_dir) {
  char cmd[] = "make clean -C %s/%s/%s/";
  char *clean_cmd;

  asprintf(&clean_cmd, cmd, workspace_root, platform, prj_dir);
  exec_command(clean_cmd);
  return 0;
}

/* TODO - add error checks and return error codes*/
int generic_cmd(char *params[]) {
  char cmd[] = "%s %s";
  char *gen_cmd;

  asprintf(&gen_cmd, cmd, params[KEY('g')], params[KEY('r')]);
  exec_command(gen_cmd);
  return 0;
}

/* TODO - add error checks and return error codes*/
int build_prj(char *params[])
{
  if(params[KEY('t')] != NULL)
    build_at_prj(params[KEY('a')], params[KEY('p')]);
  else if(params[KEY('m')] != NULL)
    build_m_prj(params[KEY('a')], params[KEY('p')], params[KEY('m')]);
  else if(params[KEY('k')] != NULL)
    build_cm_prj(params[KEY('a')], params[KEY('p')]);
  else
    return -1;

  return 0;
}

/* TODO - add error checks and return error codes*/
int clean_prj(char *params[])
{
  if(params[KEY('t')] != NULL)
    clean_at_prj(params[KEY('a')], params[KEY('p')]);
  else if(params[KEY('m')] != NULL)
    clean_m_prj(params[KEY('a')], params[KEY('p')]);
  else if(params[KEY('k')] != NULL)
    clean_cm_prj(params[KEY('a')], params[KEY('p')]);
  else
    return -1;

  return 0;
}

/* TODO - add error checks and return error codes*/
int process_params(char *params[]) {
  int i;
  for (i = 0; i< KEY_ARR_SZ; i++){
    if(params[i] != NULL) {
      switch ('a' + i) {
      case 'b':
        build_prj(params);
        break;
      case 'c':
        clean_prj(params);
        break;
      case 'g':
        generic_cmd(params);
        break;
      }
    }
  }
  return 0;
}

/* TODO - send port map to CODI*/
int register_agent(char *turff_ops[]) {
  int turff_sock_fd;
  struct addrinfo *addr_p;
  char *turff_id;

  addr_p = connect_to_socket(turff_ops[KEY('i')], turff_ops[KEY('d')], &turff_sock_fd);

  if(addr_p == NULL) {
    INFO("Please ensure that CODI is running\n");
    ERROR("Could not connect to CODI\n");
  } else {
    INFO("Connected to codi on %s:%s\n",
      turff_ops[KEY('i')], turff_ops[KEY('d')]);
  }

  if (turff_ops[KEY('n')] == NULL) {
    turff_id = getenv(TURFFID);
    if(turff_id == NULL) {
      INFO("Unable to read TURFFID from the environment\n");
      INFO("Setting TURFFID to \"default\"\n");
      turff_ops[KEY('n')] = "default" ;
    } else
      turff_ops[KEY('n')] = turff_id ;
  }
  sleep(2);
  send_args(turff_sock_fd, turff_ops);

  return 0;
}
