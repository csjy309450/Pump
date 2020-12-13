# aimed at containing static libs or windows import libs to project
if(${pump_OS_PLATFORM} STREQUAL "linux_x64")
    if(${pump_DEBUG_FLAG})
        set(pump_plugin_INNER_DEP_SLIBS
                )
    else()
        set(pump_plugin_INNER_DEP_SLIBS
                )
    endif()
elseif(${pump_OS_PLATFORM} STREQUAL "linux_x86")
    if(${pump_DEBUG_FLAG})
        set(pump_plugin_INNER_DEP_SLIBS
                )
    else()
        set(pump_plugin_INNER_DEP_SLIBS
                )
    endif()
elseif(${pump_OS_PLATFORM} STREQUAL "win_x64")
    if(${pump_DEBUG_FLAG})
        set(pump_plugin_INNER_DEP_SLIBS
                )
    else()
        set(pump_plugin_INNER_DEP_SLIBS
                )
    endif()
elseif(${pump_OS_PLATFORM} STREQUAL "win_x86")
    if(${pump_DEBUG_FLAG})
        set(pump_plugin_INNER_DEP_SLIBS
                )
    else()
        set(pump_plugin_INNER_DEP_SLIBS
                )
    endif()
endif()

file(GLOB_RECURSE pump_plugin_INNER_DEP_SLIBFILES
        "${pump_plugin_INNER_PROJECT_ROOT_DIR}/dependence/slibs/${pump_OS_PLATFORM}_${pump_CPUARCH_FLAGE}/${pump_BUILD_TYPE}/*")
# 不同平台库后缀不同
if (${pump_OS_PLATFORM} MATCHES "win")
    file(GLOB_RECURSE pump_plugin_INNER_DEP_SLIBS
            "${pump_plugin_INNER_PROJECT_ROOT_DIR}/dependence/slibs/${pump_OS_PLATFORM}_${pump_CPUARCH_FLAGE}/${pump_BUILD_TYPE}/*.lib")
elseif(${pump_OS_PLATFORM} MATCHES "linux")
    file(GLOB_RECURSE pump_plugin_INNER_DEP_SLIBS
            "${pump_plugin_INNER_PROJECT_ROOT_DIR}/dependence/slibs/${pump_OS_PLATFORM}_${pump_CPUARCH_FLAGE}/${pump_BUILD_TYPE}/*.a")
endif (${pump_OS_PLATFORM} MATCHES "win")
message("pump_plugin_INNER_DEP_SLIBS=${pump_plugin_INNER_DEP_SLIBS}")