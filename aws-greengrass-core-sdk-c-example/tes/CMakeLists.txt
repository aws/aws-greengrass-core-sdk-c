cmake_minimum_required(VERSION 2.8)
project(tes_example)

find_package(aws-greengrass-core-sdk-c REQUIRED)

find_package(aws-cpp-sdk-core QUIET)
find_package(aws-cpp-sdk-dynamodb QUIET)

if(aws-cpp-sdk-core_FOUND AND aws-cpp-sdk-dynamodb_FOUND)
    add_executable(tes_example main.c tes.cpp)
    target_link_libraries(tes_example aws-greengrass-core-sdk-c aws-cpp-sdk-core aws-cpp-sdk-dynamodb)
    target_compile_features(tes_example PRIVATE cxx_std_11)
    target_compile_options(tes_example PRIVATE -Werror -Wall -Wextra -pedantic)
else()
    message(WARNING "TES example requires missing dependency: https://github.com/aws/aws-sdk-cpp")
endif()
