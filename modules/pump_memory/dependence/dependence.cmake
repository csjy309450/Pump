#########################################################
# @file:    dependence.cmake
# @author:  yangzheng
# @brief:   modular pump_memory dependence configure.
#########################################################
message("<pump::pump_memory> /modules/pump_memory/dependence/dependence.cmake in")

# check pump_core
if(${pump_core_FOUND})
    message("<pump_memory> pump_core found")
else()
    message(FATAL_ERROR "<pump_memory> pump_core not found!")
endif()

#</find Boost>
# find_package(Boost REQUIRED COMPONENTS
#         thread
#         system
#         program_options)
#</find Boost>

# set target 3rdparty header
set(pump_memory_INNER_DEP_INC_DIRS
        ${pump_core_INCLUDE_DIRS}
        ${pump_macro_INCLUDE_DIRS}
        ${pump_function_INCLUDE_DIRS}
        ${pump_memory_INNER_PROJECT_ROOT_DIR}/dependence/include
        ${pump_memory_INNER_PROJECT_ROOT_DIR}/include
        ${pump_memory_INNER_PROJECT_ROOT_DIR}/src
        )
message("<pump::pump_memory> pump_memory_INNER_DEP_INC_DIRS> ${pump_memory_INNER_DEP_INC_DIRS}")

# set target 3rdparty libs
set(pump_memory_INNER_DEP_LIBS
        pump_core_dll
        pump_function_sll
        )
message("<pump::pump_memory> pump_memory_INNER_DEP_LIBS> ${pump_memory_INNER_DEP_LIBS}")

# set test srdparty header
set(pump_memory_INNER_TEST_DEP_INC_DIRS
        ${pump_memory_INNER_PROJECT_ROOT_DIR}/dependence/include
        ${pump_memory_INNER_PROJECT_ROOT_DIR}/include
        ${pump_memory_INNER_PROJECT_ROOT_DIR}/src
        # ${Boost_INCLUDE_DIRS}
        ${pump_test_INCLUDE_DIRS}
        )
message("<pump::pump_memory> pump_memory_INNER_TEST_DEP_INC_DIRS> ${pump_memory_INNER_TEST_DEP_INC_DIRS}")

# set test srdparty libs
set(pump_memory_INNER_TEST_DEP_LIBS
        # ${Boost_LIBRARIES}
        pump_test_sll
        )
message("<pump::pump_memory> pump_memory_INNER_TEST_DEP_LIBS> ${pump_memory_INNER_TEST_DEP_LIBS}")

message("<pump::pump_memory> /modules/pump_memory/dependence/dependence.cmake out")