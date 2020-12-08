#########################################################
# @file:    dependence.cmake
# @author:  yangzheng
# @brief:   modular pump_event dependence configure.
#########################################################
message("<pump::pump_core> /modules/pump_event/dependence/dependence.cmake in")

#</find Boost>
# find_package(Boost REQUIRED COMPONENTS
#         thread
#         system
#         program_options)
#</find Boost>

# containing header dir to project
set(pump_event_INNER_DEP_INC_DIRS
        ${pump_event_INNER_PROJECT_ROOT_DIR}/dependence/include/${pump_OS_PLATFORM}
        ${pump_event_INNER_PROJECT_ROOT_DIR}/include
        ${pump_event_INNER_PROJECT_ROOT_DIR}/src
        # ${Boost_INCLUDE_DIRS}
        ${pump_macro_INCLUDE_DIRS}
        ${pump_core_INCLUDE_DIRS}
        ${pump_function_INCLUDE_DIRS}
        ${pump_memory_INCLUDE_DIRS}
        )
message("<pump::pump_event> pump_event_INNER_DEP_INC_DIRS> ${pump_event_INNER_DEP_INC_DIRS}")

# containing 3rdpart libs to project
include(${pump_event_INNER_PROJECT_ROOT_DIR}/dependence/dlibs/CMakeLists.txt)
include(${pump_event_INNER_PROJECT_ROOT_DIR}/dependence/slibs/CMakeLists.txt)
set(pump_event_INNER_DEP_LIBS
        ${pump_event_INNER_DEP_SLIBS}
        ${pump_event_INNER_DEP_DLIBS}
        # ${Boost_LIBRARIES}
        pump_core_dll
        pump_memory_sll
        pump_function_sll
        )
message("<pump::pump_event> pump_event_INNER_DEP_LIBS> ${pump_event_INNER_DEP_LIBS}")


# set test srdparty header
set(pump_event_INNER_TEST_DEP_INC_DIRS
        ${pump_event_INNER_DEP_INC_DIRS}
        ${pump_test_INCLUDE_DIRS}
        ${pump_event_INNER_PROJECT_ROOT_DIR}/dependence/include/${pump_OS_PLATFORM}
        ${pump_event_INNER_PROJECT_ROOT_DIR}/src
        )
message("<pump::pump_event> pump_event_INNER_TEST_DEP_INC_DIRS> ${pump_event_INNER_TEST_DEP_INC_DIRS}")

# set test srdparty libs
set(pump_event_INNER_TEST_DEP_LIBS
        ${pump_event_INNER_DEP_LIBS}
        ${pump_event_INNER_DEP_SLIBS}
        ${pump_event_INNER_DEP_DLIBS}
        pump_core_dll
        pump_event_dll
        pump_test_sll
        pump_memory_sll
        )
message("<pump::pump_event> pump_event_INNER_TEST_DEP_LIBS> ${pump_event_INNER_TEST_DEP_LIBS}")

message("<pump::pump_core> /modules/pump_event/dependence/dependence.cmake out")