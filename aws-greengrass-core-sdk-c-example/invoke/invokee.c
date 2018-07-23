/*
 * Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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
    char input[MESSAGE_SIZE];
    char output[MESSAGE_SIZE];
    char err_output[MESSAGE_SIZE];
    size_t amount_read = 0;
    int ret = 0;

    memset(input, 0, MESSAGE_SIZE);
    memset(output, 0, MESSAGE_SIZE);
    memset(err_output, 0, MESSAGE_SIZE);

    gg_log(GG_LOG_INFO, "Client context: %s", cxt->client_context);

    /* Read input event for the lambda to process. */
    err = loop_lambda_request_read(input, MESSAGE_SIZE, &amount_read);
    if(err) {
        gg_log(GG_LOG_ERROR,
                "Failed to read data. amount_read(%zu), amount_requested(%zu), err(%d)" ,
                amount_read, sizeof(input), err);

        goto cleanup;
    }


    /* Process the input. In this case, we are doing string concatenation. */
    ret = snprintf(output, MESSAGE_SIZE, "%s world", input);
    if(ret < 0 || ret >= MESSAGE_SIZE) {
        gg_log(GG_LOG_ERROR, "Failed to format output. return code: %d", ret);
        /* Send error back to invoker when failed to process input. */
        sprintf(err_output, "Failed to process event: %d", ret);
        err = gg_lambda_handler_write_error(err_output);
        if(err) {
            gg_log(GG_LOG_ERROR, "Failed to send error back: %d", err);
        }
        goto cleanup;
    }

    gg_log(GG_LOG_INFO, "Return output: %s size: %d", output, ret);
    err = gg_lambda_handler_write_response(output, ret);
    if(err) {
        gg_log(GG_LOG_ERROR, "Failed to send reponse: %d", err);
        goto cleanup;
    }

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

    /* start the runtime in blocking mode. This blocks forever. */
    gg_runtime_start(handler, 0);

cleanup:
    return -1;
}
