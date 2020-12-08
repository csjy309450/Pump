# aimed at containing static libs or windows import libs to project
if(${pump_OS_PLATFORM} STREQUAL "linux_x64")
    if(${pump_DEBUG_FLAG})
        set(pump_function_INNER_DEP_SLIBS
                )
    else()
        set(pump_function_INNER_DEP_SLIBS
                )
    endif()
elseif(${pump_OS_PLATFORM} STREQUAL "linux_x86")
    if(${pump_DEBUG_FLAG})
        set(pump_function_INNER_DEP_SLIBS
                )
    else()
        set(pump_function_INNER_DEP_SLIBS
                )
    endif()
elseif(${pump_OS_PLATFORM} STREQUAL "win_x64")
    if(${pump_DEBUG_FLAG})
        set(pump_function_INNER_DEP_SLIBS
                )
    else()
        set(pump_function_INNER_DEP_SLIBS
                )
    endif()
elseif(${pump_OS_PLATFORM} STREQUAL "win_x86")
    if(${pump_DEBUG_FLAG})
        set(pump_function_INNER_DEP_SLIBS
                )
    else()
        set(pump_function_INNER_DEP_SLIBS
                )
    endif()
endif()