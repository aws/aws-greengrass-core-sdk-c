/*
 * Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "greengrasssdk.h"

#define ERROR_BUFFER_SIZE 128
#define RESPONSE_SIZE 128

/* loop read the request bytes into buffer. */
gg_error loop_request_read(gg_request ggreq, void *buffer,
        size_t buffer_size, size_t *total_read) {
    gg_error err = GGE_SUCCESS;
    uint8_t *read_index = (uint8_t*)buffer;
    size_t remaining_buf_size = buffer_size;
    size_t amount_read = 0;

    do {
        err = gg_request_read(ggreq, read_index, remaining_buf_size,
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
gg_error invoke_a_lambda() {
    gg_error err = GGE_SUCCESS;
    gg_request ggreq = NULL;
    /* payload should be a binary payload without null terminator. */
    char payload[] = {'h', 'e', 'l', 'l', 'o'};
    size_t amount_read = 0;
    /* example context: base64-encoded json { "custom":{ "value": "key" }} */
    char customer_context[] = "eyAiY3VzdG9tIjp7ICJ2YWx1ZSI6ICJrZXkiIH19";
    char qualifier[] = "1";
    char response[RESPONSE_SIZE];
    char error[ERROR_BUFFER_SIZE];
    struct gg_request_result result;

    memset(response, 0, RESPONSE_SIZE);
    memset(error, 0, ERROR_BUFFER_SIZE);

    gg_invoke_options opts = {
        /* Fill in your PlusLambda arn eg arn:aws:lambda:us-west-2:123456789012:function:Invokee:1 */
        .function_arn = "arn:aws:lambda:us-west-2:123456789012:function:Invokee:1",
        .customer_context = customer_context,
        .qualifier = qualifier,
        .type = GG_INVOKE_REQUEST_RESPONSE,
        .payload = payload,
        .payload_size = sizeof(payload)
    };

    /* Initialize the request handle. */
    err = gg_request_init(&ggreq);
    if(err) {
        gg_log(GG_LOG_ERROR, "Failed to initialize request");
        goto done;
    }

    err = gg_invoke(ggreq, &opts, &result);
    if(err) {
        gg_log(GG_LOG_ERROR, "gg_invoke failed with client error: %d", err);
        goto done;
    }

    switch(result.request_status) {
    case GG_REQUEST_SUCCESS:
        /* The invocation was successful. Read the response from the lambda function. */
        err = loop_request_read(ggreq, response, RESPONSE_SIZE, &amount_read);
        if(err) {
            gg_log(GG_LOG_ERROR,
                "Failed to read data. amount_read(%zu), amount_requested(%zu), err(%d)",
                amount_read, sizeof(response), err);
            goto cleanup;
        }

        gg_log(GG_LOG_INFO, "Response: %s read: %d", response, amount_read);
        break;
    case GG_REQUEST_HANDLED:
        /* The invocation was successful, however the lambda responded with an error.
         * Read the error message. */
    case GG_REQUEST_UNHANDLED:
        /* The invocation was unsuccessful, because lambda exits abnormally.
         * Read the error message. */
        err = loop_request_read(ggreq, error, ERROR_BUFFER_SIZE, &amount_read);
        if(err) {
            gg_log(GG_LOG_ERROR,
                "Failed to read error message. amount_read(%zu), ERROR_BUFFER_SIZE(%zu), err(%d)",
                amount_read, ERROR_BUFFER_SIZE, err);
            goto cleanup;
        }

        gg_log(GG_LOG_INFO, "Lambda invocation failed. error: %s status: %d",
                error, result.request_status);
        break;
    default:
        gg_log(GG_LOG_ERROR, "Some other errors happened: %d", result.request_status);
    }

cleanup:
    gg_request_close(ggreq);

done:
    return err;
}

void handler(const gg_lambda_context *cxt) {
    /* cxt is not used. */
    (void)cxt;

    invoke_a_lambda();
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
