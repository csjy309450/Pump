# set(pump_plugin_INNER_INCLUDE_DIR ${pump_plugin_INNER_INCLUDE_DIR} ${pump_plugin_INNER_PROJECT_ROOT_DIR}/src/)
# message("<pump_plugin-pump_plugin_INNER_INCLUDE_DIR> ${pump_plugin_INNER_INCLUDE_DIR}")
# include_directories(${pump_plugin_INNER_INCLUDE_DIR})

set(pump_plugin_INNER_EXSAMPLE_COMPILE_FLAG )
set(pump_plugin_INNER_EXSAMPLE_LINK_FLAG )

if(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
    set(pump_plugin_INNER_EXSAMPLE_COMPILE_FLAG ${pump_plugin_INNER_EXSAMPLE_COMPILE_FLAG}
            -Wl,-rpath=./)
    set(pump_plugin_INNER_EXSAMPLE_LINK_FLAG ${pump_plugin_INNER_EXSAMPLE_LINK_FLAG}
            -lrt)
elseif(${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
elseif(${CMAKE_GENERATOR} MATCHES "Visual Studio")
endif()

set(pump_plugin_INNER_EXSAMPLE_COMPILE_FLAG ${pump_plugin_INNER_EXSAMPLE_COMPILE_FLAG}
        -DPUMP_PLUGIN_USING_DLL)
set(CMAKE_CXX_STANDARD 98)

# add_executable(WinCmdClient ${pump_plugin_INNER_PROJECT_ROOT_DIR}/example/WinCmdClient.cpp)
# target_include_directories(WinCmdClient PRIVATE ${pump_plugin_INNER_TEST_DEP_INC_DIRS})
# target_link_libraries(WinCmdClient pump_plugin_dll ${pump_plugin_INNER_TEST_DEP_LIBS} ${pump_plugin_INNER_EXSAMPLE_LINK_FLAG})
# target_compile_options(WinCmdClient PRIVATE ${pump_plugin_INNER_EXSAMPLE_COMPILE_FLAG})
# set_target_properties(WinCmdClient PROPERTIES FOLDER "pump_plugin/example")