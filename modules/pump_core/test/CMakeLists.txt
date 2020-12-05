set(CMAKE_CXX_STANDARD 98)

# set(pump_core_INNER_INCLUDE_DIR ${pump_core_INNER_INCLUDE_DIR} ${pump_core_INNER_PROJECT_ROOT_DIR}/src/)
# message("<pump_core-pump_core_INNER_INCLUDE_DIR> ${pump_core_INNER_INCLUDE_DIR}")
# include_directories(${pump_core_INNER_INCLUDE_DIR})

set(pump_core_INNER_TEST_COMPILE_FLAG )
set(pump_core_INNER_TEST_LINK_FLAG )

if(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
    set(pump_core_INNER_TEST_COMPILE_FLAG ${pump_core_INNER_TEST_COMPILE_FLAG}
            -Wl,-rpath=./)
    set(pump_core_INNER_TEST_LINK_FLAG ${pump_core_INNER_TEST_LINK_FLAG}
            -lrt)
elseif(${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
elseif(${CMAKE_GENERATOR} MATCHES "Visual Studio")
endif()

set(pump_core_INNER_TEST_COMPILE_FLAG ${pump_core_INNER_TEST_COMPILE_FLAG}
        -DPUMP_CORE_USING_DLL)

add_executable(pump_core_ut0 ${pump_core_INNER_PROJECT_ROOT_DIR}/test/pump_core_ut0.cpp)
target_include_directories(pump_core_ut0 PRIVATE ${pump_core_INNER_TEST_DEP_INC_DIRS})
target_link_libraries(pump_core_ut0 pump_core_dll ${pump_core_INNER_TEST_DEP_LIBS} ${pump_core_INNER_TEST_LINK_FLAG})
target_compile_options(pump_core_ut0 PRIVATE ${pump_core_INNER_TEST_COMPILE_FLAG})
set_target_properties(pump_core_ut0 PROPERTIES FOLDER "pump_core/test")

add_executable(pump_core_ut1 
    ${pump_core_INNER_PROJECT_ROOT_DIR}/test/pump_core_ut1.cpp
    ${pump_core_INNER_PROJECT_ROOT_DIR}/test/async_pipe_server_thread.h
    ${pump_core_INNER_PROJECT_ROOT_DIR}/test/async_pipe_server_thread.cpp
    ${pump_core_INNER_PROJECT_ROOT_DIR}/test/async_pipe_server_recv_thread.h
    ${pump_core_INNER_PROJECT_ROOT_DIR}/test/async_pipe_server_recv_thread.cpp
    )
target_include_directories(pump_core_ut1 PRIVATE ${pump_core_INNER_TEST_DEP_INC_DIRS})
target_link_libraries(pump_core_ut1 pump_core_dll ${pump_core_INNER_TEST_DEP_LIBS} ${pump_core_INNER_TEST_LINK_FLAG})
target_compile_options(pump_core_ut1 PRIVATE ${pump_core_INNER_TEST_COMPILE_FLAG})
set_target_properties(pump_core_ut1 PROPERTIES FOLDER "pump_core/test")

add_executable(pump_core_test_fixture_tcp_server ${pump_core_INNER_PROJECT_ROOT_DIR}/test/pump_core_test_fixture_tcp_server.cpp)
target_include_directories(pump_core_test_fixture_tcp_server PRIVATE ${pump_core_INNER_TEST_DEP_INC_DIRS})
target_link_libraries(pump_core_test_fixture_tcp_server pump_core_dll ${pump_core_INNER_TEST_DEP_LIBS} ${pump_core_INNER_TEST_LINK_FLAG})
target_compile_options(pump_core_test_fixture_tcp_server PRIVATE ${pump_core_INNER_TEST_COMPILE_FLAG})
set_target_properties(pump_core_test_fixture_tcp_server PROPERTIES FOLDER "pump_core/test")

add_executable(pump_core_test_fixture_tcp_client ${pump_core_INNER_PROJECT_ROOT_DIR}/test/pump_core_test_fixture_tcp_client.cpp)
target_include_directories(pump_core_test_fixture_tcp_client PRIVATE ${pump_core_INNER_TEST_DEP_INC_DIRS})
target_link_libraries(pump_core_test_fixture_tcp_client pump_core_dll ${pump_core_INNER_TEST_DEP_LIBS} ${pump_core_INNER_TEST_LINK_FLAG})
target_compile_options(pump_core_test_fixture_tcp_client PRIVATE ${pump_core_INNER_TEST_COMPILE_FLAG})
set_target_properties(pump_core_test_fixture_tcp_client PROPERTIES FOLDER "pump_core/test")

add_executable(pump_core_test_fixture_cmder_client ${pump_core_INNER_PROJECT_ROOT_DIR}/test/pump_core_test_fixture_cmder_client.cpp)
target_include_directories(pump_core_test_fixture_cmder_client PRIVATE ${pump_core_INNER_TEST_DEP_INC_DIRS})
target_link_libraries(pump_core_test_fixture_cmder_client pump_core_dll ${pump_core_INNER_TEST_DEP_LIBS} ${pump_core_INNER_TEST_LINK_FLAG})
target_compile_options(pump_core_test_fixture_cmder_client PRIVATE ${pump_core_INNER_TEST_COMPILE_FLAG})
set_target_properties(pump_core_test_fixture_cmder_client PROPERTIES FOLDER "pump_core/test")

add_executable(pump_core_test_fixture_cmder_server ${pump_core_INNER_PROJECT_ROOT_DIR}/test/pump_core_test_fixture_cmder_server.cpp)
target_include_directories(pump_core_test_fixture_cmder_server PRIVATE ${pump_core_INNER_TEST_DEP_INC_DIRS})
target_link_libraries(pump_core_test_fixture_cmder_server pump_core_dll ${pump_core_INNER_TEST_DEP_LIBS} ${pump_core_INNER_TEST_LINK_FLAG})
target_compile_options(pump_core_test_fixture_cmder_server PRIVATE ${pump_core_INNER_TEST_COMPILE_FLAG})
set_target_properties(pump_core_test_fixture_cmder_server PROPERTIES FOLDER "pump_core/test")

add_executable(pump_core_test_fixture_file ${pump_core_INNER_PROJECT_ROOT_DIR}/test/pump_core_test_fixture_file.cpp)
target_include_directories(pump_core_test_fixture_file PRIVATE ${pump_core_INNER_TEST_DEP_INC_DIRS})
target_link_libraries(pump_core_test_fixture_file pump_core_dll ${pump_core_INNER_TEST_DEP_LIBS} ${pump_core_INNER_TEST_LINK_FLAG})
target_compile_options(pump_core_test_fixture_file PRIVATE ${pump_core_INNER_TEST_COMPILE_FLAG})
set_target_properties(pump_core_test_fixture_file PROPERTIES FOLDER "pump_core/test")

# add_executable(pump_core_test_fixture_reg ${pump_core_INNER_PROJECT_ROOT_DIR}/test/pump_core_test_fixture_reg.cpp)
# target_include_directories(pump_core_test_fixture_reg PRIVATE ${pump_core_INNER_TEST_DEP_INC_DIRS})
# target_link_libraries(pump_core_test_fixture_reg pump_core_dll ${pump_core_INNER_TEST_DEP_LIBS} ${pump_core_INNER_TEST_LINK_FLAG})
# target_compile_options(pump_core_test_fixture_reg PRIVATE ${pump_core_INNER_TEST_COMPILE_FLAG})
# set_target_properties(pump_core_test_fixture_reg PROPERTIES FOLDER "pump_core/test")

add_executable(pump_core_test_fixture_dllso ${pump_core_INNER_PROJECT_ROOT_DIR}/test/pump_core_test_fixture_dllso.cpp)
target_include_directories(pump_core_test_fixture_dllso PRIVATE ${pump_core_INNER_TEST_DEP_INC_DIRS})
target_link_libraries(pump_core_test_fixture_dllso pump_core_dll ${pump_core_INNER_TEST_DEP_LIBS} ${pump_core_INNER_TEST_LINK_FLAG})
target_compile_options(pump_core_test_fixture_dllso PRIVATE ${pump_core_INNER_TEST_COMPILE_FLAG})
set_target_properties(pump_core_test_fixture_dllso PROPERTIES FOLDER "pump_core/test")

add_executable(pump_core_test_fixture_thxpool ${pump_core_INNER_PROJECT_ROOT_DIR}/test/pump_core_test_fixture_thxpool.cpp)
target_include_directories(pump_core_test_fixture_thxpool PRIVATE ${pump_core_INNER_TEST_DEP_INC_DIRS})
target_link_libraries(pump_core_test_fixture_thxpool pump_core_dll ${pump_core_INNER_TEST_DEP_LIBS} ${pump_core_INNER_TEST_LINK_FLAG})
target_compile_options(pump_core_test_fixture_thxpool PRIVATE ${pump_core_INNER_TEST_COMPILE_FLAG})
set_target_properties(pump_core_test_fixture_thxpool PROPERTIES FOLDER "pump_core/test")