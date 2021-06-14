set(CMAKE_CXX_STANDARD 98)

# set(pump_ac_INNER_INCLUDE_DIR ${pump_ac_INNER_INCLUDE_DIR} ${pump_ac_INNER_PROJECT_ROOT_DIR}/src/)
# message("<pump_ac-pump_ac_INNER_INCLUDE_DIR> ${pump_ac_INNER_INCLUDE_DIR}")
# include_directories(${pump_ac_INNER_INCLUDE_DIR})

set(pump_ac_INNER_EXSAMPLE_COMPILE_FLAG )
set(pump_ac_INNER_EXSAMPLE_LINK_FLAG )

if(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
    set(pump_ac_INNER_EXSAMPLE_COMPILE_FLAG ${pump_ac_INNER_EXSAMPLE_COMPILE_FLAG}
            -Wl,-rpath=./)
    set(pump_ac_INNER_EXSAMPLE_LINK_FLAG ${pump_ac_INNER_EXSAMPLE_LINK_FLAG}
            -lrt)
elseif(${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
elseif(${CMAKE_GENERATOR} MATCHES "Visual Studio")
endif()

set(pump_ac_INNER_EXSAMPLE_COMPILE_FLAG ${pump_ac_INNER_EXSAMPLE_COMPILE_FLAG}
        -Dpump_ac_USING_DLL)
# add_executable(WinCmdClient ${pump_ac_INNER_PROJECT_ROOT_DIR}/example/WinCmdClient.cpp)
# target_include_directories(WinCmdClient PRIVATE ${pump_ac_INNER_TEST_DEP_INC_DIRS})
# target_link_libraries(WinCmdClient pump_ac_dll ${pump_ac_INNER_TEST_DEP_LIBS} ${pump_ac_INNER_EXSAMPLE_LINK_FLAG})
# target_compile_options(WinCmdClient PRIVATE ${pump_ac_INNER_EXSAMPLE_COMPILE_FLAG})
# set_target_properties(WinCmdClient PROPERTIES FOLDER "pump_ac/example")