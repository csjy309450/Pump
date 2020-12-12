message("<pump::pump_memory> /modules/pump_core/CMakeLists.txt in")
message("<pump::pump_memory> CMAKE_CURRRENT_BINARY_DIR>:${CMAKE_CURRRENT_BINARY_DIR}")
message("<pump::pump_memory> PROJECT_SOURCE_DIR>:${PROJECT_SOURCE_DIR}")
message("<pump::pump_memory> pump_memory_SOURCE_DIR>:${pump_memory_SOURCE_DIR}")
message("<pump::pump_memory> CMAKE_CURRENT_SOURCE_DIR>:${CMAKE_CURRENT_SOURCE_DIR}")
message("<pump::pump_memory> CMAKE_MODULE_PATH>:${CMAKE_MODULE_PATH}")
message("<pump::pump_memory> CMAKE_CURRENT_LIST_FILE>:${CMAKE_CURRENT_LIST_FILE}")
message("<pump::pump_memory> CMAKE_CURRENT_LIST_DIR>:${CMAKE_CURRENT_LIST_DIR}")

set(pump_memory_INNER_VERSION "0.9.0")

# config modular pump_memory variale
if(NOT ${PROJECT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_LIST_DIR})
    message("<pump::pump_memory> pump_memory_IS_SUB_MODULAR>: false")
    set(pump_memory_IS_SUB_MODULAR false)
    set(pump_memory_INNER_PROJECT_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
    message("<pump_memory_INNER_PROJECT_ROOT_DIR>: ${pump_memory_INNER_PROJECT_ROOT_DIR}")
    set(pump_memory_INNER_TOOLS_DIR ${PROJECT_SOURCE_DIR}/cmake)
    message("<pump::pump_memory> pump_memory_INNER_TOOLS_DIR>: ${pump_memory_INNER_TOOLS_DIR}")
else()
    message("<pump::pump_memory> pump_memory_IS_SUB_MODULAR>: true")
    set(pump_memory_INNER_PROJECT_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
    message("<pump::pump_memory> pump_memory_INNER_PROJECT_ROOT_DIR>: ${pump_memory_INNER_PROJECT_ROOT_DIR}")
    set(pump_memory_INNER_TOOLS_DIR ${pump_memory_INNER_PROJECT_ROOT_DIR}/cmake)
    message("<pump::pump_memory> pump_memory_INNER_TOOLS_DIR>: ${pump_memory_INNER_TOOLS_DIR}")
endif()

# load tools
include(${pump_memory_INNER_TOOLS_DIR}/find_files.cmake)

# load 3rdparty dependence, will create pump_memory_INNER_DEP_INC_DIRS pump_memory_INNER_DEP_LIBS
include(${pump_memory_INNER_PROJECT_ROOT_DIR}/dependence/dependence.cmake)

# load compiler config
include(${pump_memory_INNER_PROJECT_ROOT_DIR}/cmake/compiler_conf.cmake)

# load modular target script
include(${pump_memory_INNER_PROJECT_ROOT_DIR}/src/CMakeLists.txt)

# load modular test script
include(${pump_memory_INNER_PROJECT_ROOT_DIR}/test/CMakeLists.txt)

message("<pump::pump_memory> /modules/pump_core/CMakeLists.txt out")