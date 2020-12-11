if(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
    set(pump_memory_INNER_INNER_TEST_COMPILE_FLAG ${pump_memory_INNER_INNER_TEST_COMPILE_FLAG}
            -DPUMP_CORE_USING_DLL
            -Wl,-rpath=./
            )
    set(pump_memory_INNER_INNER_TEST_LINK_FLAG ${pump_memory_INNER_INNER_TEST_LINK_FLAG}
            ${pump_memory_INNER_TEST_DEP_LIBS}
            -lrt
            )
elseif(${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
elseif(${CMAKE_GENERATOR} MATCHES "Visual Studio")
    set(pump_memory_INNER_INNER_TEST_COMPILE_FLAG ${pump_memory_INNER_INNER_TEST_COMPILE_FLAG}
            -DPUMP_CORE_USING_DLL
            )
    set(pump_memory_INNER_INNER_TEST_LINK_FLAG ${pump_memory_INNER_INNER_TEST_LINK_FLAG}
            ${pump_memory_INNER_TEST_DEP_LIBS}
            )
endif()

set(CMAKE_CXX_STANDARD 11)

add_executable(pump_memory_ut0
        ${pump_memory_INNER_HEADERS}
        ${pump_memory_INNER_PROJECT_ROOT_DIR}/test/pump_memory_ut0.cpp)
target_include_directories(pump_memory_ut0 PRIVATE 
        ${pump_memory_INNER_INCLUDE_DIR}
        ${pump_memory_INNER_TEST_DEP_INC_DIRS})
target_link_libraries(pump_memory_ut0
        pump_memory_sll
        ${pump_memory_INNER_DEP_LIBS}
        ${pump_memory_INNER_TEST_DEP_LIBS}
        )
target_compile_options(pump_memory_ut0 PRIVATE ${pump_memory_INNER_INNER_TEST_COMPILE_FLAG})
set_target_properties(pump_memory_ut0 PROPERTIES FOLDER "pump_memory/test")