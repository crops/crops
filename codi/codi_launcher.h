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

#ifndef CODI_LAUNCHER_H
#define CODI_LAUNCHER_H

#include <jansson.h>
#include "globals.h"


/* Docker Remote API requests */
#define GET_REQUEST              "GET"
#define POST_REQUEST             "POST"
#define DOCKER_UNIX_SOCKET       "/var/run/docker.sock"
#define DOCKER_ENG_IP            "127.0.0.1"
#define DOCKER_ENG_PORT       	 "2376"
#define GET_IMAGES               "/images/json"
#define ALL_CONTAINERS           "/containers/json?all=true"
#define START_CONTAINER          "/containers/%s/start"
#define CREATE_CONTAINER         "/containers/create?name=%s"
#define GET_IMAGE                "/images/create?fromImage=%s&tag=%s"

#define NAMED_CONTAINER          "/containers/json?all=true&filters=" \
                                 "{%%22name%%22:[%%22%s%%22]}"

#define RUNNING_NAMED_CONTAINER  "/containers/json?all=true&filters=" \
                                 "{%%22name%%22:[%%22%s%%22],%%22status%%22:[%%22running%%22]}"

/* Docker Engine Responses */
#define PULL_COMPLETE      "Pull complete"
#define IMAGE_UP_TO_DATE   "Image is up to date"
#define NO_ERROR_204       "204 No Content"
#define NOT_MODIFIED_304   "304 Not Modified"
#define CREATED_201        "201 Created"
#define OK_200             "200 OK"

#define CONTAINER_PARAMS(...) #__VA_ARGS__

char *curl_request(char *http_method, char *url, char *api,  char *data);
size_t process_srv_reply(void *ptr, size_t size, size_t nmemb, void *reply_p);
json_t *request_json(char *request_method, char *url, char *api, char *data);
bool is_container_running(char *url, char *cont_name);
bool start_container(char *url, char *cont_name);

#endif
