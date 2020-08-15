#########################################################
# @file:    dependence.cmake
# @author:  yangzheng
# @brief:   modular pump_test dependence configure.
#########################################################
message("<pump::pump_test> /modules/pump_test/dependence/dependence.cmake in")

# containing header dir to project
set(pump_test_INNER_DEP_INC_DIRS
        ${pump_test_INNER_PROJECT_ROOT_DIR}/dependence/include/${pump_OS_PLATFORM}
        ${pump_test_INNER_PROJECT_ROOT_DIR}/include
        ${pump_test_INNER_PROJECT_ROOT_DIR}/src
        ${Boost_INCLUDE_DIRS}
        )
message("<pump::pump_test> pump_test_INNER_DEP_INC_DIRS> ${pump_test_INNER_DEP_INC_DIRS}")

# containing 3rdpart libs to project
include(${pump_test_INNER_PROJECT_ROOT_DIR}/dependence/dlibs/CMakeLists.txt)
include(${pump_test_INNER_PROJECT_ROOT_DIR}/dependence/slibs/CMakeLists.txt)
set(pump_test_INNER_DEP_LIBS
        ${pump_test_INNER_DEP_SLIBS}
        ${pump_test_INNER_DEP_DLIBS}
        ${Boost_LIBRARIES}
        )
message("<pump::pump_test> pump_test_INNER_DEP_LIBS> ${pump_test_INNER_DEP_LIBS}")

message("<pump::pump_test> /modules/pump_test/dependence/dependence.cmake out")