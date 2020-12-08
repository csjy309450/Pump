set(CMAKE_CXX_STANDARD 11)

set(pump_event_INNER_TEST_COMPILE_FLAG )
set(pump_event_INNER_TEST_LINK_FLAG )
set(pump_event_INNER_TEST_DEFINE_FLAG 
        -DPUMP_EVENT_USING_DLL
        -DPUMP_CORE_USING_DLL
        -DPUMP_MEMORY_USING_SLL
        )

if(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
    set(pump_event_INNER_TEST_COMPILE_FLAG ${pump_event_INNER_TEST_COMPILE_FLAG}
            -Wl,-rpath=./)
    set(pump_event_INNER_TEST_LINK_FLAG ${pump_event_INNER_TEST_LINK_FLAG}
            -lrt)
elseif(${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
elseif(${CMAKE_GENERATOR} MATCHES "Visual Studio")
endif()

set(pump_event_INNER_TEST_COMPILE_FLAG ${pump_event_INNER_TEST_COMPILE_FLAG} ${Db_Flage})
message("<pump_event>:pump_event_INNER_TEST_COMPILE_FLAG=${pump_event_INNER_TEST_COMPILE_FLAG}")

add_executable(pump_event_test_fixture_0
        ${pump_event_INNER_HEADERS}
        ${pump_event_INNER_PROJECT_ROOT_DIR}/test/pump_event_test_fixture_0.cpp)
target_include_directories(pump_event_test_fixture_0 PRIVATE ${pump_event_INNER_TEST_DEP_INC_DIRS})
target_link_libraries(pump_event_test_fixture_0 ${pump_event_INNER_TEST_DEP_LIBS})
target_compile_definitions(pump_event_test_fixture_0 PRIVATE ${pump_event_INNER_TEST_DEFINE_FLAG})
target_compile_options(pump_event_test_fixture_0 PRIVATE ${pump_event_INNER_TEST_COMPILE_FLAG})
set_target_properties(pump_event_test_fixture_0 PROPERTIES FOLDER "pump_event/test")

add_executable(pump_event_test_fixture_1
        ${pump_event_INNER_HEADERS}
        ${pump_event_INNER_PROJECT_ROOT_DIR}/test/pump_event_test_fixture_1.cpp)
target_include_directories(pump_event_test_fixture_1 PRIVATE ${pump_event_INNER_TEST_DEP_INC_DIRS})
target_link_libraries(pump_event_test_fixture_1 ${pump_event_INNER_TEST_DEP_LIBS})
target_compile_definitions(pump_event_test_fixture_1 PRIVATE ${pump_event_INNER_TEST_DEFINE_FLAG})
target_compile_options(pump_event_test_fixture_1 PRIVATE ${pump_event_INNER_TEST_COMPILE_FLAG})
set_target_properties(pump_event_test_fixture_1 PROPERTIES FOLDER "pump_event/test")