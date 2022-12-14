/* SPDX-License-Identifier: Apache-2.0 */
/* (c) Copyright 2021 - 2022 Xilinx, Inc. All rights reserved. */
/*
 * Net Driver Test Suite
 * Basic tests
 */

/**
 * @defgroup basic-read_sysfs Read driver sysfs files
 * @ingroup basic
 * @{
 *
 * @objective Check that the files in sysfs related to the tested
 *            driver can be read.
 *
 * @param env            Testing environment:
 *                      - @ref env-peer2peer
 *
 * @par Scenario:
 *
 * @note Scenarios: X3-ST08.
 *
 * @author Dmitry Izbitsky <Dmitry.Izbitsky@oktetlabs.ru>
 */

#define TE_TEST_NAME "basic/read_sysfs"

#include "net_drv_test.h"
#include "te_string.h"
#include "tapi_rpc_dirent.h"

int
main(int argc, char *argv[])
{
    rcf_rpc_server *iut_rpcs = NULL;
    const struct if_nameindex *iut_if = NULL;
    char *iut_drv_name = NULL;

    te_string path = TE_STRING_INIT;
    rpc_dir_p pdir = RPC_NULL;

    TEST_START;
    TEST_GET_PCO(iut_rpcs);
    TEST_GET_IF(iut_if);

    CHECK_NOT_NULL(iut_drv_name = net_drv_driver_name(iut_rpcs->ta));

    CHECK_RC(te_string_append(&path, "/sys/kernel/debug/%s/nic_%s/",
                              iut_drv_name, iut_if->if_name));

    TEST_STEP("Check whether driver/interface subdirectory can be found "
              "in /sys/kernel/debug/.");
    RPC_AWAIT_ERROR(iut_rpcs);
    pdir = rpc_opendir(iut_rpcs, path.ptr);
    if (pdir == RPC_NULL)
    {
        TEST_VERDICT("driver/interface subdirectory cannot be opened in "
                     "/sys/kernel/debug/");
    }
    else
    {
        rpc_closedir(iut_rpcs, pdir);
    }

    TEST_STEP("Read contents of all readable files under "
              "/sys/kernel/debug/[driver_module]/nic_[ifname]/.");
    rc = net_drv_cat_all_files(iut_rpcs, path.ptr);
    if (rc != 0)
        TEST_VERDICT("Failed to read files from /sys/kernel/debug/");

    te_string_reset(&path);
    CHECK_RC(te_string_append(&path, "/sys/module/%s/", iut_drv_name));

    TEST_STEP("Check whether driver subdirectory can be found "
              "in /sys/module/.");
    RPC_AWAIT_ERROR(iut_rpcs);
    pdir = rpc_opendir(iut_rpcs, path.ptr);
    if (pdir == RPC_NULL)
    {
        TEST_VERDICT("driver subdirectory cannot be opened in "
                     "/sys/module/");
    }
    else
    {
        rpc_closedir(iut_rpcs, pdir);
    }

    TEST_STEP("Read contents of all readable files under "
              "/sys/module/[driver_module]/.");
    rc = net_drv_cat_all_files(iut_rpcs, path.ptr);
    if (rc != 0)
        TEST_VERDICT("Failed to read files from /sys/kernel/module/");

    TEST_SUCCESS;

cleanup:

    free(iut_drv_name);
    te_string_free(&path);

    TEST_END;
}
