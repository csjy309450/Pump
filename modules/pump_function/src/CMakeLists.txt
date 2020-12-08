message("<pump::pump_function> /modules/pump_function/src/CMakeLists.txt in")

include(${pump_function_INNER_PROJECT_ROOT_DIR}/src/pump_function/CMakeLists.txt)

if(${pump_function_BUILD_FLAG} MATCHES "static")
    set(pump_function_INNER_SLIB_NAME pump_function)
    if(${pump_COMPILER} MATCHES "vc")
        set(pump_function_INNER_SLIB_NAME lib${pump_function_INNER_SLIB_NAME})
        set(pump_function_INNER_SLIB_NAME ${pump_function_INNER_SLIB_NAME}-${pump_COMPILER}-${pump_CPUARCH_FLAGE}-sll)
    elseif(${pump_COMPILER} MATCHES "gcc")
        set(pump_function_INNER_SLIB_NAME ${pump_function_INNER_SLIB_NAME}-${pump_COMPILER})
    endif()
    if(${pump_BUILD_TYPE} STREQUAL "Debug")
        set(pump_function_INNER_SLIB_NAME ${pump_function_INNER_SLIB_NAME}-d)
    else()
        set(pump_function_INNER_SLIB_NAME ${pump_function_INNER_SLIB_NAME}-r)
    endif()
    string(REPLACE "." "_" pump_function_INNER_VERSION_STR ${pump_function_INNER_VERSION})
    set(pump_function_INNER_SLIB_NAME ${pump_function_INNER_SLIB_NAME}-${pump_function_INNER_VERSION_STR})
    message("<pump::pump_function> pump_function_INNER_SLIB_NAME>:${pump_function_INNER_SLIB_NAME}")
    add_library(pump_function_sll
        STATIC
        ${pump_function_INNER_HEADERS}
        ${pump_function_INNER_SRCS}
        )
    target_include_directories(pump_function_sll PRIVATE ${pump_function_INNER_INCLUDE_DIR})
    target_compile_definitions(pump_function_sll PRIVATE PUMP_FUNCTION_BUILD_SLL=1)
    target_compile_options(pump_function_sll PRIVATE ${pump_function_INNER_COMPILE_FLAG})
    set_target_properties(pump_function_sll PROPERTIES OUTPUT_NAME ${pump_function_INNER_SLIB_NAME})
    set_target_properties(pump_function_sll PROPERTIES FOLDER "pump_function/target")
    if(${pump_OS_PLATFORM} MATCHES "win")
        set(pump_function_LIBRARIES "${pump_function_INNER_SLIB_NAME}.lib" CACHE STRING INTERNAL FORCE)
    elseif(${pump_OS_PLATFORM} MATCHES "linux")
        set(pump_function_LIBRARIES "${pump_function_INNER_SLIB_NAME}.a" CACHE STRING INTERNAL FORCE)
    endif()
    message("<pump::pump_function> pump_function_LIBRARIES>: ${pump_function_LIBRARIES}")
endif()

if(${pump_function_BUILD_FLAG} MATCHES "shared")
    set(pump_function_DLIB_NAME pump_function)
    if(${pump_COMPILER} MATCHES "vc")
        set(pump_function_DLIB_NAME lib${pump_function_DLIB_NAME})
        set(pump_function_DLIB_NAME ${pump_function_DLIB_NAME}-${pump_COMPILER}-${pump_CPUARCH_FLAGE}-dll)
    elseif(${pump_COMPILER} MATCHES "gcc")
        set(pump_function_DLIB_NAME ${pump_function_DLIB_NAME}-${pump_COMPILER})
    endif()
    if(${pump_BUILD_TYPE} STREQUAL "Debug")
        set(pump_function_DLIB_NAME ${pump_function_DLIB_NAME}-d)
    else()
        set(pump_function_DLIB_NAME ${pump_function_DLIB_NAME}-r)
    endif()
    string(REPLACE "." "_" pump_function_INNER_VERSION_STR ${pump_function_INNER_VERSION})
    set(pump_function_DLIB_NAME ${pump_function_DLIB_NAME}-${pump_function_INNER_VERSION_STR})
    message("<pump::pump_function> pump_function_DLIB_NAME>:${pump_function_DLIB_NAME}")
    add_library(pump_function_dll
        SHARED
        ${pump_function_INNER_HEADERS}
        ${pump_function_INNER_SRCS}
        )
    target_include_directories(pump_function_dll PRIVATE ${pump_function_INNER_INCLUDE_DIR})
    target_link_libraries(pump_function_dll
        ${pump_function_INNER_DEP_LIBS}
        ${pump_function_INNER_LINK_FLAG}
        )
    target_compile_definitions(pump_function_dll PRIVATE PUMP_FUNCTION_BUILD_DLL=1)
    target_compile_options(pump_function_dll PRIVATE ${pump_function_INNER_COMPILE_FLAG})
    set_target_properties(pump_function_dll PROPERTIES OUTPUT_NAME ${pump_function_DLIB_NAME})
    set_target_properties(pump_function_dll PROPERTIES FOLDER "pump_function/target")
    if(${pump_OS_PLATFORM} MATCHES "win")
        set(pump_function_LIBRARIES "${pump_function_DLIB_NAME}.dll" CACHE STRING INTERNAL FORCE)
    elseif(${pump_OS_PLATFORM} MATCHES "linux")
        set(pump_function_LIBRARIES "${pump_function_DLIB_NAME}.so" CACHE STRING INTERNAL FORCE)
    endif()
    message("<pump::pump_function> pump_function_LIBRARIES>: ${pump_function_LIBRARIES}")
endif()

message("<pump::pump_function> /modules/pump_function/src/CMakeLists.txt out")
