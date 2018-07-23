/*
 * Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * This example shows publishing message to cloud from lambda function. It
 * requires a subscription being set up properly in Greengrass Group with source
 * as the function ARN of native function, target as IoT Cloud, and topic as
 * "to/cloud".
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "greengrasssdk.h"

#define MESSAGE_SIZE 100

void handler(const gg_lambda_context *cxt) {
    gg_error err = GGE_SUCCESS;
    gg_request ggreq = NULL;
    const char topic[] = "to/cloud";
    char message[MESSAGE_SIZE];
    int ret = 0;
    gg_request_result result;

    memset(message, 0, MESSAGE_SIZE);

    err = gg_request_init(&ggreq);
    if(err) {
        gg_log(GG_LOG_ERROR, "Failed to initialize request");
        goto cleanup;
    }

    ret = sprintf(message, "hello from: %s", cxt->function_arn);
    if(ret < 0) {
        gg_log(GG_LOG_ERROR, "failed to format message with ret: %d", ret);
        goto cleanup;
    }

    err = gg_publish(ggreq, topic, message, MESSAGE_SIZE, &result);
    if(err) {
        gg_log(GG_LOG_ERROR, "gg_publish failed: %d", err);
        goto cleanup;
    }

    if(result.request_status) {
        gg_log(GG_LOG_ERROR, "gg_publish had result request_status %d",
            result.request_status);
        goto cleanup;
    }

cleanup:
    gg_request_close(ggreq);
    return;
}

int main() {
    gg_error err = GGE_SUCCESS;

    err = gg_global_init(0);
    if(err) {
        gg_log(GG_LOG_ERROR, "gg_global_init failed %d", err);
        goto cleanup;
    }

    gg_runtime_start(handler, 0);

cleanup:
    return -1;
}
