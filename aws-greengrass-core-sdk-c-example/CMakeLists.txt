cmake_minimum_required(VERSION 2.8)
project(aws-greengrass-core-sdk-c-example)

find_package(aws-greengrass-core-sdk-c REQUIRED)

add_executable(simple_handler simple_handler.c)
target_link_libraries(simple_handler aws-greengrass-core-sdk-c)
target_compile_options(simple_handler PRIVATE -Werror -Wall -Wextra -pedantic -std=c99 -Wc++-compat)

add_executable(publish_example publish.c)
target_link_libraries(publish_example aws-greengrass-core-sdk-c)
target_compile_options(publish_example PRIVATE -Werror -Wall -Wextra -pedantic -std=c99 -Wc++-compat)

add_executable(invoker invoke/invoker.c)
target_link_libraries(invoker aws-greengrass-core-sdk-c)
target_compile_options(invoker PRIVATE -Werror -Wall -Wextra -pedantic -std=c99 -Wc++-compat)

add_executable(invokee invoke/invokee.c)
target_link_libraries(invokee aws-greengrass-core-sdk-c)
target_compile_options(invokee PRIVATE -Werror -Wall -Wextra -pedantic -std=c99 -Wc++-compat)

add_executable(shadow_example shadow.c)
target_link_libraries(shadow_example aws-greengrass-core-sdk-c)
target_compile_options(shadow_example PRIVATE -Werror -Wall -Wextra -pedantic -std=c99 -Wc++-compat)

add_executable(hello_world_example hello_world.c)
target_link_libraries(hello_world_example aws-greengrass-core-sdk-c)
target_compile_options(hello_world_example PRIVATE -Werror -Wall -Wextra -pedantic -std=c99 -Wc++-compat)

add_subdirectory(tes)

add_executable(secretsmanager_example secretsmanager.c)
target_link_libraries(secretsmanager_example aws-greengrass-core-sdk-c)
target_compile_options(secretsmanager_example PRIVATE -Werror -Wall -Wextra -pedantic -std=c99 -Wc++-compat)
