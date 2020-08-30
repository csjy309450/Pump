
message("<pump::pump_function> /modules/pump_function/test/CMakeLists.txt in")

set(pump_function_INNER_INCLUDE_DIR ${pump_function_INNER_INCLUDE_DIR} ${pump_function_INNER_PROJECT_ROOT_DIR}/src/)
message("<pump::pump_function> pump_function_INNER_INCLUDE_DIR> ${pump_function_INNER_INCLUDE_DIR}")
include_directories(${pump_function_INNER_INCLUDE_DIR})

set(pump_function_INNER_TEST_COMPILE_FLAG )
set(pump_function_INNER_TEST_LINK_FLAG )

if(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
    set(pump_function_INNER_TEST_COMPILE_FLAG ${pump_function_INNER_TEST_COMPILE_FLAG}
            -Wl,-rpath=./)
    set(pump_function_INNER_TEST_LINK_FLAG ${pump_function_INNER_TEST_LINK_FLAG}
            -lrt)
elseif(${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
elseif(${CMAKE_GENERATOR} MATCHES "Visual Studio")
endif()

set(pump_function_INNER_TEST_COMPILE_FLAG ${pump_function_INNER_TEST_COMPILE_FLAG}
        -DPUMP_FUNCTION_USING_DLL)

set(CMAKE_CXX_STANDARD 98)
add_executable(pump_function_test_fixture0
        ${pump_function_INNER_HEADERS}
${pump_function_INNER_PROJECT_ROOT_DIR}/test/pump_function_test_fixture0.cpp)
target_include_directories(pump_function_test_fixture0 PRIVATE ${pump_function_INNER_TEST_DEP_INC_DIRS})
target_link_libraries(pump_function_test_fixture0 ${pump_function_INNER_TEST_DEP_LIBS})
target_compile_definitions(pump_function_test_fixture0 PRIVATE ${pump_function_INNER_TEST_DEFINE_FLAG})
target_compile_options(pump_function_test_fixture0 PRIVATE ${pump_function_INNER_TEST_COMPILE_FLAG})
set_target_properties(pump_function_test_fixture0 PROPERTIES FOLDER "pump_function/test")

message("<pump::pump_function> /modules/pump_function/test/CMakeLists.txt out")