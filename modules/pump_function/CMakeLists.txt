message("<pump::pump_function> /modules/pump_function/CMakeLists.txt in")

message("<pump::pump_function> CMAKE_CURRRENT_BINARY_DIR>:${CMAKE_CURRRENT_BINARY_DIR}")
message("<pump::pump_function> PROJECT_SOURCE_DIR>:${PROJECT_SOURCE_DIR}")
message("<pump::pump_function> pump_function_INNER_SOURCE_DIR>:${pump_function_INNER_SOURCE_DIR}")
message("<pump::pump_function> CMAKE_CURRENT_SOURCE_DIR>:${CMAKE_CURRENT_SOURCE_DIR}")
message("<pump::pump_function> CMAKE_MODULE_PATH>:${CMAKE_MODULE_PATH}")
message("<pump::pump_function> CMAKE_CURRENT_LIST_FILE>:${CMAKE_CURRENT_LIST_FILE}")
message("<pump::pump_function> CMAKE_CURRENT_LIST_DIR>:${CMAKE_CURRENT_LIST_DIR}")

set(pump_function_INNER_VERSION "0.9.0")

# config modular evn variale
if(NOT ${PROJECT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_LIST_DIR})
    # 不是独立编译模块
    message("<pump::pump_function> pump_function_INNER_IS_SUB_MODULAR>: true")
    set(pump_function_INNER_IS_SUB_MODULAR true)
    set(pump_function_INNER_PROJECT_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
    message("<pump_function_INNER_PROJECT_ROOT_DIR>: ${pump_function_INNER_PROJECT_ROOT_DIR}")
    set(pump_function_INNER_TOOLS_DIR ${PROJECT_SOURCE_DIR}/cmake)
    message("<pump::pump_function> pump_function_INNER_TOOLS_DIR>: ${pump_function_INNER_TOOLS_DIR}")
else()
    # 独立编译模块
    message("<pump::pump_function> pump_function_INNER_IS_SUB_MODULAR>: false")
    set(pump_function_INNER_IS_SUB_MODULAR false)
    set(pump_function_INNER_PROJECT_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
    message("<pump::pump_function> pump_function_INNER_PROJECT_ROOT_DIR>: ${pump_function_INNER_PROJECT_ROOT_DIR}")
    set(pump_function_INNER_TOOLS_DIR ${pump_function_INNER_PROJECT_ROOT_DIR}/cmake)
    message("<pump::pump_function> pump_function_INNER_TOOLS_DIR>: ${pump_function_INNER_TOOLS_DIR}")
endif()

#load 3rdparty dependence, will create pump_function_INNER_DEP_INC_DIRS pump_function_INNER_DEP_LIBS
include(${pump_function_INNER_PROJECT_ROOT_DIR}/dependence/dependence.cmake)

# load compiler config
include(${pump_function_INNER_PROJECT_ROOT_DIR}/cmake/compiler_conf.cmake)

# load modular build script
include(${pump_function_INNER_PROJECT_ROOT_DIR}/src/CMakeLists.txt)

# load modular test script
include(${pump_function_INNER_PROJECT_ROOT_DIR}/test/CMakeLists.txt)

message("<pump::pump_function> /modules/pump_function/CMakeLists.txt out")