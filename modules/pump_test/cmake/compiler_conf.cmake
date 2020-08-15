message("<pump::pump_test> /modules/pump_test/cmake/compiler_conf.cmake in")
set(pump_test_INNER_COMPILE_FLAG )
set(pump_test_INNER_LINK_FLAG )

# pump_test code debug level: DEBUG(0) INFO(1) WARN(2) ERROR(3) CRITICAL(4) RELEASE(5)
set(debug_pump_test 0)
if(${debug_pump_test} LESS 1)
    set(Db_Flage -D_TEST_LEVEL_DEBUG -D_TEST_LEVEL_INFO -D_TEST_LEVEL_WARN -D_TEST_LEVEL_ERROR -D_TEST_LEVEL_CRITICAL)
elseif(${debug_pump_test} LESS 2)
    set(Db_Flage -D_TEST_LEVEL_INFO -D_TEST_LEVEL_WARN -D_TEST_LEVEL_ERROR -D_TEST_LEVEL_CRITICAL)
elseif(${debug_pump_test} LESS 3)
    set(Db_Flage -D_TEST_LEVEL_WARN -D_TEST_LEVEL_ERROR -D_TEST_LEVEL_CRITICAL)
elseif(${debug_pump_test} LESS 4)
    set(Db_Flage -D_TEST_LEVEL_ERROR -D_TEST_LEVEL_CRITICAL)
elseif(${debug_pump_test} LESS 5)
    set(Db_Flage -D_TEST_LEVEL_CRITICAL)
else(${debug_pump_test} LESS 6)
    set(Db_Flage )
endif()
message("<pump::pump_test> Debug: ${Db_Flage}")
set(pump_test_INNER_COMPILE_FLAG ${pump_test_INNER_COMPILE_FLAG} ${Db_Flage})
# add_compile_options(${Db_Flage} ${pump_test_INNER_COMPILE_FLAG})

if(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
    set(pump_test_INNER_COMPILE_FLAG ${pump_test_INNER_COMPILE_FLAG}
            -Wl,-rpath=. -Wno-write-strings -fPIC)
    if(${pump_test_BUILD_FLAG} MATCHES "shared")
        set(pump_test_INNER_COMPILE_FLAG ${pump_test_INNER_COMPILE_FLAG}
                -DPUMP_TEST_BUILD_DLL)
    endif()
    set(pump_test_INNER_LINK_FLAG ${pump_test_INNER_LINK_FLAG}
        -lpthread -lrt)
elseif(${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
    set(pump_test_INNER_COMPILE_FLAG ${pump_test_INNER_COMPILE_FLAG}
        )
elseif(${CMAKE_GENERATOR} STREQUAL "MSYS Makefiles")
elseif(${CMAKE_GENERATOR} STREQUAL "Borland Makefiles")
elseif(${CMAKE_GENERATOR} STREQUAL "NMake Makefiles JOM")
elseif(${CMAKE_GENERATOR} STREQUAL "NMake Makefiles")
elseif(${CMAKE_GENERATOR} MATCHES "Visual Studio")
    set_property(GLOBAL PROPERTY USE_FOLDERS On)
    if(${CMAKE_GENERATOR} STREQUAL "Visual Studio 14 2015")
        if(${pump_test_BUILD_FLAG} MATCHES "shared")
            set(pump_test_INNER_COMPILE_FLAG ${pump_test_INNER_COMPILE_FLAG}
                    -DPUMP_TEST_BUILD_DLL -DGLOG_NO_ABBREVIATED_SEVERITIES)
        endif()
    elseif(${CMAKE_GENERATOR} STREQUAL "Visual Studio 14 2015 Win64")
        if(${pump_test_BUILD_FLAG} MATCHES "shared")
            set(pump_test_INNER_COMPILE_FLAG ${pump_test_INNER_COMPILE_FLAG}
                    -DPUMP_TEST_BUILD_DLL -DGLOG_NO_ABBREVIATED_SEVERITIES)
        endif()
    elseif(${CMAKE_GENERATOR} STREQUAL "Visual Studio 12 2013")
        set(pump_COMPILER vc12)
        if(${pump_test_BUILD_FLAG} MATCHES "shared")
            set(pump_test_INNER_COMPILE_FLAG ${pump_test_INNER_COMPILE_FLAG}
                -DPUMP_TEST_BUILD_DLL -DGLOG_NO_ABBREVIATED_SEVERITIES)
        endif()
    elseif(${CMAKE_GENERATOR} STREQUAL "Visual Studio 10")
    elseif(${CMAKE_GENERATOR} STREQUAL "Visual Studio 10 Win64")
    elseif(${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008")
    elseif(${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008 Win64")
    elseif(${CMAKE_GENERATOR} STREQUAL "Visual Studio 8 2005")
    elseif(${CMAKE_GENERATOR} STREQUAL "Visual Studio 8 2005 Win64")
    elseif(${CMAKE_GENERATOR} STREQUAL "Visual Studio 6")
    endif()
elseif(${CMAKE_GENERATOR} STREQUAL "Watcom WMake")
elseif(${CMAKE_GENERATOR} STREQUAL "CodeBlocks - MinGW Makefiles")
elseif(${CMAKE_GENERATOR} STREQUAL "CodeBlocks - NMake Makefiles")
elseif(${CMAKE_GENERATOR} STREQUAL "CodeBlocks - Unix Makefiles")
elseif(${CMAKE_GENERATOR} STREQUAL "Eclipse CDT4 - MinGW Makefiles")
elseif(${CMAKE_GENERATOR} STREQUAL "Eclipse CDT4 - NMake Makefiles")
elseif(${CMAKE_GENERATOR} STREQUAL "Eclipse CDT4 - Unix Makefiles")
endif()

message("<pump::pump_test> /modules/pump_test/cmake/compiler_conf.cmake out")