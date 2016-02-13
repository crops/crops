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
#include <jansson.h>
#include <string.h>
#include <curl/curl.h>
#include <ctype.h>
#include "codi_launcher.h"

/* execute the request and return an array of json objects */
json_t *request_json(char *request_method, char *url, char *api, char *data)
{
  char *in = 0, *j_start;
  json_t *root;
  json_error_t err;

  in = curl_request(request_method, url, api, data);

  /* skip the header and move to the json array */
  j_start = strchr(in, '[');
  root = json_loads(j_start, 0, &err);
  free(in);
  return root;
}

/* execute the request and return the raw server reply */
char *curl_request(char *http_method, char *url, char *api,  char *data)
{

  CURL *curl = NULL;
  CURLcode res;
  struct curl_slist *api_hdr = NULL;
  curl_mem_chunk_t srv_reply ;
  char *request_str ;

  srv_reply.mem = calloc(1, 1) ;

  if (srv_reply.mem == NULL)
    ERR("ERROR: Unable to allocate memory");

  srv_reply.size = 0 ;
  curl = curl_easy_init();

  if (curl) {
    /* check if url is a unix socket*/
    if (url[0] == '/') {
      /* map the unix socket as a connection endpoint */
      curl_easy_setopt(curl, CURLOPT_UNIX_SOCKET_PATH, url);
      asprintf(&request_str, "%s%s", "http:", api);
    } else {
        asprintf(&request_str, "%s%s%s",  "http://", url, api);
    }

    curl_easy_setopt(curl, CURLOPT_URL, request_str);

    if (!strcmp(http_method, POST_REQUEST)) {
      curl_easy_setopt(curl, CURLOPT_POST, 1L);
      api_hdr = curl_slist_append(api_hdr, "Content-Type: application/json");
      api_hdr = curl_slist_append(api_hdr, "Expect:");
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, api_hdr);

      if (data == NULL)
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
      else
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    }

    curl_easy_setopt(curl, CURLOPT_HEADER, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &srv_reply);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, process_srv_reply);

#ifdef DBG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
#endif

    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
      INFO("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

    if (!strcmp(http_method, POST_REQUEST))
      curl_slist_free_all(api_hdr);

    free(request_str);
    curl_easy_cleanup(curl);
  }

  return srv_reply.mem;
}

size_t process_srv_reply(void *ptr, size_t size, size_t nmemb, void *reply_p)
{
  size_t chunk_size = size * nmemb;

  curl_mem_chunk_t *mem_chunk = (curl_mem_chunk_t *)reply_p;

  /* append chunk_size memory block to the original memory block */
  mem_chunk->mem = realloc(mem_chunk->mem, mem_chunk->size + chunk_size + 1);
  if (mem_chunk->mem == NULL)
    ERR("not enough memory (realloc returned NULL)\n");

  INFO("%s", ptr);

  /* copy the data into the newly allocated chunk */
  memcpy(&(mem_chunk->mem[mem_chunk->size]), ptr, chunk_size);
  mem_chunk->size += chunk_size;
  mem_chunk->mem[mem_chunk->size] = 0;
  return chunk_size;
}

bool is_container_running(char *url, char *cont_name)
{
  char *api_str = NULL;
  json_t *result;

  asprintf(&api_str, RUNNING_NAMED_CONTAINER, cont_name);
  result = request_json(GET_REQUEST, url, api_str, NULL);
  free(api_str);

  if (json_array_size(result)) {
    json_decref(result);
    return true;
  } else {

#ifdef DBG
  INFO("Container : %s is not running\n", cont_name);
#endif

    json_decref(result);
    return false;
  }
}
