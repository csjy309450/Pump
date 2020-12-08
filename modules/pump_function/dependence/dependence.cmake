#########################################################
# @file:    dependence.cmake
# @author:  yangzheng
# @brief:   modular pump_function dependence configure.
#########################################################
message("<pump::pump_function> /modules/pump_function/dependence/dependence.cmake in")

# containing header dir to project
set(pump_function_INNER_DEP_INC_DIRS
        ${pump_function_INNER_PROJECT_ROOT_DIR}/dependence/include/${pump_OS_PLATFORM}
        ${pump_function_INNER_PROJECT_ROOT_DIR}/include
        ${pump_function_INNER_PROJECT_ROOT_DIR}/src
        )
message("<pump::pump_function> pump_function_INNER_DEP_INC_DIRS> ${pump_function_INNER_DEP_INC_DIRS}")

# containing 3rdpart libs to project
include(${pump_function_INNER_PROJECT_ROOT_DIR}/dependence/dlibs/CMakeLists.txt)
include(${pump_function_INNER_PROJECT_ROOT_DIR}/dependence/slibs/CMakeLists.txt)
set(pump_function_INNER_DEP_LIBS
        ${pump_function_INNER_DEP_SLIBS}
        ${pump_function_INNER_DEP_DLIBS}
        )
message("<pump::pump_function> pump_function_INNER_DEP_LIBS> ${pump_function_INNER_DEP_LIBS}")


# set test srdparty header
set(pump_function_INNER_TEST_DEP_INC_DIRS
        ${pump_function_INNER_PROJECT_ROOT_DIR}/dependence/include/${pump_OS_PLATFORM}
        ${pump_function_INNER_PROJECT_ROOT_DIR}/include
        ${pump_function_INNER_PROJECT_ROOT_DIR}/src
        ${pump_core_INCLUDE_DIRS}
        ${pump_macro_INCLUDE_DIRS}
        ${pump_test_INCLUDE_DIRS}
        )
message("<pump::pump_function> pump_function_INNER_TEST_DEP_INC_DIRS> ${pump_function_INNER_TEST_DEP_INC_DIRS}")

# set test srdparty libs
set(pump_function_INNER_TEST_DEP_LIBS
        ${pump_function_INNER_DEP_SLIBS}
        ${pump_function_INNER_DEP_DLIBS}
        pump_core_dll
        pump_function_sll
        pump_test_sll
        )
message("<pump::pump_function> pump_function_INNER_TEST_DEP_LIBS> ${pump_function_INNER_TEST_DEP_LIBS}")

message("<pump::pump_function> dependence.cmake> out")