
set(CMAKE_CXX_STANDARD 98)
# set(pump_http_INNER_INCLUDE_DIR ${pump_http_INNER_INCLUDE_DIR} ${pump_http_INNER_PROJECT_ROOT_DIR}/src/)
# message("<pump_http-pump_http_INNER_INCLUDE_DIR> ${pump_http_INNER_INCLUDE_DIR}")
# include_directories(${pump_http_INNER_INCLUDE_DIR})

set(pump_http_INNER_TEST_COMPILE_FLAG )
set(pump_http_INNER_TEST_LINK_FLAG )

if(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
    set(pump_http_INNER_TEST_COMPILE_FLAG ${pump_http_INNER_TEST_COMPILE_FLAG}
            -Wl,-rpath=./)
    set(pump_http_INNER_TEST_LINK_FLAG ${pump_http_INNER_TEST_LINK_FLAG}
            -lrt)
elseif(${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
elseif(${CMAKE_GENERATOR} MATCHES "Visual Studio")
endif()

set(pump_http_INNER_TEST_COMPILE_FLAG ${pump_http_INNER_TEST_COMPILE_FLAG}
        -DPUMP_HTTP_USING_DLL
        -DPUMP_CORE_USING_DLL
        -DPUMP_EVENT_USING_DLL
        -DPUMP_MEMORY_USING_SLL
        -DPUMP_FUNCTION_USING_SLL
        )

add_executable(pump_http_ut0 ${pump_http_INNER_PROJECT_ROOT_DIR}/test/pump_http_ut0.cpp)
target_include_directories(pump_http_ut0 PRIVATE ${pump_http_INNER_TEST_DEP_INC_DIRS})
target_link_libraries(pump_http_ut0 pump_http_dll ${pump_http_INNER_TEST_DEP_LIBS} ${pump_http_INNER_TEST_LINK_FLAG})
target_compile_options(pump_http_ut0 PRIVATE ${pump_http_INNER_TEST_COMPILE_FLAG})
set_target_properties(pump_http_ut0 PROPERTIES FOLDER "pump_http/test")
