message("<pump::pump_core> /modules/pump_core/src/CMakeLists.txt in")

include(${pump_core_INNER_PROJECT_ROOT_DIR}/src/pump_core/CMakeLists.txt)

if(${pump_core_BUILD_FLAG} MATCHES "static")
    set(CMAKE_CXX_STANDARD 98)
    set(pump_core_INNER_SLIB_NAME pump_core)
    if(${pump_COMPILER} MATCHES "vc")
        set(pump_core_INNER_SLIB_NAME lib${pump_core_INNER_SLIB_NAME})
        set(pump_core_INNER_SLIB_NAME ${pump_core_INNER_SLIB_NAME}-${pump_COMPILER}-${pump_CPUARCH_FLAGE}-sll)
    elseif(${pump_COMPILER} MATCHES "gcc")
        set(pump_core_INNER_SLIB_NAME ${pump_core_INNER_SLIB_NAME}-${pump_COMPILER})
    endif()
    if(${pump_BUILD_TYPE} STREQUAL "Debug")
        set(pump_core_INNER_SLIB_NAME ${pump_core_INNER_SLIB_NAME}-d)
    else()
        set(pump_core_INNER_SLIB_NAME ${pump_core_INNER_SLIB_NAME}-r)
    endif()
    string(REPLACE "." "_" pump_core_INNER_VERSION_STR ${pump_core_INNER_VERSION})
    set(pump_core_INNER_SLIB_NAME ${pump_core_INNER_SLIB_NAME}-${pump_core_INNER_VERSION_STR})
    message("<pump::pump_core> pump_core_INNER_SLIB_NAME=${pump_core_INNER_SLIB_NAME}")
    add_library(pump_core_sll
        STATIC
        ${pump_core_INNER_HEADERS}
        ${pump_core_INNER_SRCS}
        )
    target_include_directories(pump_core_sll PRIVATE ${pump_core_INNER_INCLUDE_DIR})
    target_compile_definitions(pump_core_sll PRIVATE PUMP_CORE_BUILD_SLL=1)
    target_compile_options(pump_core_sll PRIVATE ${pump_core_INNER_COMPILE_FLAG})
    set_target_properties(pump_core_sll PROPERTIES OUTPUT_NAME ${pump_core_INNER_SLIB_NAME})
    set_target_properties(pump_core_sll PROPERTIES FOLDER "pump_core/target")
    if(${pump_OS_PLATFORM} MATCHES "win")
        set(pump_core_LIBRARIES "${pump_core_INNER_SLIB_NAME}.lib" CACHE STRING INTERNAL FORCE)
    elseif(${pump_OS_PLATFORM} MATCHES "linux")
        set(pump_core_LIBRARIES "${pump_core_INNER_SLIB_NAME}.a" CACHE STRING INTERNAL FORCE)
    endif()
    message("<pump::pump_core> pump_core_LIBRARIES=${pump_core_LIBRARIES}")
endif()

if(${pump_core_BUILD_FLAG} MATCHES "shared")
    set(CMAKE_CXX_STANDARD 98)
    set(pump_core_DLIB_NAME pump_core)
    if(${pump_COMPILER} MATCHES "vc")
        set(pump_core_DLIB_NAME lib${pump_core_DLIB_NAME})
        set(pump_core_DLIB_NAME ${pump_core_DLIB_NAME}-${pump_COMPILER}-${pump_CPUARCH_FLAGE}-dll)
    elseif(${pump_COMPILER} MATCHES "gcc")
        set(pump_core_DLIB_NAME ${pump_core_DLIB_NAME}-${pump_COMPILER})
    endif()
    if(${pump_BUILD_TYPE} STREQUAL "Debug")
        set(pump_core_DLIB_NAME ${pump_core_DLIB_NAME}-d)
    else()
        set(pump_core_DLIB_NAME ${pump_core_DLIB_NAME}-r)
    endif()
    string(REPLACE "." "_" pump_core_INNER_VERSION_STR ${pump_core_INNER_VERSION})
    set(pump_core_DLIB_NAME ${pump_core_DLIB_NAME}-${pump_core_INNER_VERSION_STR})
    message("<pump::pump_core> pump_core_DLIB_NAME=${pump_core_DLIB_NAME}")
    add_library(pump_core_dll
        SHARED
        ${pump_core_INNER_HEADERS}
        ${pump_core_INNER_SRCS}
        )
    target_link_libraries(pump_core_dll
        ${pump_core_INNER_DEP_LIBS}
        ${pump_core_INNER_LINK_FLAG}
        )
    target_include_directories(pump_core_dll PRIVATE ${pump_core_INNER_INCLUDE_DIR})
    target_compile_definitions(pump_core_dll PRIVATE PUMP_CORE_BUILD_DLL=1)
    target_compile_options(pump_core_dll PRIVATE ${pump_core_INNER_COMPILE_FLAG})
    set_target_properties(pump_core_dll PROPERTIES OUTPUT_NAME ${pump_core_DLIB_NAME})
    set_target_properties(pump_core_dll PROPERTIES FOLDER "pump_core/target")
    if(${pump_OS_PLATFORM} MATCHES "win")
        set(pump_core_LIBRARIES "${pump_core_DLIB_NAME}.dll" CACHE STRING INTERNAL FORCE)
    elseif(${pump_OS_PLATFORM} MATCHES "linux")
        set(pump_core_LIBRARIES "${pump_core_DLIB_NAME}.so" CACHE STRING INTERNAL FORCE)
    endif()
    message("<pump::pump_core> pump_core_LIBRARIES=${pump_core_LIBRARIES}")
endif()

message("<pump::pump_core> /modules/pump_core/src/CMakeLists.txt out")
