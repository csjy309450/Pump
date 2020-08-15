message("<pump::pump_core> /cmake/Findpump_core.cmake in")

if(NOT pump_core_FOUND)
    add_subdirectory(${pump_ROOT_DIR}/modules/pump_core/)
endif()

set(pump_core_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/modules/pump_core/include)
message("<pump::pump_core> pump_core_INCLUDE_DIRS=${pump_core_INCLUDE_DIRS}")

set(pump_core_LIBRARIES "" CACHE STRING INTERNAL FORCE)
message("<pump::pump_core> pump_core_LIBRARIES=${pump_core_LIBRARIES}")

set(pump_core_FOUND TRUE)
message("<pump::pump_core> pump_core_FOUND=${pump_core_FOUND}")

message("<pump::pump_core> /cmake/Findpump_core.cmake out")