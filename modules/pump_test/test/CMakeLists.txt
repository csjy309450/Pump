message("<pump::pump_test> /modules/pump_test/test/CMakeLists.txt in")

set(pump_test_INNER_INCLUDE_DIR ${pump_test_INNER_INCLUDE_DIR} ${pump_test_INNER_PROJECT_ROOT_DIR}/src/)
message("<pump::pump_test> pump_test_INNER_INCLUDE_DIR> ${pump_test_INNER_INCLUDE_DIR}")
include_directories(${pump_test_INNER_INCLUDE_DIR})

set(pump_test_INNER_TEST_COMPILE_FLAG )
set(pump_test_INNER_TEST_LINK_FLAG )

if(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
    set(pump_test_INNER_TEST_COMPILE_FLAG ${pump_test_INNER_TEST_COMPILE_FLAG}
            -Wl,-rpath=./)
    set(pump_test_INNER_TEST_LINK_FLAG ${pump_test_INNER_TEST_LINK_FLAG}
            -lrt)
elseif(${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
elseif(${CMAKE_GENERATOR} MATCHES "Visual Studio")
endif()

set(pump_test_INNER_TEST_COMPILE_FLAG ${pump_test_INNER_TEST_COMPILE_FLAG}
        -Dpump_test_USING_DLL)

message("<pump::pump_test> /modules/pump_test/test/CMakeLists.txt out")