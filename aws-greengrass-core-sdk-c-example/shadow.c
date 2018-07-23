/*
 * Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * This example shows how to update Greengrass local shadow and get the 
 * shadow document back.
 */

#include <stdio.h>
#include <stdlib.h>
#include "greengrasssdk.h"

#define BUFFER_SIZE 512

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
            gg_log(GG_LOG_ERROR, "gg_request_read had an error");
            goto cleanup;
        }
        *total_read += amount_read;
        read_index += amount_read;
        remaining_buf_size -= amount_read;
    } while(amount_read);

cleanup:
    return err;
}

gg_error update_thing_shadow() {
    gg_error err = GGE_SUCCESS;
    gg_request ggreq = NULL;
    const char thing_name[] = "foo";
    const char payload[] =
        "{"
            "\"state\": {"
                "\"desired\": {"
                    "\"mode\": \"OFF\""
                "}"
            "}"
        "}";
    size_t amount_read = 0;
    struct gg_request_result result;
    char read_buf[BUFFER_SIZE];

    err = gg_request_init(&ggreq);
    if(err) {
        gg_log(GG_LOG_ERROR, "Failed to initialize request");
        goto done;
    }

    err = gg_update_thing_shadow(ggreq, thing_name, payload, &result);
    if(err) {
        gg_log(GG_LOG_ERROR, "gg_update_thing_shadow failed with err %d",
            err);
        goto cleanup;
    }

    gg_log(GG_LOG_INFO, "gg_update_thing_shadow had result request_status %d",
            result.request_status);
    if(result.request_status != GG_REQUEST_SUCCESS) {
        // shadow update failed, reads error response
        err = loop_request_read(ggreq, read_buf, BUFFER_SIZE, &amount_read);
        if(err) {
            gg_log(GG_LOG_ERROR,
                "Failed to read shadow data. amount_read(%zu), BUFFER_SIZE(%zu), err(%d)",
                amount_read, BUFFER_SIZE, err);
            goto cleanup;
        }

        gg_log(GG_LOG_ERROR, "update_thing_shadow failed. error messsage: %.*s",
                (int)amount_read, read_buf);
    } else {
        // shadow update succeeded, reads success response
        err = loop_request_read(ggreq, read_buf, BUFFER_SIZE, &amount_read);
        if(err) {
            gg_log(GG_LOG_ERROR,
                "Failed to read shadow data. amount_read(%zu), BUFFER_SIZE(%zu), err(%d)",
                amount_read, BUFFER_SIZE, err);
            goto cleanup;
        }
        gg_log(GG_LOG_INFO, "update_thing_shadow succeed. response: %.*s",
                (int)amount_read, read_buf);
    }

cleanup:
    gg_request_close(ggreq);

done:
    return err;
}

gg_error get_thing_shadow() {
    gg_error err = GGE_SUCCESS;
    gg_request ggreq = NULL;
    const char thing_name[] = "foo";
    size_t amount_read = 0;
    struct gg_request_result result;
    char read_buf[BUFFER_SIZE];

    err = gg_request_init(&ggreq);
    if(err) {
        gg_log(GG_LOG_ERROR, "Failed to initialize request");
        goto done;
    }

    err = gg_get_thing_shadow(ggreq, thing_name, &result);
    if(err) {
        gg_log(GG_LOG_ERROR, "gg_get_thing_shadow failed with err %d", err);
        goto cleanup;
    }

    gg_log(GG_LOG_INFO, "gg_get_thing_shadow had result request_status %d",
            result.request_status);
    if(result.request_status != GG_REQUEST_SUCCESS) {
        // get shadow failed, reads error response
        err = loop_request_read(ggreq, read_buf, BUFFER_SIZE, &amount_read);
        if(err) {
            gg_log(GG_LOG_ERROR,
                    "Failed to read shadow data. amount_read(%zu), READ_BUFFER_SIZE(%zu), err(%d)",
                    amount_read, BUFFER_SIZE, err);
            goto cleanup;
        }

        gg_log(GG_LOG_ERROR, "get_thing_shadow failed. error message: %.*s",
                (int)amount_read, read_buf);
    } else {
        // get shadow succeeded, reads success response
        err = loop_request_read(ggreq, read_buf, BUFFER_SIZE, &amount_read);
        if(err) {
            gg_log(GG_LOG_ERROR,
                    "Failed to read shadow data. amount_read(%zu), READ_BUFFER_SIZE(%zu), err(%d)",
                    amount_read, BUFFER_SIZE, err);
            goto cleanup;
        }

        gg_log(GG_LOG_INFO, "get_thing_shadow succeeded. response: %.*s",
                (int)amount_read, read_buf);
    }
cleanup:
    gg_request_close(ggreq);

done:
    return err;
}


void handler(const gg_lambda_context *cxt) {
    /* cxt is not used. */
    (void) cxt;

    update_thing_shadow();
    get_thing_shadow();
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
