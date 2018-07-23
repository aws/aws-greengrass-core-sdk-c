/*
 * Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * This example shows publishing a "hello world" message to cloud in a
 * continuous loop. It requires a subscription being set up properly in
 * Greengrass Group with source as the function ARN of native function, target
 * as IoT Cloud, and topic as "hello/world".
 *
 */

#include <unistd.h>
#include "greengrasssdk.h"

#define MESSAGE_SIZE 100

void handler(const gg_lambda_context *cxt) {
    (void)cxt;
    return;
}

int main() {
    gg_error err = GGE_SUCCESS;
    gg_request ggreq = NULL;
    gg_request_result result;
    const char message[] = "hello world!";
    const char topic[] = "hello/world";

    err = gg_global_init(0);
    if(err) {
        gg_log(GG_LOG_ERROR, "gg_global_init failed %d", err);
        goto cleanup;
    }

    gg_runtime_start(handler, GG_RT_OPT_ASYNC);

    err = gg_request_init(&ggreq);
    if(err) {
        gg_log(GG_LOG_ERROR, "Failed to initialize request");
        goto cleanup;
    }

    for(;;) {
        err = gg_publish(ggreq, topic, message, sizeof(message), &result);
        if(err) {
            gg_log(GG_LOG_ERROR, "gg_publish failed: %d", err);
            goto cleanup;
        }

        sleep(3);
    }

cleanup:
    return -1;
}
