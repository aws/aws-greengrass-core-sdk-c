
find_path(GGSDK_INCLUDE_DIR 
    NAMES greengrasssdk.h
    PATHS "${CMAKE_SOURCE_DIR}/aws-greengrass-core-sdk-c/include"
    )
message(STATUS "Greengrass SDK Include Directory: ${GGSDK_INCLUDE_DIR}")
