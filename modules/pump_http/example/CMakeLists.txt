set(CMAKE_CXX_STANDARD 98)

# set(pump_http_INNER_INCLUDE_DIR ${pump_http_INNER_INCLUDE_DIR} ${pump_http_INNER_PROJECT_ROOT_DIR}/src/)
# message("<pump_http-pump_http_INNER_INCLUDE_DIR> ${pump_http_INNER_INCLUDE_DIR}")
# include_directories(${pump_http_INNER_INCLUDE_DIR})

set(pump_http_INNER_EXSAMPLE_COMPILE_FLAG )
set(pump_http_INNER_EXSAMPLE_LINK_FLAG )

if(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
    set(pump_http_INNER_EXSAMPLE_COMPILE_FLAG ${pump_http_INNER_EXSAMPLE_COMPILE_FLAG}
            -Wl,-rpath=./)
    set(pump_http_INNER_EXSAMPLE_LINK_FLAG ${pump_http_INNER_EXSAMPLE_LINK_FLAG}
            -lrt)
elseif(${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
elseif(${CMAKE_GENERATOR} MATCHES "Visual Studio")
endif()

set(pump_http_INNER_EXSAMPLE_COMPILE_FLAG ${pump_http_INNER_EXSAMPLE_COMPILE_FLAG}
        -DPUMP_HTTP_USING_DLL)
# add_executable(WinCmdClient ${pump_http_INNER_PROJECT_ROOT_DIR}/example/WinCmdClient.cpp)
# target_include_directories(WinCmdClient PRIVATE ${pump_http_INNER_TEST_DEP_INC_DIRS})
# target_link_libraries(WinCmdClient pump_http_dll ${pump_http_INNER_TEST_DEP_LIBS} ${pump_http_INNER_EXSAMPLE_LINK_FLAG})
# target_compile_options(WinCmdClient PRIVATE ${pump_http_INNER_EXSAMPLE_COMPILE_FLAG})
# set_target_properties(WinCmdClient PROPERTIES FOLDER "pump_http/example")