message("<pump::pump_core> /modules/pump_core/example/CMakeLists.txt in")

# set(pump_core_INNER_INCLUDE_DIR ${pump_core_INNER_INCLUDE_DIR} ${pump_core_INNER_PROJECT_ROOT_DIR}/src/)
# message("<pump_core-pump_core_INNER_INCLUDE_DIR> ${pump_core_INNER_INCLUDE_DIR}")
# include_directories(${pump_core_INNER_INCLUDE_DIR})

set(pump_core_INNER_EXSAMPLE_COMPILE_FLAG )
set(pump_core_INNER_EXSAMPLE_LINK_FLAG )

if(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
    set(pump_core_INNER_EXSAMPLE_COMPILE_FLAG ${pump_core_INNER_EXSAMPLE_COMPILE_FLAG}
            -Wl,-rpath=./)
    set(pump_core_INNER_EXSAMPLE_LINK_FLAG ${pump_core_INNER_EXSAMPLE_LINK_FLAG}
            -lrt)
elseif(${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
elseif(${CMAKE_GENERATOR} MATCHES "Visual Studio")
endif()

set(pump_core_INNER_EXSAMPLE_COMPILE_FLAG ${pump_core_INNER_EXSAMPLE_COMPILE_FLAG}
        -DPUMP_CORE_USING_DLL)

set(CMAKE_CXX_STANDARD 98)
add_executable(WinCmdClient ${pump_core_INNER_PROJECT_ROOT_DIR}/example/WinCmdClient.cpp)
target_include_directories(WinCmdClient PRIVATE ${pump_core_INNER_TEST_DEP_INC_DIRS})
target_link_libraries(WinCmdClient pump_core_dll ${pump_core_INNER_TEST_DEP_LIBS} ${pump_core_INNER_EXSAMPLE_LINK_FLAG})
target_compile_options(WinCmdClient PRIVATE ${pump_core_INNER_EXSAMPLE_COMPILE_FLAG})
set_target_properties(WinCmdClient PROPERTIES FOLDER "pump_core/example")

set(CMAKE_CXX_STANDARD 98)
add_executable(WinCmdServer ${pump_core_INNER_PROJECT_ROOT_DIR}/example/WinCmdServer.cpp)
target_include_directories(WinCmdServer PRIVATE ${pump_core_INNER_TEST_DEP_INC_DIRS})
target_link_libraries(WinCmdServer pump_core_dll ${pump_core_INNER_TEST_DEP_LIBS} ${pump_core_INNER_EXSAMPLE_LINK_FLAG})
target_compile_options(WinCmdServer PRIVATE ${pump_core_INNER_EXSAMPLE_COMPILE_FLAG})
set_target_properties(WinCmdServer PROPERTIES FOLDER "pump_core/example")

message("<pump::pump_core> /modules/pump_core/example/CMakeLists.txt out")