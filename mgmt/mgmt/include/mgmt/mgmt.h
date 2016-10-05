/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef _MGMT_MGMT_H_
#define _MGMT_MGMT_H_

#include <inttypes.h>
#include <json/json.h>

#include <os/queue.h>

#ifdef __cplusplus
extern "C" {
#endif

/* MTU for newtmgr responses */
#define MGMT_MAX_MTU 1024

#ifndef STR
/* Stringification of constants */
#define STR(x) #x
#endif


/* First 64 groups are reserved for system level newtmgr commands.
 * Per-user commands are then defined after group 64.
 */
#define MGMT_GROUP_ID_DEFAULT   (0)
#define MGMT_GROUP_ID_IMAGE     (1)
#define MGMT_GROUP_ID_STATS     (2)
#define MGMT_GROUP_ID_CONFIG    (3)
#define MGMT_GROUP_ID_LOGS      (4)
#define MGMT_GROUP_ID_CRASH     (5)
#define MGMT_GROUP_ID_SPLIT     (6)
#define MGMT_GROUP_ID_PERUSER   (64)

/**
 * Newtmgr JSON error codes
 */
#define MGMT_ERR_EOK      (0)
#define MGMT_ERR_EUNKNOWN (1)
#define MGMT_ERR_ENOMEM   (2)
#define MGMT_ERR_EINVAL   (3)
#define MGMT_ERR_ETIMEOUT (4)
#define MGMT_ERR_ENOENT   (5)
#define MGMT_ERR_EPERUSER (256)

struct mgmt_jbuf {
    /* json_buffer must be first element in the structure */
    struct json_buffer mjb_buf;  /* for input */
    struct json_encoder mjb_enc; /* for output */
};

typedef int (*mgmt_handler_func_t)(struct mgmt_jbuf *);

struct mgmt_handler {
    mgmt_handler_func_t mh_read;
    mgmt_handler_func_t mh_write;
};

struct mgmt_group {
    struct mgmt_handler *mg_handlers;
    uint16_t mg_handlers_count;
    uint16_t mg_group_id;
    STAILQ_ENTRY(mgmt_group) mg_next;
};

#define MGMT_GROUP_SET_HANDLERS(__group, __handlers)       \
    (__group)->mg_handlers = (__handlers);                 \
    (__group)->mg_handlers_count = (sizeof((__handlers)) / \
            sizeof(struct mgmt_handler));

int mgmt_group_register(struct mgmt_group *group);
void mgmt_jbuf_setoerr(struct mgmt_jbuf *njb, int errcode);
struct mgmt_handler *mgmt_find_handler(uint16_t group_id, uint16_t handler_id);

#ifdef __cplusplus
}
#endif

#endif /* _MGMT_MGMT_H_ */