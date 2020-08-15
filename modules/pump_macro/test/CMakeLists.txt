message("<pump::pump_macro> /modules/pump_macro/test/CMakeLists.txt in")

set(pump_macro_INNER_INCLUDE_DIR ${pump_macro_INNER_INCLUDE_DIR} ${pump_macro_INNER_PROJECT_ROOT_DIR}/src/)
message("<pump::pump_macro> pump_macro_INNER_INCLUDE_DIR=${pump_macro_INNER_INCLUDE_DIR}")
# include_directories(${pump_macro_INNER_INCLUDE_DIR})

set(pump_macro_INNER_TEST_COMPILE_FLAG )
set(pump_macro_INNER_TEST_LINK_FLAG )

if(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
        set(pump_macro_INNER_TEST_COMPILE_FLAG ${pump_macro_INNER_TEST_COMPILE_FLAG}
                -Wl,-rpath=./)
        set(pump_macro_INNER_TEST_LINK_FLAG ${pump_macro_INNER_TEST_LINK_FLAG}
                -lrt)
elseif(${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
elseif(${CMAKE_GENERATOR} MATCHES "Visual Studio")
endif()

set(pump_macro_INNER_TEST_COMPILE_FLAG ${pump_macro_INNER_TEST_COMPILE_FLAG}
        -DPUMP_MACRO_USING_DLL)

message("<pump::pump_macro> /modules/pump_macro/test/CMakeLists.txt out")