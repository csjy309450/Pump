message("<pump::pump_http> /modules/pump_http/src/CMakeLists.txt in")

include(${pump_http_INNER_PROJECT_ROOT_DIR}/src/pump_http/CMakeLists.txt)

message("<pump::pump_http> pump_http_INNER_DEP_LIBS2>: ${pump_http_INNER_DEP_LIBS}")

if(${pump_http_BUILD_FLAG} MATCHES "static")
    set(CMAKE_CXX_STANDARD 98)
    set(pump_http_INNER_SLIB_NAME pump_http)
    if(${pump_COMPILER} MATCHES "vc")
        set(pump_http_INNER_SLIB_NAME lib${pump_http_INNER_SLIB_NAME})
        set(pump_http_INNER_SLIB_NAME ${pump_http_INNER_SLIB_NAME}-${pump_COMPILER}-${pump_CPUARCH_FLAGE}-sll)
    elseif(${pump_COMPILER} MATCHES "gcc")
        set(pump_http_INNER_SLIB_NAME ${pump_http_INNER_SLIB_NAME}-${pump_COMPILER})
    endif()
    if(${pump_BUILD_TYPE} STREQUAL "Debug")
        set(pump_http_INNER_SLIB_NAME ${pump_http_INNER_SLIB_NAME}-d)
    else()
        set(pump_http_INNER_SLIB_NAME ${pump_http_INNER_SLIB_NAME}-r)
    endif()
    string(REPLACE "." "_" pump_http_INNER_VERSION_STR ${pump_http_INNER_VERSION})
    set(pump_http_INNER_SLIB_NAME ${pump_http_INNER_SLIB_NAME}-${pump_http_INNER_VERSION_STR})
    message("<pump::pump_http> pump_http_INNER_SLIB_NAME>:${pump_http_INNER_SLIB_NAME}")
    add_library(pump_http_sll
        STATIC
        ${pump_http_INNER_HEADERS}
        ${pump_http_INNER_SRCS}
        )
    target_include_directories(pump_http_sll PRIVATE ${pump_http_INNER_INCLUDE_DIR})
    target_compile_definitions(pump_http_sll PRIVATE PUMP_HTTP_BUILD_SLL=1)
    target_compile_options(pump_http_sll PRIVATE ${pump_http_INNER_COMPILE_FLAG})
    set_target_properties(pump_http_sll PROPERTIES OUTPUT_NAME ${pump_http_INNER_SLIB_NAME})
    set_target_properties(pump_http_sll PROPERTIES FOLDER "pump_http/target")
    if(${pump_OS_PLATFORM} MATCHES "win")
        set(pump_http_LIBRARIES "${pump_http_INNER_SLIB_NAME}.lib" CACHE STRING INTERNAL FORCE)
    elseif(${pump_OS_PLATFORM} MATCHES "linux")
        set(pump_http_LIBRARIES "${pump_http_INNER_SLIB_NAME}.a" CACHE STRING INTERNAL FORCE)
    endif()
    message("<pump::pump_http> pump_http_LIBRARIES>: ${pump_http_LIBRARIES}")
endif()

if(${pump_http_BUILD_FLAG} MATCHES "shared")
    set(CMAKE_CXX_STANDARD 98)
    set(pump_http_DLIB_NAME pump_http)
    if(${pump_COMPILER} MATCHES "vc")
        set(pump_http_DLIB_NAME lib${pump_http_DLIB_NAME})
        set(pump_http_DLIB_NAME ${pump_http_DLIB_NAME}-${pump_COMPILER}-${pump_CPUARCH_FLAGE}-dll)
    elseif(${pump_COMPILER} MATCHES "gcc")
        set(pump_http_DLIB_NAME ${pump_http_DLIB_NAME}-${pump_COMPILER})
    endif()
    if(${pump_BUILD_TYPE} STREQUAL "Debug")
        set(pump_http_DLIB_NAME ${pump_http_DLIB_NAME}-d)
    else()
        set(pump_http_DLIB_NAME ${pump_http_DLIB_NAME}-r)
    endif()
    string(REPLACE "." "_" pump_http_INNER_VERSION_STR ${pump_http_INNER_VERSION})
    set(pump_http_DLIB_NAME ${pump_http_DLIB_NAME}-${pump_http_INNER_VERSION_STR})
    message("<pump::pump_http> pump_http_DLIB_NAME>:${pump_http_DLIB_NAME}")
    add_library(pump_http_dll
        SHARED
        ${pump_http_INNER_HEADERS}
        ${pump_http_INNER_SRCS}
        )
    target_link_libraries(pump_http_dll
        ${pump_http_INNER_DEP_LIBS}
        ${pump_http_INNER_LINK_FLAG}
        )
    target_include_directories(pump_http_dll PRIVATE ${pump_http_INNER_INCLUDE_DIR})
    target_compile_definitions(pump_http_dll PRIVATE PUMP_HTTP_BUILD_DLL=1)
    target_compile_options(pump_http_dll PRIVATE ${pump_http_INNER_COMPILE_FLAG})
    set_target_properties(pump_http_dll PROPERTIES OUTPUT_NAME ${pump_http_DLIB_NAME})
    set_target_properties(pump_http_dll PROPERTIES FOLDER "pump_http/target")
    if(${pump_OS_PLATFORM} MATCHES "win")
        set(pump_http_LIBRARIES "${pump_http_DLIB_NAME}.dll" CACHE STRING INTERNAL FORCE)
    elseif(${pump_OS_PLATFORM} MATCHES "linux")
        set(pump_http_LIBRARIES "${pump_http_DLIB_NAME}.so" CACHE STRING INTERNAL FORCE)
    endif()
    message("<pump::pump_http> pump_http_LIBRARIES>: ${pump_http_LIBRARIES}")
endif()

message("<pump::pump_http> /modules/pump_http/src/CMakeLists.txt out")

