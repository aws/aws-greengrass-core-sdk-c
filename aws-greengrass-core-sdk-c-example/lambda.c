// Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.

#include <stdio.h>
#include <stdlib.h>
#include "greengrasssdk.h"

#define ERROR_BUFFER_SIZE 128
#define READ_BUFFER_SIZE 8192

char error_buf[ERROR_BUFFER_SIZE];
char read_buf[READ_BUFFER_SIZE];

struct customer_data {
    int key;
    int value;
};

struct response_data {
    int value;
};

gg_error loop_lambda_request_read(void *buffer, size_t buffer_size,
                                  size_t *total_read) {
    gg_error err = GGE_SUCCESS;
    uint8_t *read_index = (uint8_t*)buffer;
    size_t remaining_buf_size = buffer_size;
    size_t amount_read = 0;

    do {
        err = gg_lambda_request_read(read_index, remaining_buf_size,
            &amount_read);
        if(err) {
            gg_log(GG_LOG_ERROR, "gg_lambda_request_read had an error\n");
            goto cleanup;
        }
        *total_read += amount_read;
        read_index += amount_read;
        remaining_buf_size -= amount_read;
    } while(amount_read);

cleanup:
    return err;
}

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

void customer_example_lambda(const gg_lambda_context *cxt) {
    gg_error err = GGE_SUCCESS;
    int ret = 0;
    struct customer_data my_packet = {0};
    size_t amount_read = 0;
    struct response_data response = {0};

    gg_log(GG_LOG_INFO, "function arn: [%s]", cxt->function_arn);
    gg_log(GG_LOG_INFO, "client context: [%s]", cxt->client_context);

    err = loop_lambda_request_read(&my_packet, sizeof(my_packet), &amount_read);
    if(err || amount_read != sizeof(my_packet)) {
        // Write an error message instead of a normal response
        ret = snprintf(error_buf, ERROR_BUFFER_SIZE,
            "Failed to read data. amount_read(%zu), amount_requested(%zu), err(%d)",
            amount_read, sizeof(my_packet), err);
        if(ret < 0) {
            gg_log(GG_LOG_ERROR, "snprintf failed %d for err %d", ret, err);
            goto cleanup;
        }

        err = gg_lambda_request_write_error(error_buf);
        if(err) {
            gg_log(GG_LOG_ERROR, "gg_lambda_request_write_error failed %d",
                err);
        }

        goto cleanup;
    }

    // In this example, only positive values are considered valid input.
    // Negative numbers will cause an error message response.
    if(my_packet.value >= 0) {
        // Do something with the read data and write the response
        response.value = my_packet.value * 2;

        err = gg_lambda_request_write_response(&response,
            sizeof(response));
        if(err) {
            gg_log(GG_LOG_ERROR, "gg_lambda_request_write_response failed %d",
                err);
        }
    } else {
        // Write an error message
        const char error_message[] = "Read a negative value";

        err = gg_lambda_request_write_error(error_message);
        if(err) {
            gg_log(GG_LOG_ERROR, "gg_lambda_request_write_response failed %d",
                err);
        }
    }

cleanup:
    return;
}

gg_error example_invoke_a_lambda() {
    gg_error err = GGE_SUCCESS;
    gg_request ggreq = NULL;
    uint8_t payload_buffer[] = {1,2,3,4,5};
    size_t amount_read = 0;
    // example context: base64-encoded json "{ 'value': 'hello world' }"
    char customer_context[] = "eyAndmFsdWUnOiAnaGVsbG8gd29ybGQnIH0=";
    char qualifier[] = "1";
    struct response_data response = {0};

    gg_invoke_options opts = {
        .function_arn = "arn:aws:lambda:us-east-1:123456789012:function:CustomerExampleLambda:1",
        .customer_context = customer_context,
        .qualifier = qualifier,
        .type = GG_INVOKE_REQUEST_RESPONSE,
        .payload = payload_buffer,
        .payload_size = sizeof(payload_buffer)
    };

    err = gg_request_init(&ggreq);
    if(err) {
        gg_log(GG_LOG_ERROR, "Failed to initialize request");
        goto done;
    }

    err = gg_invoke(ggreq, &opts);
    switch(err) {
    case GGE_SUCCESS:
        // The invoked lambda successfully returned a response
        err = loop_request_read(ggreq, &response, sizeof(response),
            &amount_read);
        if(err || amount_read != sizeof(response)) {
            gg_log(GG_LOG_ERROR,
                "Failed to read data. amount_read(%zu), amount_requested(%zu), err(%d)",
                amount_read, sizeof(response), err);
            goto cleanup;
        }

        gg_log(GG_LOG_INFO, "Response.value %d", response.value);
        break;
    case GGE_SUCCESS_ERROR_MESSAGE_RESPONSE:
        // The invoked lambda had an application error
        // and returned an error message instead of a normal response
        err = loop_request_read(ggreq, error_buf, ERROR_BUFFER_SIZE,
            &amount_read);
        if(err) {
            gg_log(GG_LOG_ERROR,
                "Failed to read error message. amount_read(%zu), ERROR_BUFFER_SIZE(%zu), err(%d)",
                amount_read, ERROR_BUFFER_SIZE, err);
            goto cleanup;
        }

        gg_log(GG_LOG_INFO, "Error: %s", error_buf);
        break;
    default:
        gg_log(GG_LOG_ERROR, "something went wrong with invoke");
        goto cleanup;
    }

cleanup:
    gg_request_close(ggreq);

done:
    return err;
}

