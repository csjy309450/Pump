message("<pump::pump_test> /modules/pump_test/src/CMakeLists.txt in")

# searching source code.
include(${pump_test_INNER_PROJECT_ROOT_DIR}/src/pump_test/CMakeLists.txt)

if(${pump_test_BUILD_FLAG} MATCHES "static")
    set(pump_test_INNER_SLIB_NAME pump_test)
    if(${pump_COMPILER} MATCHES "vc")
        set(pump_test_INNER_SLIB_NAME lib${pump_test_INNER_SLIB_NAME})
        set(pump_test_INNER_SLIB_NAME ${pump_test_INNER_SLIB_NAME}-${pump_COMPILER}-${pump_CPUARCH_FLAGE}-sll)
    elseif(${pump_COMPILER} MATCHES "gcc")
        set(pump_test_INNER_SLIB_NAME ${pump_test_INNER_SLIB_NAME}-${pump_COMPILER})
    endif()
    if(${pump_BUILD_TYPE} STREQUAL "Debug")
        set(pump_test_INNER_SLIB_NAME ${pump_test_INNER_SLIB_NAME}-d)
    else()
        set(pump_test_INNER_SLIB_NAME ${pump_test_INNER_SLIB_NAME}-r)
    endif()
    string(REPLACE "." "_" pump_test_INNER_VERSION_STR ${pump_test_INNER_VERSION})
    set(pump_test_INNER_SLIB_NAME ${pump_test_INNER_SLIB_NAME}-${pump_test_INNER_VERSION_STR})
    message("<pump::pump_test> pump_test_INNER_SLIB_NAME>:${pump_test_INNER_SLIB_NAME}")
    add_library(pump_test_sll
        STATIC
        ${pump_test_INNER_HEADERS}
        ${pump_test_INNER_SRCS}
        )
    target_include_directories(pump_test_sll PRIVATE ${pump_test_INNER_INCLUDE_DIR})
    target_compile_definitions(pump_test_sll PRIVATE PUMP_TEST_BUILD_SLL=1)
    target_compile_options(pump_test_sll PRIVATE ${pump_test_INNER_COMPILE_FLAG})
    set_target_properties(pump_test_sll PROPERTIES OUTPUT_NAME ${pump_test_INNER_SLIB_NAME})
    set_target_properties(pump_test_sll PROPERTIES FOLDER "pump_test/target")
    if(${pump_OS_PLATFORM} MATCHES "win")
        set(pump_test_LIBRARIES "${pump_test_INNER_SLIB_NAME}.lib" CACHE STRING INTERNAL FORCE)
    elseif(${pump_OS_PLATFORM} MATCHES "linux")
        set(pump_test_LIBRARIES "${pump_test_INNER_SLIB_NAME}.a" CACHE STRING INTERNAL FORCE)
    endif()
    message("<pump::pump_test> pump_test_LIBRARIES>=${pump_test_LIBRARIES}")
endif()

if(${pump_test_BUILD_FLAG} MATCHES "shared")
    set(pump_test_DLIB_NAME pump_test)
    if(${pump_COMPILER} MATCHES "vc")
        set(pump_test_DLIB_NAME lib${pump_test_DLIB_NAME})
        set(pump_test_DLIB_NAME ${pump_test_DLIB_NAME}-${pump_COMPILER}-${pump_CPUARCH_FLAGE}-dll)
    elseif(${pump_COMPILER} MATCHES "gcc")
        set(pump_test_DLIB_NAME ${pump_test_DLIB_NAME}-${pump_COMPILER})
    endif()
    if(${pump_BUILD_TYPE} STREQUAL "Debug")
        set(pump_test_DLIB_NAME ${pump_test_DLIB_NAME}-d)
    else()
        set(pump_test_DLIB_NAME ${pump_test_DLIB_NAME}-r)
    endif()
    string(REPLACE "." "_" pump_test_INNER_VERSION_STR ${pump_test_INNER_VERSION})
    set(pump_test_DLIB_NAME ${pump_test_DLIB_NAME}-${pump_test_INNER_VERSION_STR})
    message("<pump::pump_test> pump_test_DLIB_NAME>:${pump_test_DLIB_NAME}")
    add_library(pump_test_dll
        SHARED
        ${pump_test_INNER_HEADERS}
        ${pump_test_INNER_SRCS}
        )
    target_include_directories(pump_test_dll PRIVATE ${pump_test_INNER_INCLUDE_DIR})
    target_link_libraries(pump_test_dll
        ${pump_test_INNER_DEP_LIBS}
        ${pump_test_INNER_LINK_FLAG}
        )
    target_compile_definitions(pump_test_dll PRIVATE PUMP_TEST_BUILD_DLL=1)
    target_compile_options(pump_test_dll PRIVATE ${pump_test_INNER_COMPILE_FLAG})
    set_target_properties(pump_test_dll PROPERTIES OUTPUT_NAME ${pump_test_DLIB_NAME})
    set_target_properties(pump_test_dll PROPERTIES FOLDER "pump_test/target")
    if(${pump_OS_PLATFORM} MATCHES "win")
        set(pump_test_LIBRARIES "${pump_test_DLIB_NAME}.dll" CACHE STRING INTERNAL FORCE)
    elseif(${pump_OS_PLATFORM} MATCHES "linux")
        set(pump_test_LIBRARIES "${pump_test_DLIB_NAME}.so" CACHE STRING INTERNAL FORCE)
    endif()
    message("<pump::pump_test> pump_test_LIBRARIES>=${pump_test_LIBRARIES}")
endif()

message("<pump::pump_test> /modules/pump_test/src/CMakeLists.txt out")
