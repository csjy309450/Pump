message("<pump::pump_macro> /modules/pump_macro/src/CMakeLists.txt in")

# searching source code.
include(${pump_macro_INNER_PROJECT_ROOT_DIR}/src/pump_macro/CMakeLists.txt)

if(${pump_macro_BUILD_FLAG} MATCHES "static")
    set(pump_macro_INNER_SLIB_NAME pump_macro)
    if(${pump_COMPILER} MATCHES "vc")
        set(pump_macro_INNER_SLIB_NAME lib${pump_macro_INNER_SLIB_NAME})
        set(pump_macro_INNER_SLIB_NAME ${pump_macro_INNER_SLIB_NAME}-${pump_COMPILER}-${pump_CPUARCH_FLAGE}-sll)
    elseif(${pump_COMPILER} MATCHES "gcc")
        set(pump_macro_INNER_SLIB_NAME ${pump_macro_INNER_SLIB_NAME}-${pump_COMPILER})
    endif()
    if(${pump_BUILD_TYPE} STREQUAL "Debug")
        set(pump_macro_INNER_SLIB_NAME ${pump_macro_INNER_SLIB_NAME}-d)
    else()
        set(pump_macro_INNER_SLIB_NAME ${pump_macro_INNER_SLIB_NAME}-r)
    endif()
    string(REPLACE "." "_" pump_macro_INNER_VERSION_STR ${pump_macro_INNER_VERSION})
    set(pump_macro_INNER_SLIB_NAME ${pump_macro_INNER_SLIB_NAME}-${pump_macro_INNER_VERSION_STR})
    message("<pump::pump_macro> pump_macro_INNER_SLIB_NAME=${pump_macro_INNER_SLIB_NAME}")
    add_library(pump_macro_sll
        STATIC
        ${pump_macro_INNER_HEADERS}
        ${pump_macro_INNER_SRCS}
        )
    target_include_directories(pump_macro_sll PRIVATE ${pump_macro_INNER_INCLUDE_DIR})
    target_compile_definitions(pump_macro_sll PRIVATE PUMP_MEMORY_BUILD_SLL=1)
    target_compile_options(pump_macro_sll PRIVATE ${pump_macro_INNER_COMPILE_FLAG})
    set_target_properties(pump_macro_sll PROPERTIES OUTPUT_NAME ${pump_macro_INNER_SLIB_NAME})
    set_target_properties(pump_macro_sll PROPERTIES FOLDER "pump_macro/target")
    if(${pump_OS_PLATFORM} MATCHES "win")
        set(pump_macro_LIBRARIES "${pump_macro_INNER_SLIB_NAME}.lib" CACHE STRING INTERNAL FORCE)
    elseif(${pump_OS_PLATFORM} MATCHES "linux")
        set(pump_macro_LIBRARIES "${pump_macro_INNER_SLIB_NAME}.a" CACHE STRING INTERNAL FORCE)
    endif()
    message("<pump::pump_macro> pump_macro_LIBRARIES=${pump_macro_LIBRARIES}")
endif()

if(${pump_macro_BUILD_FLAG} MATCHES "shared")
    set(pump_macro_DLIB_NAME pump_macro)
    if(${pump_COMPILER} MATCHES "vc")
        set(pump_macro_DLIB_NAME lib${pump_macro_DLIB_NAME})
        set(pump_macro_DLIB_NAME ${pump_macro_DLIB_NAME}-${pump_COMPILER}-${pump_CPUARCH_FLAGE}-dll)
    elseif(${pump_COMPILER} MATCHES "gcc")
        set(pump_macro_DLIB_NAME ${pump_macro_DLIB_NAME}-${pump_COMPILER})
    endif()
    if(${pump_BUILD_TYPE} STREQUAL "Debug")
        set(pump_macro_DLIB_NAME ${pump_macro_DLIB_NAME}-d)
    else()
        set(pump_macro_DLIB_NAME ${pump_macro_DLIB_NAME}-r)
    endif()
    string(REPLACE "." "_" pump_macro_INNER_VERSION_STR ${pump_macro_INNER_VERSION})
    set(pump_macro_DLIB_NAME ${pump_macro_DLIB_NAME}-${pump_macro_INNER_VERSION_STR})
    message("<pump::pump_macro> pump_macro_DLIB_NAME=${pump_macro_DLIB_NAME}")
    add_library(pump_macro_dll
        SHARED
        ${pump_macro_INNER_HEADERS}
        ${pump_macro_INNER_SRCS}
        )
    target_include_directories(pump_macro_dll PRIVATE ${pump_macro_INNER_INCLUDE_DIR})
    target_link_libraries(pump_macro_dll
        ${pump_macro_INNER_DEP_LIBS}
        ${pump_macro_INNER_LINK_FLAG}
        )
    target_compile_definitions(pump_macro_dll PRIVATE pump_macro_BUILD_DLL=1)
    target_compile_options(pump_macro_dll PRIVATE ${pump_macro_INNER_COMPILE_FLAG})
    set_target_properties(pump_macro_dll PROPERTIES OUTPUT_NAME ${pump_macro_DLIB_NAME})
    set_target_properties(pump_macro_dll PROPERTIES FOLDER "pump_macro/target")
    if(${pump_OS_PLATFORM} MATCHES "win")
        set(pump_macro_LIBRARIES "${pump_macro_DLIB_NAME}.dll" CACHE STRING INTERNAL FORCE)
    elseif(${pump_OS_PLATFORM} MATCHES "linux")
        set(pump_macro_LIBRARIES "${pump_macro_DLIB_NAME}.so" CACHE STRING INTERNAL FORCE)
    endif()
    message("<pump::pump_macro> pump_macro_LIBRARIES=${pump_macro_LIBRARIES}")
endif()

message("<pump::pump_macro> /modules/pump_macro/src/CMakeLists.txt out")
