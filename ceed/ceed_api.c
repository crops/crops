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
#include <unistd.h>
#include <ctype.h>

char *ceed_ops[KEY_ARR_SZ];

void print_ceed_usage(int argc, char *argv[]) {

  printf("Usage: %s -p prj -i 192.168.99.100 -s 10000 -b\n", argv[0]);
  printf("	-i CODI ip address [default: 127.0.0.1 (linux) or 192.168.99.100 (win/mac)]\n");
  printf("	-s CODI listening port [default: 10000]\n");
  printf("	-l list available build containers\n");
  printf("	-d container_id. Send command to container_id\n");
  printf("	-p prj_name [required]\n");
  printf("	-b build project [NOTE: -b and -c are mutually exclusive]\n");
  printf("	-c clean project [NOTE: -c and -b are mutually exclusive]\n");
  printf("	-t autotools project [NOTE: -t and -k and -m are mutually exclusive]\n");
  printf("	-k cmake project [NOTE: -k and -t and -m are mutually exclusive]\n");
  printf("	-m target make makefile_target [NOTE: -m and -t and -k are mutually exclusive]\n");
  printf("	-g gen_cmd execute generic command\n");
  printf("	-r args generic command arguments\n");
  printf("	-e env environment variables\n");
  printf("	-h print this help menu\n");
  printf("	-v show ceed version\n");
  exit(EXIT_FAILURE);
}

void init_ceed_params(){
#if defined(_WIN32) || defined(__CYGWIN__)
  ceed_ops[KEY('a')] = "win";
  ceed_ops[KEY('i')] = CODI_IP_VIRT;
#elif __APPLE__
  ceed_ops[KEY('a')] = "mac";
  ceed_ops[KEY('i')] = CODI_IP_VIRT;
#elif __linux
  ceed_ops[KEY('a')] = "linux";
  ceed_ops[KEY('i')] = CODI_IP_NATIVE;
#endif
  ceed_ops[KEY('p')] = NULL;
  ceed_ops[KEY('s')] = CODI_PORT;
  ceed_ops[KEY('v')] = VERSION;
  ceed_ops[KEY('r')] = " ";
  ceed_ops[KEY('z')] = CEED_NAME;
}

void parse_ceed_params(int argc, char *argv[]) {
  int c, b_flg = 0, c_flg = 0, t_flg = 0, m_flg = 0, k_flg = 0;

  init_ceed_params();

  while((c = getopt(argc, argv, "hvltm:kbcp:i:s:g:r:d:")) != -1) {
    switch (c) {
    case 'h':
      print_ceed_usage(argc, argv);
      exit(EXIT_SUCCESS);
      break;
    case 'v':
      INFO("ceed version : %s\n", VERSION);
      exit(EXIT_SUCCESS);
      break;
    case 'p':
      ceed_ops[KEY('p')] = optarg;
      break;
    case 'i':
      ceed_ops[KEY('i')] = optarg;
      break;
    case 's':
      ceed_ops[KEY('s')] = optarg;
      break;
    case 'g':
      ceed_ops[KEY('g')] = optarg;
      break;
    case 'r':
      ceed_ops[KEY('r')] = optarg;
      break;
    case 'd':
      ceed_ops[KEY('d')] = optarg;
      break;
    case 'l':
        asprintf(&ceed_ops[KEY('l')] , "set");
      break;
    case 'b':
      if(c_flg) {
        print_ceed_usage(argc, argv);
      } else {
        b_flg++;
        asprintf(&ceed_ops[KEY('b')] , "set");
      }
      break;
    case 'c':
      if(b_flg) {
        print_ceed_usage(argc, argv);
      } else {
        c_flg++;
        asprintf(&ceed_ops[KEY('c')] , "set");
      }
      break;
    case 't':
      if(m_flg || k_flg) {
        print_ceed_usage(argc, argv);
      } else {
        t_flg++;
        asprintf(&ceed_ops[KEY('t')] , "set");
      }
      break;
    case 'm':
      if(t_flg || k_flg)
        print_ceed_usage(argc, argv);
      else {
        m_flg++;
        ceed_ops[KEY('m')] = optarg;
      }
      break;
    case 'k':
      if(t_flg || m_flg)
        print_ceed_usage(argc, argv);
      else {
        k_flg++;
        asprintf(&ceed_ops[KEY('k')] , "set");
      }
      break;
    case '?':
      if (optopt == 'p' || optopt == 'i' || optopt == 's' || optopt == 'm')
        ERR("Option -%c requires an argument.\n", optopt);
      else if (isprint(optopt))
        ERR("Unknown option `-%c'.\n", optopt);
      else
        ERR("Unknown option character `\\x%x'.\n", optopt);
    default:
      print_ceed_usage(argc, argv);
    }
  }

  if ((ceed_ops[KEY('l')] == NULL && ceed_ops[KEY('p')] == NULL && ceed_ops[KEY('g')] == NULL )
    || (ceed_ops[KEY('i')] == NULL || ceed_ops[KEY('s')] == NULL)) {
    print_ceed_usage(argc, argv);
  }
}

