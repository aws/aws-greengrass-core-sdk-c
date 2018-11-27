/*
 * Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 */

#include <stdio.h>

#include "greengrasssdk.h"

/*
 * In case the system loads the stub library instead of the true
 * implementation library shipped with GGC by mistake, print an error.
 */
static void print_loaded_stub_error() {
    fprintf(stderr, "ERROR: Loaded stub instead of implementation library!\n");
}

/***************************************
**            Global Methods          **
***************************************/

gg_error gg_global_init(uint32_t opt) {
    (void)opt;
    print_loaded_stub_error();
    return GGE_RESERVED_MAX;
}

/***************************************
**           Logging Methods          **
***************************************/

gg_error gg_log(gg_log_level level, const char *format, ...) {
    (void)level;
    (void)format;
    print_loaded_stub_error();
    return GGE_RESERVED_MAX;
}

/***************************************
**         gg_request Methods         **
***************************************/

gg_error gg_request_init(gg_request *ggreq) {
    (void)ggreq;
    print_loaded_stub_error();
    return GGE_RESERVED_MAX;
}

gg_error gg_request_close(gg_request ggreq) {
    (void)ggreq;
    print_loaded_stub_error();
    return GGE_RESERVED_MAX;
}

gg_error gg_request_read(gg_request ggreq, void *buffer, size_t buffer_size,
                         size_t *amount_read) {
    (void)ggreq;
    (void)buffer;
    (void)buffer_size;
    (void)amount_read;
    print_loaded_stub_error();
    return GGE_RESERVED_MAX;
}

/***************************************
**           Runtime Methods          **
***************************************/

gg_error gg_runtime_start(gg_lambda_handler handler, uint32_t opt) {
    (void)handler;
    (void)opt;
    print_loaded_stub_error();
    return GGE_RESERVED_MAX;
}

gg_error gg_lambda_handler_read(void *buffer, size_t buffer_size,
                                size_t *amount_read) {
    (void)buffer;
    (void)buffer_size;
    (void)amount_read;
    print_loaded_stub_error();
    return GGE_RESERVED_MAX;
}

gg_error gg_lambda_handler_write_response(const void *response,
                                          size_t response_size) {
    (void)response;
    (void)response_size;
    print_loaded_stub_error();
    return GGE_RESERVED_MAX;
}

gg_error gg_lambda_handler_write_error(const char *error_message) {
    (void)error_message;
    print_loaded_stub_error();
    return GGE_RESERVED_MAX;
}

/***************************************
**     AWS Secrets Manager Methods    **
***************************************/

gg_error gg_get_secret_value(gg_request ggreq, const char *secret_id,
                             const char *version_id, const char *version_stage,
                             gg_request_result *result) {
    (void)ggreq;
    (void)secret_id;
    (void)version_id;
    (void)version_stage;
    (void)result;
    print_loaded_stub_error();
    return GGE_RESERVED_MAX;
}

/***************************************
**           Lambda Methods           **
***************************************/

gg_error gg_invoke(gg_request ggreq, const gg_invoke_options *opts,
                   gg_request_result *result) {
    (void)ggreq;
    (void)opts;
    (void)result;
    print_loaded_stub_error();
    return GGE_RESERVED_MAX;
}

/***************************************
**           AWS IoT Methods          **
***************************************/

gg_error gg_publish(gg_request ggreq, const char *topic, const void *payload,
                    size_t payload_size, gg_request_result *result) {
    (void)ggreq;
    (void)topic;
    (void)payload;
    (void)payload_size;
    (void)result;
    print_loaded_stub_error();
    return GGE_RESERVED_MAX;
}

gg_error gg_get_thing_shadow(gg_request ggreq, const char *thing_name,
                             gg_request_result *result) {
    (void)ggreq;
    (void)thing_name;
    (void)result;
    print_loaded_stub_error();
    return GGE_RESERVED_MAX;
}

gg_error gg_update_thing_shadow(gg_request ggreq, const char *thing_name,
                                const char *update_payload,
                                gg_request_result *result) {
    (void)ggreq;
    (void)thing_name;
    (void)update_payload;
    (void)result;
    print_loaded_stub_error();
    return GGE_RESERVED_MAX;
}

gg_error gg_delete_thing_shadow(gg_request ggreq, const char *thing_name,
                                gg_request_result *result) {
    (void)ggreq;
    (void)thing_name;
    (void)result;
    print_loaded_stub_error();
    return GGE_RESERVED_MAX;
}