gg_error example_get_thing_shadow() {
    gg_error err = GGE_SUCCESS;
    gg_request ggreq = NULL;
    const char thing_name[] = "foo";
    size_t amount_read = 0;

    err = gg_request_init(&ggreq);
    if(err) {
        gg_log(GG_LOG_ERROR, "Failed to initialize request");
        goto done;
    }

    err = gg_get_thing_shadow(ggreq, thing_name);
    if(err) {
        gg_log(GG_LOG_ERROR, "get_thing_shadow failed with err %d", err);
        goto cleanup;
    }

    err = loop_request_read(ggreq, read_buf, READ_BUFFER_SIZE,
        &amount_read);
    if(err) {
        gg_log(GG_LOG_ERROR,
            "Failed to read shadow data. amount_read(%zu), READ_BUFFER_SIZE(%zu), err(%d)",
            amount_read, READ_BUFFER_SIZE, err);
        goto cleanup;
    }

    gg_log(GG_LOG_INFO, "get_thing_shadow read (size %zu): %.*s", amount_read,
        (int)amount_read, read_buf);

cleanup:
    gg_request_close(ggreq);

done:
    return err;
}

gg_error example_update_thing_shadow() {
    gg_error err = GGE_SUCCESS;
    gg_request ggreq = NULL;
    const char thing_name[] = "foo";
    const char payload[] =
    "{"
        "\"state\": {"
            "\"desired\": {"
                "\"mode\": \"ON\""
            "}"
        "}"
    "}";
    size_t amount_read = 0;

    err = gg_request_init(&ggreq);
    if(err) {
        gg_log(GG_LOG_ERROR, "Failed to initialize request");
        goto done;
    }

    err = gg_update_thing_shadow(ggreq, thing_name, payload, sizeof(payload));
    if(err) {
        gg_log(GG_LOG_ERROR, "gg_update_thing_shadow failed with err %d",
            err);
        goto cleanup;
    }

    err = loop_request_read(ggreq, read_buf, READ_BUFFER_SIZE,
        &amount_read);
    if(err) {
        gg_log(GG_LOG_ERROR,
            "Failed to read shadow data. amount_read(%zu), READ_BUFFER_SIZE(%zu), err(%d)",
            amount_read, READ_BUFFER_SIZE, err);
        goto cleanup;
    }

    gg_log(GG_LOG_INFO, "get_thing_shadow read (size %zu): %.*s", amount_read,
        (int)amount_read, read_buf);

cleanup:
    gg_request_close(ggreq);

done:
    return err;
}

gg_error example_delete_thing_shadow() {
    gg_error err = GGE_SUCCESS;
    gg_request ggreq = NULL;
    const char thing_name[] = "foo";

    err = gg_request_init(&ggreq);
    if(err) {
        gg_log(GG_LOG_ERROR, "Failed to initialize request");
        goto done;
    }

    err = gg_delete_thing_shadow(ggreq, thing_name);
    if(err) {
        gg_log(GG_LOG_ERROR, "get_thing_shadow failed with err %d", err);
        goto cleanup;
    }

cleanup:
    gg_request_close(ggreq);

done:
    return err;
}

int main() {
    gg_error err = GGE_SUCCESS;

    err = gg_global_init(0);
    if(err) {
        gg_log(GG_LOG_ERROR, "gg_global_init failed %d", err);
        goto cleanup;
    }

    err = example_invoke_a_lambda();
    if(err) {
        gg_log(GG_LOG_ERROR, "example_invoke_a_lambda failed %d", err);
        goto cleanup;
    }

    err = example_get_thing_shadow();
    if(err) {
        gg_log(GG_LOG_ERROR, "example_get_thing_shadow failed %d", err);
        goto cleanup;
    }

    err = example_update_thing_shadow();
    if(err) {
        gg_log(GG_LOG_ERROR, "example_update_thing_shadow failed %d", err);
        goto cleanup;
    }

    err = example_delete_thing_shadow();
    if(err) {
        gg_log(GG_LOG_ERROR, "example_delete_thing_shadow failed %d", err);
        goto cleanup;
    }

    gg_runtime_start(customer_example_lambda, 0);

cleanup:
    return 0;
}
