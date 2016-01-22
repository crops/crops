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

#ifndef GLOBALS_H
#define GLOBALS_H

#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define KEY_ARR_SZ 'z' -'a' + 1
#define KEY(var) abs('a'- #var[1])


#define CODI_PORT      "10000"
#define CODI_IP_NATIVE "127.0.0.1"
#define CODI_IP_VIRT   "192.168.99.100"
#define TURFF_PORT     "9999"
#define MSG_TERM       "EOM"
#define VERSION        "0.1"
#define TURFF_EOM      "TEOM"
#define CEED_NAME      "ceed"
#define TURFF_NAME     "turff"
#define CODI_NAME      "codi"
#define TURFFID        "TURFFID"

void info(const char *file, int line, const char *func, const char* fmt, ...);
void debug(const char *file, int line, const char *func, const char* fmt, ...);
void error(const char *file, int line, const char *func, const char* fmt, ...);

#define INFO(...)  info(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define ERROR(...) error(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define DEBUG(...) debug(__FILE__, __LINE__, __func__, __VA_ARGS__)

typedef enum { false, true } bool;

/* parameter chunk*/
typedef struct chunk{
  uint32_t op_code;
  char arg[20];
  struct chunk *next;
} msg_chunk;

typedef struct{
  char  *mem;
  size_t size;
} curl_mem_chunk_t;

#endif
