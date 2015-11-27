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

#include <jansson.h>
#include <string.h>
#include <curl/curl.h>
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
char *request(char *request_method, char *url, char *api, char *data)
{
  char *in ;

  in = curl_request(request_method, url, api, data);
  return in;
}

char *curl_request(char *http_method, char *url, char *api,  char *data)
{

  CURL *curl = NULL;
  CURLcode res;
  struct curl_slist *api_hdr = NULL;
  curl_mem_chunk_t srv_reply ;
  char *request_str ;

  srv_reply.mem = malloc(1) ;

  if (srv_reply.mem == NULL)
    ERROR("ERROR: Unable to allocate memory");

  srv_reply.size = 0 ;
  curl = curl_easy_init();

  if (curl) {
    /* check if url is a unix socket*/
    if (url[0] == '/') {
      /* map the unix socket as a connection endpoint */
      curl_easy_setopt(curl, CURLOPT_UNIX_SOCKET_PATH, url);

    if (!strcmp(http_method, POST_REQUEST))
      asprintf(&request_str, "%s%s", "http:", api);
    else
      asprintf(&request_str, "'%s%s'", "http:", api);
    } else {
      if (!strcmp(http_method, POST_REQUEST))
        asprintf(&request_str, "%s%s", url, api);
      else
        asprintf(&request_str, "'%s%s'", url, api);
    }

    curl_easy_setopt(curl, CURLOPT_URL, request_str);

    if (!strcmp(http_method, POST_REQUEST)) {
      curl_easy_setopt(curl, CURLOPT_POST, 1L);
      api_hdr = curl_slist_append(api_hdr, "Content-Type: application/json");
      api_hdr = curl_slist_append(api_hdr, "Expect:");
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, api_hdr);

      if (data == NULL)
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, " ");
      else
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    }

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
    ERROR("not enough memory (realloc returned NULL)\n");

  INFO("%s", ptr);

  /* copy the data into the newly allocated chunk */
  memcpy(&(mem_chunk->mem[mem_chunk->size]), ptr, chunk_size);
  mem_chunk->size += chunk_size;
  mem_chunk->mem[mem_chunk->size] = 0;
  return chunk_size;
}


bool start_existing_container(char *url, char *cont_name)
{
  char *api_str = NULL, *result;

  asprintf(&api_str, START_CONTAINER, cont_name);
  result = request(POST_REQUEST, url, api_str, NULL);
  free(api_str);

  if (strstr(result, NO_ERROR_204) != NULL ||
      strstr(result, NOT_MODIFIED_304) != NULL) {
    free(result);
    return true;
  } else {
    free(result);
    return false;
  }
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
    json_decref(result);
    return false;
  }
}

bool container_exists(char *url, char *cont_name)
{
  char *api_str = NULL;
  json_t *result;

  asprintf(&api_str, NAMED_CONTAINER, cont_name);
  result = request_json(GET_REQUEST, url, api_str, NULL);
  free(api_str);

  if (json_array_size(result)) {
    json_decref(result);
    return true;
  } else {
    json_decref(result);
    return false;
  }
}

bool download_img(char *url, char *repo, char *tag)
{
  char *api_str = NULL, *result;

  asprintf(&api_str, GET_IMAGE, repo, tag);
  result = request(POST_REQUEST, url, api_str, NULL);
  free(api_str);

  if (strstr(result, PULL_COMPLETE) != NULL ||
    strstr(result, IMAGE_UP_TO_DATE) != NULL) {
    free(result);
    return true;
  } else {
    free(result);
    return false;
  }
}

bool start_new_container(char *url, char *img, char *host_dir)
{
  char *cont_params = NULL, *result;

char *cont_definition = CONTAINER_PARAMS(
  {
    "Hostname": "",
    "Domainname": "",
    "User": "",
    "AttachStdin": false,
    "AttachStdout": false,
    "AttachStderr": false,
    "Tty": false,
    "OpenStdin": false,
    "StdinOnce": false,
    "Env": null,
    "Cmd": [
      "/bin/crops"
    ],
    "Entrypoint": null,
    "Image": "%s",
    "Labels": null,
    "Mounts": [
      {
        "Source": "%s",
        "Destination": "/crops",
        "Mode": "",
        "RW": true
      }
    ],
    "WorkingDir": "",
    "NetworkDisabled": false,
    "ExposedPorts": null,
    "StopSignal": "SIGTERM",
    "HostConfig": {
      "Binds": ["%s:/crops"],
      "Links": null,
      "LxcConf": null,
      "Memory": 0,
      "MemorySwap": 0,
      "MemoryReservation": 0,
      "KernelMemory": 0,
      "CpuShares": 0,
      "CpuPeriod": 0,
      "CpuQuota": 0,
      "CpusetCpus": "",
      "CpusetMems": "",
      "BlkioWeight": 0,
      "MemorySwappiness": null,
      "OomKillDisable": false,
      "PortBindings": null,
      "PublishAllPorts": true,
      "Privileged": false,
      "ReadonlyRootfs": false,
      "Dns": null,
      "DnsOptions": null,
      "DnsSearch": null,
      "ExtraHosts": null,
      "VolumesFrom": null,
      "CapAdd": null,
      "CapDrop": null,
      "RestartPolicy": { "Name": "", "MaximumRetryCount": 0 },
      "NetworkMode": "host",
      "Devices": null,
      "Ulimits": null,
      "LogConfig": { "Type": "json-file", "Config": {} },
      "SecurityOpt": null,
      "CgroupParent": "",
      "VolumeDriver": ""
    }
  }
);

  asprintf(&cont_params, cont_definition, img, host_dir, host_dir);
  result = request(POST_REQUEST, url, CREATE_CONTAINER, cont_params);
  free(cont_params);

  if (strstr(result, CREATED_201) != NULL) {
    free(result);
    return true;
  } else {
    free(result);
    return false;
  }
}
