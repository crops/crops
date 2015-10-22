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

#include <stdio.h>
#include <stdlib.h>
#include "globals.h"

void info(const char *file, int line, const char *func, const char* fmt, ...) {
  va_list args;

  printf("[INFO] ");
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

void debug(const char *file, int line, const char *func, const char* fmt, ...) {
  va_list args;

  printf("[DEBUG] %s:%d: [%s] ", file, line, func);
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

void error(const char *file, int line, const char *func, const char* fmt, ...) {
  va_list args;

  printf("[ERROR] %s:%d: [%s] ", file, line, func);
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
  exit(EXIT_FAILURE);
}
