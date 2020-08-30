message("<pump::pump_memory> /modules/pump_memory/src/CMakeLists.txt in")

include(${pump_memory_INNER_PROJECT_ROOT_DIR}/src/pump_memory/CMakeLists.txt)

message("<pump::pump_memory> pump_memory_INNER_DEP_LIBS2=${pump_memory_INNER_DEP_LIBS}")

if(${pump_memory_BUILD_FLAG} MATCHES "static")
    set(CMAKE_CXX_STANDARD 11)
    set(pump_memory_INNER_SLIB_NAME pump_memory)
    if(${pump_COMPILER} MATCHES "vc")
        set(pump_memory_INNER_SLIB_NAME lib${pump_memory_INNER_SLIB_NAME})
        set(pump_memory_INNER_SLIB_NAME ${pump_memory_INNER_SLIB_NAME}-${pump_COMPILER}-${pump_CPUARCH_FLAGE}-sll)
    elseif(${pump_COMPILER} MATCHES "gcc")
        set(pump_memory_INNER_SLIB_NAME ${pump_memory_INNER_SLIB_NAME}-${pump_COMPILER})
    endif()
    if(${pump_BUILD_TYPE} STREQUAL "Debug")
        set(pump_memory_INNER_SLIB_NAME ${pump_memory_INNER_SLIB_NAME}-d)
    else()
        set(pump_memory_INNER_SLIB_NAME ${pump_memory_INNER_SLIB_NAME}-r)
    endif()
    string(REPLACE "." "_" pump_memory_INNER_VERSION_STR ${pump_memory_INNER_VERSION})
    set(pump_memory_INNER_SLIB_NAME ${pump_memory_INNER_SLIB_NAME}-${pump_memory_INNER_VERSION_STR})
    message("<pump::pump_memory> pump_memory_INNER_SLIB_NAME>:${pump_memory_INNER_SLIB_NAME}")
    add_library(pump_memory_sll
            STATIC
            ${pump_memory_INNER_HEADERS}
            ${pump_memory_INNER_SRCS}
            )
    target_include_directories(pump_memory_sll PRIVATE ${pump_memory_INNER_DEP_INC_DIRS})
    target_compile_definitions(pump_memory_sll PRIVATE PUMP_MEMORY_BUILD_SLL=1)
    target_compile_options(pump_memory_sll PRIVATE ${pump_memory_INNER_COMPILE_FLAG})
    set_target_properties(pump_memory_sll PROPERTIES OUTPUT_NAME ${pump_memory_INNER_SLIB_NAME})
    set_target_properties(pump_memory_sll PROPERTIES FOLDER "pump_memory/target")
    if(${pump_OS_PLATFORM} MATCHES "win")
        set(pump_memory_LIBRARIES "${pump_memory_INNER_SLIB_NAME}.lib" CACHE STRING INTERNAL FORCE)
    elseif(${pump_OS_PLATFORM} MATCHES "linux")
        set(pump_memory_LIBRARIES "${pump_memory_INNER_SLIB_NAME}.a" CACHE STRING INTERNAL FORCE)
    endif()
    message("<pump::pump_memory> pump_memory_LIBRARIES=${pump_memory_LIBRARIES}")
endif()

if(${pump_memory_BUILD_FLAG} MATCHES "shared")
    set(CMAKE_CXX_STANDARD 11)
    set(pump_memory_DLIB_NAME pump_memory)
    if(${pump_COMPILER} MATCHES "vc")
        set(pump_memory_DLIB_NAME lib${pump_memory_DLIB_NAME})
        set(pump_memory_DLIB_NAME ${pump_memory_DLIB_NAME}-${pump_COMPILER}-${pump_CPUARCH_FLAGE}-dll)
    elseif(${pump_COMPILER} MATCHES "gcc")
        set(pump_memory_DLIB_NAME ${pump_memory_DLIB_NAME}-${pump_COMPILER})
    endif()
    if(${pump_BUILD_TYPE} STREQUAL "Debug")
        set(pump_memory_DLIB_NAME ${pump_memory_DLIB_NAME}-d)
    else()
        set(pump_memory_DLIB_NAME ${pump_memory_DLIB_NAME}-r)
    endif()
    string(REPLACE "." "_" pump_memory_INNER_VERSION_STR ${pump_memory_INNER_VERSION})
    set(pump_memory_DLIB_NAME ${pump_memory_DLIB_NAME}-${pump_memory_INNER_VERSION_STR})
    message("<pump::pump_memory> pump_memory_DLIB_NAME>:${pump_memory_DLIB_NAME}")
    add_library(pump_memory_dll
            SHARED
            ${pump_memory_INNER_HEADERS}
            ${pump_memory_INNER_SRCS}
            )
    target_link_libraries(pump_memory_dll
            ${pump_memory_INNER_DEP_LIBS}
            ${pump_memory_INNER_LINK_FLAG}
            )
    target_include_directories(pump_memory_dll ${pump_memory_INNER_DEP_INC_DIRS})
    target_compile_definitions(pump_memory_dll PRIVATE pump_memory_BUILD_DLL=1)
    target_compile_options(pump_memory_dll PRIVATE ${pump_memory_INNER_COMPILE_FLAG})
    set_target_properties(pump_memory_dll PROPERTIES OUTPUT_NAME ${pump_memory_DLIB_NAME})
    set_target_properties(pump_memory_dll PROPERTIES FOLDER "pump_memory/target")
    if(${pump_OS_PLATFORM} MATCHES "win")
        set(pump_memory_LIBRARIES "${pump_memory_DLIB_NAME}.dll" CACHE STRING INTERNAL FORCE)
    elseif(${pump_OS_PLATFORM} MATCHES "linux")
        set(pump_memory_LIBRARIES "${pump_memory_DLIB_NAME}.so" CACHE STRING INTERNAL FORCE)
    endif()
    message("<pump::pump_memory> pump_memory_LIBRARIES=${pump_memory_LIBRARIES}")
endif()

message("<pump::pump_memory> /modules/pump_memory/src/CMakeLists.txt out")
