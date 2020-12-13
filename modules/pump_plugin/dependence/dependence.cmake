#########################################################
# @file:    dependence.cmake
# @author:  yangzheng
# @brief:   modular pump_plugin dependence configure.
#########################################################
message("<pump::pump_plugin> /modules/pump_plugin/dependence/dependence.cmake in")

# containing header dir to project
set(pump_plugin_INNER_DEP_INC_DIRS
        ${pump_plugin_INNER_PROJECT_ROOT_DIR}/dependence/include/${pump_OS_PLATFORM}
        ${pump_plugin_INNER_PROJECT_ROOT_DIR}/include
        ${pump_plugin_INNER_PROJECT_ROOT_DIR}/src
        ${pump_macro_INCLUDE_DIRS}
        ${pump_core_INCLUDE_DIRS}
        ${pump_memory_INCLUDE_DIRS}
        ${pump_function_INCLUDE_DIRS}
        ${pump_event_INCLUDE_DIRS}
        )
message("<pump::pump_plugin> pump_plugin_INNER_DEP_INC_DIRS> ${pump_plugin_INNER_DEP_INC_DIRS}")

if (${pump_OS_PLATFORM} MATCHES "win")
        set(pump_plugin_INNER_DEP_LIBS
                version.lib
        )
elseif(${pump_OS_PLATFORM} MATCHES "linux")
endif (${pump_OS_PLATFORM} MATCHES "win")

# containing 3rdpart libs to project
include(${pump_plugin_INNER_PROJECT_ROOT_DIR}/dependence/dlibs/CMakeLists.txt)
include(${pump_plugin_INNER_PROJECT_ROOT_DIR}/dependence/slibs/CMakeLists.txt)
set(pump_plugin_INNER_DEP_LIBS
        ${pump_plugin_INNER_DEP_LIBS}
        ${pump_plugin_INNER_DEP_SLIBS}
        ${pump_plugin_INNER_DEP_DLIBS}
        pump_core_dll
        )
message("<pump::pump_plugin> pump_plugin_INNER_DEP_LIBS> ${pump_plugin_INNER_DEP_LIBS}")

# set test srdparty header
set(pump_plugin_INNER_TEST_DEP_INC_DIRS
        ${pump_plugin_INNER_PROJECT_ROOT_DIR}/dependence/include/${pump_OS_PLATFORM}
        ${pump_plugin_INNER_PROJECT_ROOT_DIR}/include
        ${pump_plugin_INNER_PROJECT_ROOT_DIR}/src
        ${pump_macro_INCLUDE_DIRS}
        ${pump_core_INCLUDE_DIRS}
        ${pump_function_INCLUDE_DIRS}
        ${pump_memory_INCLUDE_DIRS}
        ${pump_test_INCLUDE_DIRS}
        )
message("<pump::pump_plugin> pump_plugin_INNER_TEST_DEP_INC_DIRS> ${pump_plugin_INNER_TEST_DEP_INC_DIRS}")

# set test srdparty libs
set(pump_plugin_INNER_TEST_DEP_LIBS
        ${pump_plugin_INNER_DEP_SLIBS}
        ${pump_plugin_INNER_DEP_DLIBS}
        pump_plugin_dll
        pump_test_sll
        )
message("<pump::pump_plugin> pump_plugin_INNER_TEST_DEP_LIBS> ${pump_plugin_INNER_TEST_DEP_LIBS}")


message("<pump::pump_plugin> /modules/pump_plugin/dependence/dependence.cmake out")