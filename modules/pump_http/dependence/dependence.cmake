#########################################################
# @file:    dependence.cmake
# @author:  yangzheng
# @brief:   modular pump_http dependence configure.
#########################################################
message("<pump::pump_http> /modules/pump_http/dependence/dependence.cmake in")

# containing header dir to project
set(pump_http_INNER_DEP_INC_DIRS
        ${pump_http_INNER_PROJECT_ROOT_DIR}/dependence/include/${pump_OS_PLATFORM}
        ${pump_http_INNER_PROJECT_ROOT_DIR}/include
        ${pump_http_INNER_PROJECT_ROOT_DIR}/src
        ${pump_macro_INCLUDE_DIRS}
        ${pump_core_INCLUDE_DIRS}
        ${pump_memory_INCLUDE_DIRS}
        ${pump_function_INCLUDE_DIRS}
        ${pump_event_INCLUDE_DIRS}
        )
message("<pump::pump_http> pump_http_INNER_DEP_INC_DIRS> ${pump_http_INNER_DEP_INC_DIRS}")

if (${pump_OS_PLATFORM} MATCHES "win")
        set(pump_http_INNER_DEP_LIBS
                version.lib
        )
elseif(${pump_OS_PLATFORM} MATCHES "linux")
endif (${pump_OS_PLATFORM} MATCHES "win")

# containing 3rdpart libs to project
include(${pump_http_INNER_PROJECT_ROOT_DIR}/dependence/dlibs/CMakeLists.txt)
include(${pump_http_INNER_PROJECT_ROOT_DIR}/dependence/slibs/CMakeLists.txt)
set(pump_http_INNER_DEP_LIBS
        ${pump_http_INNER_DEP_LIBS}
        ${pump_http_INNER_DEP_SLIBS}
        ${pump_http_INNER_DEP_DLIBS}
        pump_core_dll
        pump_event_dll
        )
message("<pump::pump_http> pump_http_INNER_DEP_LIBS> ${pump_http_INNER_DEP_LIBS}")

# set test srdparty header
set(pump_http_INNER_TEST_DEP_INC_DIRS
        ${pump_http_INNER_PROJECT_ROOT_DIR}/dependence/include/${pump_OS_PLATFORM}
        ${pump_http_INNER_PROJECT_ROOT_DIR}/include
        ${pump_macro_INCLUDE_DIRS}
        ${pump_core_INCLUDE_DIRS}
        ${pump_event_INCLUDE_DIRS}
        ${pump_function_INCLUDE_DIRS}
        ${pump_memory_INCLUDE_DIRS}
        ${pump_test_INCLUDE_DIRS}
        )
message("<pump::pump_http> pump_http_INNER_TEST_DEP_INC_DIRS> ${pump_http_INNER_TEST_DEP_INC_DIRS}")

# set test srdparty libs
set(pump_http_INNER_TEST_DEP_LIBS
        ${pump_http_INNER_DEP_SLIBS}
        ${pump_http_INNER_DEP_DLIBS}
        pump_http_dll
        pump_test_sll
        )
message("<pump::pump_http> pump_http_INNER_TEST_DEP_LIBS> ${pump_http_INNER_TEST_DEP_LIBS}")


message("<pump::pump_http> /modules/pump_http/dependence/dependence.cmake out")