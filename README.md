# AWS Greengrass Core C SDK - README.md

## Overview

The AWS Greengrass Core SDK for C provides an interface to interact with Greengrass Core system on the edge. It is c89 compliant and is meant to be performant while minimizing dependencies.

AWS Greengrass Core SDK for C is now in General Availability.

## Requirements
* libc 2.14+
* cmake 2.8+
* Greengrass Core 1.6+

## Building the SDK

Clone the SDK into local workspace:

```
git clone https://github.com/aws/aws-greengrass-core-sdk-c.git
```

Build the SDK:

```
cd aws-greengrass-core-sdk-c
mkdir build && cd build
cmake ..
cmake --build .
```

The build will produce a shared object named **libaws-greengrass-core-sdk-c.so** under build/aws-greengras-core-sdk-c directory. This is the shared object that the Lambda executable links to.

**Note:**
  - the shared object is a stub implementation that helps Lambda executable to link against. It will be overriden by the actual shared object that comes with Greengrass Core release bundle.
  - the `-Wl,--enable-new-dtags` flag is needed for adding Greengrass C SDK shared object path into Lambda executable's RUNPATH, so that the stub shared object could be overriden by the **libaws-greengrass-core-sdk-c.so** which comes along with Greengrass Core Release bundle. It is automatically included when linking to aws-greengrass-core-sdk-c.

## Building Greengrass Native Lambda with CMake
You can use CMake to build Greengrass Native Lambda. Other build tools could work as well.
Here cmake is shown as an example:

### Setting up a CMake Project
Create a directory to hold your project

```
mkdir my_gg_native_function
```

Open the directory and add a `CMakeLists.txt` file that specifies your project's name, executables, source files, and linked libraries. The following is a minimal example:

```
# minimal CMakeLists.txt for the AWS Greengrass SDK for C
cmake_minimum_required(VERSION 2.8)
# "my_gg_native_function" is just an example value.
project(my_gg_native_function)

# Locate the AWS Greengras SDK for C package.
# Requires that you build with:
#   -Daws-greengrass-core-sdk-c_DIR=/path/to/sdk_build
# or export/set:
#   CMAKE_PREFIX_PATH=/path/to/sdk_build
find_package(aws-greengrass-core-sdk-c REQUIRED)

add_executable(my_gg_native_funtion main.cpp)
target_link_libraries(my_gg_native_funtion aws-greengrass-core-sdk-c)
```

To get started quickly, there are several pre-made examples in aws-greengras-core-sdk-c-example directory for your reference. Those examples are built along with the SDK.

## Creating Deployment Package
After lambda is built, it should be packaged into deployment package, a zip file consisting of your executable and any dependencies. Then you could upload the lambda to AWS Lambda and deploy it to device using AWS Greengrass by following normal process.

## Using the SDK
### Initialization
All code using the AWS Greengrass SDK for C must do **gg_global_init()** before calling any other APIs and **gg_runtime_start()** to start the runtime as following:
```
#include "greengrasssdk.h"

void handler(const gg_lambda_context *cxt) {
    /* Your lambda logic goes here. */
}

int main() {
    gg_global_init(0);
    /* start the runtime in blocking mode. This blocks forever. */
    gg_runtime_start(handler, 0);
}
```

### Reading Event Payload
To read event payload for lambda handler to process, **gg_lambda_handler_read()** shall be used. The amount of data written into the buffer is not guaranteed to be complete until **amount_read** is zero.

The following is a sample usage of the method:

```
/* loop read handler event into buffer. */
gg_error loop_lambda_handler_read(void *buffer,
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
```
### Writing Handler Response
After lambda handler finishes processing, response could be returned using **gg_lambda_handler_write_response()**. In the case if this method is not called, empty response will be returned.

### Writing Handler Error Response
When lambda encounters any error, the error response could be returned to caller using **gg_lambda_handler_write_error()**.

### Initialize API Request
Every API request must be initialized using **gg_request_init()** before making the actual request.

### Reading API Response
There are several cases which require reading of response after an API call, such as **gg_invoke()**, **gg_get_thing_shadow()**, etc. **gg_request_read()** shall be used after the method call to retrieve output response.

The following is a sample usage of the method:

```
/* loop read request bytes into buffer. */
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
```

### Error Handling
When there is an error on method call, all the APIs have **gg_error** returned as return code. And for **gg_publish()**, **gg_invoke()** and **gg_xxx_thing_shadow()** APIs, you can check server side error from request status from **gg_request_result()** struct.

## API Documentation
Please see [API Documentation](https://aws-greengrass-core-sdk-c-docs.s3-website-us-east-1.amazonaws.com/v1.0.0/index.html) for more details.

## Opening Issues
If you encounter a bug with the AWS Greengrass SDK for C, we would like to hear about it. Search the [existing issues](https://github.com/aws/aws-greengrass-core-sdk-c/issues) and see if others are also experiencing the issue before opening a new issue. When creating issue, please fill in the following template:
```
[Problem Description]:  Describe the problem in detail
[Reproduction Steps]: Describe the steps in detail
[Target OS/Architecture]: eg. x86_64 Ubuntu 16.04
[Greengrass Core Version]: eg. 1.6.0
[Greengrass Core SDK for C Version]: eg. 1.0.0
[Compiler and Version] : eg. GCC 5.0 / LLVM 7
[Cmake Version] eg. Cmake 3.2
[Logs] runtime.log, lambda log
```
