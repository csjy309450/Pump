#########################################################
# @file:    dependence.cmake
# @author:  yangzheng
# @brief:   modular pump_core dependence configure.
#########################################################
message("<pump::pump_core> /modules/pump_core/dependence/dependence.cmake in")

# #</find Boost>
# find_package(Boost REQUIRED COMPONENTS
#         thread
#         system
#         program_options)
# #</find Boost>

# TODO unzip boost.zip
# if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
#         # Scan all files in directory.
#         execute_process(COMMAND find ${in_dir} -type d
#                 RESULT_VARIABLE ret
#                 OUTPUT_VARIABLE dir_dirs)
# elseif(${CMAKE_SYSTEM_NAME}  MATCHES "Windows")
#         message("<pump::pump_core> expand ${pump_core_INNER_PROJECT_ROOT_DIR}/dependence/include/win/boost.zip")
#         execute_process(COMMAND expand ${pump_core_INNER_PROJECT_ROOT_DIR}/dependence/include/win/boost.zip
#                 RESULT_VARIABLE ret)
# endif(${CMAKE_SYSTEM_NAME}  MATCHES "Linux")

# containing header dir to project
set(pump_core_INNER_DEP_INC_DIRS
        ${pump_core_INNER_PROJECT_ROOT_DIR}/dependence/include/${pump_OS_PLATFORM}
        ${pump_core_INNER_PROJECT_ROOT_DIR}/include
        ${pump_core_INNER_PROJECT_ROOT_DIR}/src
        ${pump_macro_INCLUDE_DIRS}
        # ${Boost_INCLUDE_DIRS}
        )
message("<pump::pump_core> pump_core_INNER_DEP_INC_DIRS=${pump_core_INNER_DEP_INC_DIRS}")

if (${pump_OS_PLATFORM} MATCHES "win")
        set(pump_core_INNER_DEP_LIBS
                version.lib
        )
elseif(${pump_OS_PLATFORM} MATCHES "linux")
endif (${pump_OS_PLATFORM} MATCHES "win")

# containing 3rdpart libs to project
include(${pump_core_INNER_PROJECT_ROOT_DIR}/dependence/dlibs/CMakeLists.txt)
include(${pump_core_INNER_PROJECT_ROOT_DIR}/dependence/slibs/CMakeLists.txt)
set(pump_core_INNER_DEP_LIBS
        ${pump_core_INNER_DEP_LIBS}
        ${pump_core_INNER_DEP_SLIBS}
        ${pump_core_INNER_DEP_DLIBS}
        # ${Boost_LIBRARIES}
        )
message("<pump::pump_core> pump_core_INNER_DEP_LIBS=${pump_core_INNER_DEP_LIBS}")

# set test srdparty header
set(pump_core_INNER_TEST_DEP_INC_DIRS
        ${pump_core_INNER_PROJECT_ROOT_DIR}/dependence/include/${pump_OS_PLATFORM}
        ${pump_core_INNER_PROJECT_ROOT_DIR}/include
        ${pump_core_INNER_PROJECT_ROOT_DIR}/src
        ${Boost_INCLUDE_DIRS}
        ${pump_core_INCLUDE_DIRS}
        ${pump_macro_INCLUDE_DIRS}
        ${pump_test_INCLUDE_DIRS}
        )
message("<pump::pump_core> pump_core_INNER_TEST_DEP_INC_DIRS=${pump_core_INNER_TEST_DEP_INC_DIRS}")

# set test srdparty libs
set(pump_core_INNER_TEST_DEP_LIBS
        ${pump_core_INNER_DEP_SLIBS}
        ${pump_core_INNER_DEP_DLIBS}
        ${Boost_LIBRARIES}
        pump_core_dll
        pump_test_sll
        )
message("<pump::pump_core> pump_core_INNER_TEST_DEP_LIBS=${pump_core_INNER_TEST_DEP_LIBS}")


message("<pump::pump_core> /modules/pump_core/dependence/dependence.cmake out")