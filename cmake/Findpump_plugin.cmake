message("<pump_plugin> /cmake/Findpump_plugin.cmake in")

if(NOT pump_plugin_FOUND)
    add_subdirectory(${pump_ROOT_DIR}/modules/pump_plugin/)
endif()

set(pump_plugin_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/modules/pump_plugin/include)
message("<pump_plugin> pump_plugin_INCLUDE_DIRS=${pump_plugin_INCLUDE_DIRS}")

set(pump_plugin_LIBRARIES "" CACHE STRING INTERNAL FORCE)
message("<pump_plugin> pump_plugin_LIBRARIES=${pump_plugin_LIBRARIES}")

set(pump_plugin_FOUND TRUE)
message("<pump_plugin> pump_plugin_FOUND=${pump_plugin_FOUND}")

message("<pump_plugin> /cmake/Findpump_plugin.cmake out")