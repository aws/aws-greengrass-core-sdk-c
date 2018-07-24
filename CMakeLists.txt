cmake_minimum_required(VERSION 2.8)
project(aws-greengrass-core-sdk-c-and-example)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

add_subdirectory(aws-greengrass-core-sdk-c)
add_subdirectory(aws-greengrass-core-sdk-c-example)

