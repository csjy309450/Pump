#####################################################
# user supper-configure
#####################################################
set(pump_CPUARCH_FLAGE x64) # x86 x64
set(pump_COMPILER gcc) # gcc g++ cl clang
set(pump_BUILD_TYPE Debug)

#####################################################
# cmake script begine
#####################################################
cmake_minimum_required(VERSION 3.2)
project(pump)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
set(pump_ROOT_DIR ${PROJECT_SOURCE_DIR})

message("<pump> CMakeLists.txt in")

message("<pump> CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}")

if(pump_BUILD_TYPE MATCHES "^((Debug)|(Release))$")
    set(CMAKE_BUILD_TYPE ${pump_BUILD_TYPE})
else()
    message(FATAL_ERROR "<pump> pump_BUILD_TYPE must be in {Debug, Release}")
endif()

# load file find function
include(${PROJECT_SOURCE_DIR}/cmake/find_files.cmake)

# check platform info and compiler info
include(${PROJECT_SOURCE_DIR}/cmake/compiler_conf.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/platforms/CMakeLists.txt)

# set libs/bin output dir
message("<pump> CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}")

# MSVC will add Debug/Release after output path automatically, so should have special handling under linux.
if(MSVC)
    set(pump_BINARY_DIR ${CMAKE_BINARY_DIR}/bin/${pump_OS_PLATFORM}_${pump_CPUARCH_FLAGE})
else()
    set(pump_BINARY_DIR ${CMAKE_BINARY_DIR}/bin/${pump_OS_PLATFORM}_${pump_CPUARCH_FLAGE}/${pump_BUILD_TYPE})
endif()
message("<pump> pump_BINARY_DIR=${pump_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${pump_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${pump_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${pump_BINARY_DIR})

# modules buiding configure
set(pump_macro_BUILD_FLAG "code")
message("<pump> pump_macro_BUILD_FLAG=${pump_macro_BUILD_FLAG}")
if(${pump_macro_BUILD_FLAG} MATCHES "(shared)|(static)|(code)")
    find_package(pump_macro)
endif()

set(pump_test_BUILD_FLAG "static")
message("<pump> pump_test_BUILD_FLAG=${pump_test_BUILD_FLAG}")
if(${pump_test_BUILD_FLAG} MATCHES "(shared)|(static)|(code)")
    find_package(pump_test)
endif()

set(pump_core_BUILD_FLAG "shared,static")
message("<pump> pump_core_BUILD_FLAG=${pump_core_BUILD_FLAG}")
if(${pump_core_BUILD_FLAG} MATCHES "(shared)|(static)|(code)")
    find_package(pump_core)
endif()

set(pump_function_BUILD_FLAG "static")
message("<pump> pump_function_BUILD_FLAG=${pump_function_BUILD_FLAG}")
if(${pump_function_BUILD_FLAG} MATCHES "(shared)|(static)|(code)")
    find_package(pump_function)
endif()

set(pump_memory_BUILD_FLAG "static")
message("<pump> pump_memory_BUILD_FLAG=${pump_memory_BUILD_FLAG}")
if(${pump_memory_BUILD_FLAG} MATCHES "(shared)|(static)|(code)")
    find_package(pump_memory)
endif()

# set(pump_mqtt_BUILD_FLAG "shared")
# message("<pump-> pump_mqtt_BUILD_FLAG=${pump_mqtt_BUILD_FLAG}")
# if(${pump_mqtt_BUILD_FLAG} MATCHES "(shared)|(static)|(code)")
#    find_package(pump_mqtt)
# endif()

# set(pump_ac_BUILD_FLAG "static")
# message("<pump> pump_ac_BUILD_FLAG=${pump_ac_BUILD_FLAG}")
# if(${pump_ac_BUILD_FLAG} MATCHES "(shared)|(static)|(code)")
#     find_package(pump_ac)
# endif()

#set(pump_cnum_BUILD_FLAG "static")
#message("<pump-pump_cnum_BUILD_FLAG>: " ${pump_cnum_BUILD_FLAG})
#if(${pump_cnum_BUILD_FLAG} MATCHES "(shared)|(static)|(code)")
#    find_package(pump_cnum)
#endif()

#set(pump_drone_BUILD_FLAG "shared")
#message(${pump_drone_BUILD_FLAG})
#if(${pump_drone_BUILD_FLAG} MATCHES "(shared)|(static)|(code)")
#    find_package(pump_drone )
#endif()
#
set(pump_event_BUILD_FLAG "shared")
message("<pump> ${pump_event_BUILD_FLAG}")
if(${pump_event_BUILD_FLAG} MATCHES "(shared)|(static)|(code)")
   find_package(pump_event)
endif()

#set(pump_plugin_BUILD_FLAG "shared,static")
#message(${pump_plugin_BUILD_FLAG})
#if(${pump_plugin_BUILD_FLAG} MATCHES "(shared)|(static)|(code)")
#   find_package(pump_plugin)
#endif()

include(${PROJECT_SOURCE_DIR}/example/CMakeLists.txt)
set("<pump> CMakeLists.txt out")