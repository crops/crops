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
char *curl_request(char *http_method, char *url, char *api,  char *data)
{

  CURL *curl = NULL;
  CURLcode res;
  struct curl_slist *api_hdr = NULL;
  curl_mem_chunk_t srv_reply ;
  char *request_str ;

  srv_reply.mem = calloc(1, 1) ;

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
  result = curl_request(POST_REQUEST, url, api_str, NULL);
  free(api_str);

  if ((strstr(result, NO_ERROR_204) != NULL) ||
    (strstr(result, NOT_MODIFIED_304) != NULL)) {
    free(result);
    return true;
  } else {

#ifdef DBG
  INFO("Unable to start container : %s. Status: %s\n", cont_name, result);
#endif

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

#ifdef DBG
  INFO("Container : %s is not running\n", cont_name);
#endif

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

#ifdef DBG
  INFO("Container : %s does not exist\n", cont_name);
#endif

    json_decref(result);
    return false;
  }
}

bool download_img(char *url, char *repo_tag)
{
  char *api_str = NULL, *repo, *tag, *tmp, *result;
  asprintf(&repo, repo_tag);
  tmp = strchr(repo, ':');

  if (!tmp)
    return false;

  tag = tmp + 1 ;
  *tmp = '\0';
  asprintf(&api_str, GET_IMAGE, repo, tag);
  result = curl_request(POST_REQUEST, url, api_str, NULL);
  free(api_str);
  free(repo);

  if (strstr(result, PULL_COMPLETE) != NULL ||
    strstr(result, IMAGE_UP_TO_DATE) != NULL ||
    strstr(result, OK_200) != NULL) {
    free(result);
    return true;
  } else {

#ifdef DBG
  INFO("Unable to download image : %s:%s. Status: %s\n", repo, tag, result);
#endif

    free(result);
    return false;
  }
}

bool create_new_container(char *url, char *img, char *host_dir)
{
  int i;
  char *cont_params = NULL, *tmp_img, *qry_str, *result;

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
    "Env": [
      "TURFFID=%s"
    ],
    "Cmd": [
      "/bin/crops"
    ],
    "Entrypoint": [
      "/bin/turff_launcher",
      "-f",
      "/opt/poky/environment-setup*"
    ],
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

  asprintf(&tmp_img, img);

  /* container names cannot have special characters */
  /* replace non alpha numberical characters with dashes */
  for(i = 0; i < strlen(tmp_img); i++) {
    if (!isalnum(tmp_img[i]))
      tmp_img[i] = '-';
  }


  asprintf(&cont_params, cont_definition, img, img, host_dir, host_dir);
  asprintf(&qry_str, CREATE_CONTAINER, tmp_img);
  free(tmp_img);
  result = curl_request(POST_REQUEST, url, qry_str, cont_params);
  free(cont_params);
  free(qry_str);



  if (strstr(result, CREATED_201) != NULL) {
    free(result);
    return true;
  } else {

#ifdef DBG
  INFO("Unable to create container : %s. Status: %s\n", img, result);
#endif

    free(result);
    return false;
  }
}

bool start_container(char *url, char *cont_name)
{
  int i;
  char *tmp_name;

  asprintf(&tmp_name, cont_name);

  /* container names cannot have special characters */
  /* replace non alpha numberical characters with dashes */
  for (i = 0; i < strlen(tmp_name); i++) {
    if (!isalnum(tmp_name[i]))
      tmp_name[i] = '-';
  }

  if (container_exists(url, tmp_name)) {
    if (start_existing_container(url, tmp_name)) {
      free(tmp_name);
      return true;
    }
  } else {
    if (download_img(url, cont_name)) {
      /* TODO host_dir will be different on Linux/Mac/Windows!!! */
      if (create_new_container(url, cont_name, "/crops")) {
        if (start_existing_container(url, tmp_name)) {
          free(tmp_name);
          return true;
        }
      }
    }
  }

  free(tmp_name);
  return false;
}
