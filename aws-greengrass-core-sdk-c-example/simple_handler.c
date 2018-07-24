/*
 * Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * This example show several key concept of writing native lambda function:
 * 1. how to write a simple lambda handler.
 * 2. how to start the runtime using "gg_runtime_start".
 * 3. how to read input event with fixed size buffer using "gg_lambda_handler_read".
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "greengrasssdk.h"

#define MESSAGE_SIZE 100

/* loop read the request bytes into buffer. */
gg_error loop_lambda_request_read(void *buffer,
        size_t buffer_size, size_t *total_read) {
    gg_error err = GGE_SUCCESS;
    uint8_t *read_index = (uint8_t*)buffer;
    size_t remaining_buf_size = buffer_size;
    size_t amount_read = 0;

    do {
        err = gg_lambda_handler_read(read_index, remaining_buf_size,
                &amount_read);
        if(err) {
            gg_log(GG_LOG_ERROR, "gg_lambda_handler_read had an error");
            goto cleanup;
        }
        *total_read += amount_read;
        read_index += amount_read;
        remaining_buf_size -= amount_read;
    } while(amount_read);

cleanup:
    return err;
}

void handler(const gg_lambda_context *cxt) {
    gg_error err = GGE_SUCCESS;
    char message[MESSAGE_SIZE];
    size_t amount_read = 0;

    memset(message, 0, MESSAGE_SIZE);

    gg_log(GG_LOG_INFO, "function arn: [%s]", cxt->function_arn);
    gg_log(GG_LOG_INFO, "client context: [%s]", cxt->client_context);

    err = loop_lambda_request_read(message, sizeof(message), &amount_read);
    if(err) {
        gg_log(GG_LOG_ERROR,
                "Failed to read data. amount_read(%zu), amount_requested(%zu), err(%d)" ,
                amount_read, sizeof(message), err);

        goto cleanup;
    }

    gg_log(GG_LOG_INFO, "Received message: [%.*s] size: [%d]", amount_read, message, amount_read);

cleanup:
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
