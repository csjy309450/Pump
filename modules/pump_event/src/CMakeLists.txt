message("<pump::pump_event> /modules/pump_event/src/CMakeLists.txt in")

include(${pump_event_INNER_PROJECT_ROOT_DIR}/src/pump_event/CMakeLists.txt)

if(${pump_event_BUILD_FLAG} MATCHES "static")
    set(CMAKE_CXX_STANDARD 98)
    set(pump_event_INNER_SLIB_NAME pump_event)
    if(${pump_COMPILER} MATCHES "vc")
        set(pump_event_INNER_SLIB_NAME lib${pump_event_INNER_SLIB_NAME})
        set(pump_event_INNER_SLIB_NAME ${pump_event_INNER_SLIB_NAME}-${pump_COMPILER}-${pump_CPUARCH_FLAGE}-sll)
    elseif(${pump_COMPILER} MATCHES "gcc")
        set(pump_event_INNER_SLIB_NAME ${pump_event_INNER_SLIB_NAME}-${pump_COMPILER})
    endif()
    if(${pump_BUILD_TYPE} STREQUAL "Debug")
        set(pump_event_INNER_SLIB_NAME ${pump_event_INNER_SLIB_NAME}-d)
    else()
        set(pump_event_INNER_SLIB_NAME ${pump_event_INNER_SLIB_NAME}-r)
    endif()
    string(REPLACE "." "_" pump_event_INNER_VERSION_STR ${pump_event_INNER_VERSION})
    set(pump_event_INNER_SLIB_NAME ${pump_event_INNER_SLIB_NAME}-${pump_event_INNER_VERSION_STR})
    message("<pump::pump_event> pump_event_INNER_SLIB_NAME>:${pump_event_INNER_SLIB_NAME}")
    add_library(pump_event_sll
        STATIC
        ${pump_event_INNER_HEADERS}
        ${pump_event_INNER_SRCS}
        )
    target_include_directories(pump_event_sll PRIVATE ${pump_event_INNER_INCLUDE_DIR})
    target_compile_definitions(pump_event_sll PRIVATE PUMP_EVENT_BUILD_SLL=1)
    target_compile_options(pump_event_sll PRIVATE ${pump_event_INNER_COMPILE_FLAG})
    set_target_properties(pump_event_sll PROPERTIES OUTPUT_NAME ${pump_event_INNER_SLIB_NAME})
    set_target_properties(pump_event_sll PROPERTIES FOLDER "pump_event/target")
    if(${pump_OS_PLATFORM} MATCHES "win")
        set(pump_event_LIBRARIES "${pump_event_INNER_SLIB_NAME}.lib" CACHE STRING INTERNAL FORCE)
    elseif(${pump_OS_PLATFORM} MATCHES "linux")
        set(pump_event_LIBRARIES "${pump_event_INNER_SLIB_NAME}.a" CACHE STRING INTERNAL FORCE)
    endif()
    message("<pump::pump_event> pump_event_LIBRARIES>: ${pump_event_LIBRARIES}")
endif()

if(${pump_event_BUILD_FLAG} MATCHES "shared")
set(CMAKE_CXX_STANDARD 98)
    set(pump_event_DLIB_NAME pump_event)
    if(${pump_COMPILER} MATCHES "vc")
        set(pump_event_DLIB_NAME lib${pump_event_DLIB_NAME})
        set(pump_event_DLIB_NAME ${pump_event_DLIB_NAME}-${pump_COMPILER}-${pump_CPUARCH_FLAGE}-dll)
    elseif(${pump_COMPILER} MATCHES "gcc")
        set(pump_event_DLIB_NAME ${pump_event_DLIB_NAME}-${pump_COMPILER})
    endif()
    if(${pump_BUILD_TYPE} STREQUAL "Debug")
        set(pump_event_DLIB_NAME ${pump_event_DLIB_NAME}-d)
    else()
        set(pump_event_DLIB_NAME ${pump_event_DLIB_NAME}-r)
    endif()
    string(REPLACE "." "_" pump_event_INNER_VERSION_STR ${pump_event_INNER_VERSION})
    set(pump_event_DLIB_NAME ${pump_event_DLIB_NAME}-${pump_event_INNER_VERSION_STR})
    message("<pump::pump_event> pump_event_DLIB_NAME>:${pump_event_DLIB_NAME}")
    add_library(pump_event_dll
        SHARED
        ${pump_event_INNER_HEADERS}
        ${pump_event_INNER_SRCS}
        )
    target_include_directories(pump_event_dll PRIVATE ${pump_event_INNER_INCLUDE_DIR})
    target_link_libraries(pump_event_dll
        ${pump_event_INNER_DEP_LIBS}
        ${pump_event_INNER_LINK_FLAG}
        )
    target_compile_definitions(pump_event_dll PRIVATE PUMP_EVENT_BUILD_DLL=1)
    target_compile_options(pump_event_dll PRIVATE ${pump_event_INNER_COMPILE_FLAG})
    set_target_properties(pump_event_dll PROPERTIES OUTPUT_NAME ${pump_event_DLIB_NAME})
    set_target_properties(pump_event_dll PROPERTIES FOLDER "pump_event/target")
    if(${pump_OS_PLATFORM} MATCHES "win")
        set(pump_event_LIBRARIES "${pump_event_DLIB_NAME}.dll" CACHE STRING INTERNAL FORCE)
    elseif(${pump_OS_PLATFORM} MATCHES "linux")
        set(pump_event_LIBRARIES "${pump_event_DLIB_NAME}.so" CACHE STRING INTERNAL FORCE)
    endif()
    message("<pump::pump_event> pump_event_LIBRARIES>: ${pump_event_LIBRARIES}")
endif()

message("<pump::pump_event> /modules/pump_event/src/CMakeLists.txt out")
