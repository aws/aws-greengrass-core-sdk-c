cmake_minimum_required(VERSION 2.8.12)
project(aws-greengrass-core-sdk-c)

set(GG_SDK_LIBRARY ${PROJECT_NAME})

list(APPEND LIB_SRC "lib/greengrasssdk.c")
set(VERSIONS_SRC "${CMAKE_CURRENT_SOURCE_DIR}/version/greengrasssdk_versions.map")
add_library(${GG_SDK_LIBRARY} SHARED ${LIB_SRC})
set_target_properties(${GG_SDK_LIBRARY} PROPERTIES PUBLIC_HEADER "include/greengrasssdk.h")
set_target_properties(${GG_SDK_LIBRARY} PROPERTIES LINK_FLAGS "-Wl,--version-script=${VERSIONS_SRC}")
target_compile_options(${GG_SDK_LIBRARY} PRIVATE -Werror -Wall -Wextra -pedantic -std=c89 -Wc++-compat)
target_link_libraries(${GG_SDK_LIBRARY} PUBLIC "-Wl,--enable-new-dtags")

target_include_directories(${GG_SDK_LIBRARY} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

install(TARGETS ${GG_SDK_LIBRARY}
    EXPORT ${GG_SDK_LIBRARY}-targets
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include
    )

install(EXPORT ${GG_SDK_LIBRARY}-targets
    FILE ${GG_SDK_LIBRARY}-config.cmake
    DESTINATION lib/cmake/${GG_SDK_LIBRARY})
