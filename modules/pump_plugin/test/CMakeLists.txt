

# set(pump_plugin_INNER_INCLUDE_DIR ${pump_plugin_INNER_INCLUDE_DIR} ${pump_plugin_INNER_PROJECT_ROOT_DIR}/src/)
# message("<pump_plugin-pump_plugin_INNER_INCLUDE_DIR> ${pump_plugin_INNER_INCLUDE_DIR}")
# include_directories(${pump_plugin_INNER_INCLUDE_DIR})

set(pump_plugin_INNER_TEST_COMPILE_FLAG )
set(pump_plugin_INNER_TEST_LINK_FLAG )

if(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
    set(pump_plugin_INNER_TEST_COMPILE_FLAG ${pump_plugin_INNER_TEST_COMPILE_FLAG}
            -Wl,-rpath=./)
    set(pump_plugin_INNER_TEST_LINK_FLAG ${pump_plugin_INNER_TEST_LINK_FLAG}
            -lrt)
elseif(${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
elseif(${CMAKE_GENERATOR} MATCHES "Visual Studio")
endif()

set(pump_plugin_INNER_TEST_COMPILE_FLAG ${pump_plugin_INNER_TEST_COMPILE_FLAG}
        -DPUMP_PLUGIN_USING_DLL
        -DPUMP_CORE_USING_DLL
        -DPUMP_EVENT_USING_DLL
        -DPUMP_MEMORY_USING_SLL
        -DPUMP_FUNCTION_USING_SLL
        )
set(CMAKE_CXX_STANDARD 98)

add_executable(pump_plugin_ut0 ${pump_plugin_INNER_PROJECT_ROOT_DIR}/test/pump_plugin_ut0.cpp)
target_include_directories(pump_plugin_ut0 PRIVATE ${pump_plugin_INNER_TEST_DEP_INC_DIRS})
target_link_libraries(pump_plugin_ut0 pump_plugin_dll ${pump_plugin_INNER_TEST_DEP_LIBS} ${pump_plugin_INNER_TEST_LINK_FLAG})
target_compile_options(pump_plugin_ut0 PRIVATE ${pump_plugin_INNER_TEST_COMPILE_FLAG})
set_target_properties(pump_plugin_ut0 PROPERTIES FOLDER "pump_plugin/test")
