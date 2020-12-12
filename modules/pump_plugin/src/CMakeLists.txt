message("<pump::pump_plugin> /modules/pump_plugin/src/CMakeLists.txt in")

include(${pump_plugin_INNER_PROJECT_ROOT_DIR}/src/pump_plugin/CMakeLists.txt)

message("<pump::pump_plugin> pump_plugin_INNER_DEP_LIBS2>: ${pump_plugin_INNER_DEP_LIBS}")

if(${pump_plugin_BUILD_FLAG} MATCHES "static")
    set(CMAKE_CXX_STANDARD 98)
    set(pump_plugin_INNER_SLIB_NAME pump_plugin)
    if(${pump_COMPILER} MATCHES "vc")
        set(pump_plugin_INNER_SLIB_NAME lib${pump_plugin_INNER_SLIB_NAME})
        set(pump_plugin_INNER_SLIB_NAME ${pump_plugin_INNER_SLIB_NAME}-${pump_COMPILER}-${pump_CPUARCH_FLAGE}-sll)
    elseif(${pump_COMPILER} MATCHES "gcc")
        set(pump_plugin_INNER_SLIB_NAME ${pump_plugin_INNER_SLIB_NAME}-${pump_COMPILER})
    endif()
    if(${pump_BUILD_TYPE} STREQUAL "Debug")
        set(pump_plugin_INNER_SLIB_NAME ${pump_plugin_INNER_SLIB_NAME}-d)
    else()
        set(pump_plugin_INNER_SLIB_NAME ${pump_plugin_INNER_SLIB_NAME}-r)
    endif()
    string(REPLACE "." "_" pump_plugin_INNER_VERSION_STR ${pump_plugin_INNER_VERSION})
    set(pump_plugin_INNER_SLIB_NAME ${pump_plugin_INNER_SLIB_NAME}-${pump_plugin_INNER_VERSION_STR})
    message("<pump::pump_plugin> pump_plugin_INNER_SLIB_NAME>:${pump_plugin_INNER_SLIB_NAME}")
    add_library(pump_plugin_sll
        STATIC
        ${pump_plugin_INNER_HEADERS}
        ${pump_plugin_INNER_SRCS}
        )
    target_include_directories(pump_plugin_sll PRIVATE ${pump_plugin_INNER_INCLUDE_DIR})
    target_compile_definitions(pump_plugin_sll PRIVATE PUMP_PLUGIN_BUILD_SLL=1)
    target_compile_options(pump_plugin_sll PRIVATE ${pump_plugin_INNER_COMPILE_FLAG})
    set_target_properties(pump_plugin_sll PROPERTIES OUTPUT_NAME ${pump_plugin_INNER_SLIB_NAME})
    set_target_properties(pump_plugin_sll PROPERTIES FOLDER "pump_plugin/target")
    if(${pump_OS_PLATFORM} MATCHES "win")
        set(pump_plugin_LIBRARIES "${pump_plugin_INNER_SLIB_NAME}.lib" CACHE STRING INTERNAL FORCE)
    elseif(${pump_OS_PLATFORM} MATCHES "linux")
        set(pump_plugin_LIBRARIES "${pump_plugin_INNER_SLIB_NAME}.a" CACHE STRING INTERNAL FORCE)
    endif()
    message("<pump::pump_plugin> pump_plugin_LIBRARIES>: ${pump_plugin_LIBRARIES}")
endif()

if(${pump_plugin_BUILD_FLAG} MATCHES "shared")
    set(CMAKE_CXX_STANDARD 98)
    set(pump_plugin_DLIB_NAME pump_plugin)
    if(${pump_COMPILER} MATCHES "vc")
        set(pump_plugin_DLIB_NAME lib${pump_plugin_DLIB_NAME})
        set(pump_plugin_DLIB_NAME ${pump_plugin_DLIB_NAME}-${pump_COMPILER}-${pump_CPUARCH_FLAGE}-dll)
    elseif(${pump_COMPILER} MATCHES "gcc")
        set(pump_plugin_DLIB_NAME ${pump_plugin_DLIB_NAME}-${pump_COMPILER})
    endif()
    if(${pump_BUILD_TYPE} STREQUAL "Debug")
        set(pump_plugin_DLIB_NAME ${pump_plugin_DLIB_NAME}-d)
    else()
        set(pump_plugin_DLIB_NAME ${pump_plugin_DLIB_NAME}-r)
    endif()
    string(REPLACE "." "_" pump_plugin_INNER_VERSION_STR ${pump_plugin_INNER_VERSION})
    set(pump_plugin_DLIB_NAME ${pump_plugin_DLIB_NAME}-${pump_plugin_INNER_VERSION_STR})
    message("<pump::pump_plugin> pump_plugin_DLIB_NAME>:${pump_plugin_DLIB_NAME}")
    add_library(pump_plugin_dll
        SHARED
        ${pump_plugin_INNER_HEADERS}
        ${pump_plugin_INNER_SRCS}
        )
    target_link_libraries(pump_plugin_dll
        ${pump_plugin_INNER_DEP_LIBS}
        ${pump_plugin_INNER_LINK_FLAG}
        )
    target_include_directories(pump_plugin_dll PRIVATE ${pump_plugin_INNER_INCLUDE_DIR})
    target_compile_definitions(pump_plugin_dll PRIVATE 
        PUMP_PLUGIN_BUILD_DLL=1
        PUMP_PLUGIN_LUA_MODE=0)
    target_compile_options(pump_plugin_dll PRIVATE ${pump_plugin_INNER_COMPILE_FLAG})
    set_target_properties(pump_plugin_dll PROPERTIES OUTPUT_NAME ${pump_plugin_DLIB_NAME})
    set_target_properties(pump_plugin_dll PROPERTIES FOLDER "pump_plugin/target")
    if(${pump_OS_PLATFORM} MATCHES "win")
        set(pump_plugin_LIBRARIES "${pump_plugin_DLIB_NAME}.dll" CACHE STRING INTERNAL FORCE)
    elseif(${pump_OS_PLATFORM} MATCHES "linux")
        set(pump_plugin_LIBRARIES "${pump_plugin_DLIB_NAME}.so" CACHE STRING INTERNAL FORCE)
    endif()
    message("<pump::pump_plugin> pump_plugin_LIBRARIES>: ${pump_plugin_LIBRARIES}")
endif()

message("<pump::pump_plugin> /modules/pump_plugin/src/CMakeLists.txt out")
