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
#include "globals.h"

char *codi_ops[KEY_ARR_SZ];

void print_codi_usage(int argc, char *argv[]) {
  printf("Usage: %s \n", argv[0]);
  printf(" -s port number on which CODI is listening [default: 10000]\n");
  printf(" -h print this help menu\n");
  printf(" -v show CODI version\n");
}

void init_codi_params(){
  int i;

  for (i = 0; i <  KEY_ARR_SZ; i++)
    codi_ops[i] = NULL;

  codi_ops[KEY('s')] = CODI_PORT;
  codi_ops[KEY('v')] = VERSION;
  codi_ops[KEY('z')] = CODI_NAME;
}

void parse_codi_params(int argc, char *argv[]) {
  int c ;

  init_codi_params() ;

  while ((c = getopt(argc, argv, "hvs:")) != -1) {
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
    case '?':
      if (optopt == 's')
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
