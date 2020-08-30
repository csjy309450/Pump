message("<pump::pump_macro> /modules/pump_macro/CMakeLists.txt in")

message("<pump::pump_macro> CMAKE_CURRRENT_BINARY_DIR>:${CMAKE_CURRRENT_BINARY_DIR}")
message("<pump::pump_macro> PROJECT_SOURCE_DIR>:${PROJECT_SOURCE_DIR}")
message("<pump::pump_macro> pump_macro_INNER_SOURCE_DIR>:${pump_macro_INNER_SOURCE_DIR}")
message("<pump::pump_macro> CMAKE_CURRENT_SOURCE_DIR>:${CMAKE_CURRENT_SOURCE_DIR}")
message("<pump::pump_macro> CMAKE_MODULE_PATH>:${CMAKE_MODULE_PATH}")
message("<pump::pump_macro> CMAKE_CURRENT_LIST_FILE>:${CMAKE_CURRENT_LIST_FILE}")
message("<pump::pump_macro> CMAKE_CURRENT_LIST_DIR>:${CMAKE_CURRENT_LIST_DIR}")

set(pump_macro_INNER_VERSION "0.9.0")

# config modular evn variale
if(NOT ${PROJECT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_LIST_DIR})
    # build module non-independently.
    message("<pump::pump_macro> pump_macro_INNER_IS_SUB_MODULAR=true")
    set(pump_macro_INNER_IS_SUB_MODULAR true)
    set(pump_macro_INNER_PROJECT_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
    message("<pump_macro_INNER_PROJECT_ROOT_DIR=${pump_macro_INNER_PROJECT_ROOT_DIR}")
    set(pump_macro_INNER_TOOLS_DIR ${PROJECT_SOURCE_DIR}/cmake)
    message("<pump::pump_macro> pump_macro_INNER_TOOLS_DIR=${pump_macro_INNER_TOOLS_DIR}")
else()
    # build module independently.
    message("<pump::pump_macro> pump_macro_INNER_IS_SUB_MODULAR=false")
    set(pump_macro_INNER_IS_SUB_MODULAR false)
    set(pump_macro_INNER_PROJECT_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
    message("<pump::pump_macro> pump_macro_INNER_PROJECT_ROOT_DIR=${pump_macro_INNER_PROJECT_ROOT_DIR}")
    set(pump_macro_INNER_TOOLS_DIR ${pump_macro_INNER_PROJECT_ROOT_DIR}/cmake)
    message("<pump::pump_macro> pump_macro_INNER_TOOLS_DIR=${pump_macro_INNER_TOOLS_DIR}")
endif()

# load 3rdparty dependence, will create pump_macro_INNER_DEP_INC_DIRS pump_macro_INNER_DEP_LIBS
include(${pump_macro_INNER_PROJECT_ROOT_DIR}/dependence/dependence.cmake)

# load compiler config
include(${pump_macro_INNER_PROJECT_ROOT_DIR}/cmake/compiler_conf.cmake)

# load modular build script
include(${pump_macro_INNER_PROJECT_ROOT_DIR}/src/CMakeLists.txt)

# load modular test script
include(${pump_macro_INNER_PROJECT_ROOT_DIR}/test/CMakeLists.txt)

message("<pump::pump_macro> /modules/pump_macro/CMakeLists.txt out")