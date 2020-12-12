message("<pump::pump_plugin> /modules/pump_plugin/cmake/compiler_conf.cmake in")
set(pump_plugin_INNER_COMPILE_FLAG )
set(pump_plugin_INNER_LINK_FLAG )

# test code lpump_pluginel: DEBUG(0) INFO(1) WARN(2) ERROR(3) CRITICAL(4) RELEASE(5)
set(debug_pump_plugin 0) # 用户设置编译选项
if(${debug_pump_plugin} LESS 1)
    set(Db_Flage -D_TEST_LEVEL_DEBUG -D_TEST_LEVEL_INFO -D_TEST_LEVEL_WARN -D_TEST_LEVEL_ERROR -D_TEST_LEVEL_CRITICAL)
elseif(${debug_pump_plugin} LESS 2)
    set(Db_Flage -D_TEST_LEVEL_INFO -D_TEST_LEVEL_WARN -D_TEST_LEVEL_ERROR -D_TEST_LEVEL_CRITICAL)
elseif(${debug_pump_plugin} LESS 3)
    set(Db_Flage -D_TEST_LEVEL_WARN -D_TEST_LEVEL_ERROR -D_TEST_LEVEL_CRITICAL)
elseif(${debug_pump_plugin} LESS 4)
    set(Db_Flage -D_TEST_LEVEL_ERROR -D_TEST_LEVEL_CRITICAL)
elseif(${debug_pump_plugin} LESS 5)
    set(Db_Flage -D_TEST_LEVEL_CRITICAL)
else(${debug_pump_plugin} LESS 6)
    set(Db_Flage )
endif()#include <boost\function.hpp>
message(">>> Debug: ${Db_Flage}")
set(pump_plugin_INNER_COMPILE_FLAG ${pump_plugin_INNER_COMPILE_FLAG} ${Db_Flage})
# add_compile_options(${Db_Flage} ${pump_plugin_INNER_COMPILE_FLAG})

if(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
    set(pump_plugin_INNER_COMPILE_FLAG ${pump_plugin_INNER_COMPILE_FLAG}
            -Wl,-rpath=. 
            -Wno-write-strings 
            -fPIC)
    if(${pump_plugin_BUILD_FLAG} MATCHES "shared")
        set(pump_plugin_INNER_COMPILE_FLAG ${pump_plugin_INNER_COMPILE_FLAG}
            -DPUMP_PLUGIN_BUILD_DLL)
    endif()
    set(pump_plugin_INNER_LINK_FLAG ${pump_plugin_INNER_LINK_FLAG}
        -lpthread 
        -lrt)
elseif(${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
    set(pump_plugin_INNER_COMPILE_FLAG ${pump_plugin_INNER_COMPILE_FLAG}
        )
elseif(${CMAKE_GENERATOR} STREQUAL "MSYS Makefiles")
elseif(${CMAKE_GENERATOR} STREQUAL "Borland Makefiles")
elseif(${CMAKE_GENERATOR} STREQUAL "NMake Makefiles JOM")
elseif(${CMAKE_GENERATOR} STREQUAL "NMake Makefiles")
elseif(${CMAKE_GENERATOR} MATCHES "Visual Studio")
    set_property(GLOBAL PROPERTY USE_FOLDERS On)
    if(${pump_plugin_BUILD_FLAG} MATCHES "shared")
        set(pump_plugin_INNER_COMPILE_FLAG ${pump_plugin_INNER_COMPILE_FLAG}
            -DPUMP_PLUGIN_BUILD_DLL 
            -DPUMP_CORE_BUILD_DLL
            -DPUMP_EVENT_BUILD_DLL)
    endif()
    if(${CMAKE_GENERATOR} STREQUAL "Visual Studio 14 2015")
    elseif(${CMAKE_GENERATOR} STREQUAL "Visual Studio 14 2015 Win64")
    elseif(${CMAKE_GENERATOR} STREQUAL "Visual Studio 12 2013")
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

message("<pump::pump_plugin> /modules/pump_plugin/cmake/compiler_conf.cmake out")