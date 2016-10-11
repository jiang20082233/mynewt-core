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

#include <assert.h>
#include "bootutil/bootutil_misc.h"
#include "bootutil/image.h"
#include "bootutil/loader.h"
#include "imgmgr/imgmgr.h"
#include "split/split.h"
#include "split_priv.h"

#define LOADER_IMAGE_SLOT   0
#define SPLIT_IMAGE_SLOT    1
#define SPLIT_TOTAL_IMAGES  2

void
split_app_init(void)
{
    int rc;

    rc = split_conf_init();
    assert(rc == 0);

    rc = split_nmgr_register();
    assert(rc == 0);
}

split_status_t
split_check_status(void)
{
    void *entry;
    int rc;

    rc = split_go(LOADER_IMAGE_SLOT, SPLIT_IMAGE_SLOT, &entry);
    switch (rc) {
    case SPLIT_GO_ERR:
        return SPLIT_STATUS_INVALID;

    case SPLIT_GO_NON_MATCHING:
        return SPLIT_STATUS_NOT_MATCHING;

    case SPLIT_GO_OK:
        return SPLIT_STATUS_MATCHING;

    default:
        assert(0);
        return SPLIT_STATUS_INVALID;
    }
}

/**
 * This validates and provides the loader image data
 *
 * @return                      0 on success; nonzero on failure.
 */
int
split_app_go(void **entry, int toboot)
{
    boot_split_mode_t split_mode;
    int run_app;
    int rc;

    if (toboot) {
        split_mode = boot_split_mode_get();

        /* if we are told not to, then we don't boot an app */
        if (split_mode == BOOT_SPLIT_MODE_LOADER) {
            return -1;
        }

        /* if this is a one-time test, reset the split mode */
        switch (split_mode) {
        case BOOT_SPLIT_MODE_LOADER:
            run_app = 0;
            break;

        case BOOT_SPLIT_MODE_TEST_APP:
            split_write_split(BOOT_SPLIT_MODE_LOADER);
            run_app = 1;
            break;

        case BOOT_SPLIT_MODE_TEST_LOADER:
            split_write_split(BOOT_SPLIT_MODE_APP);
            run_app = 0;
            break;

        case BOOT_SPLIT_MODE_APP:
            run_app = 1;
            break;

        default:
            run_app = 0;
            break;
        }

        if (!run_app) {
            return -1;
        }
    }

    rc = split_go(LOADER_IMAGE_SLOT, SPLIT_IMAGE_SLOT, entry);
    return rc;
}
