message("<pump_ac> /cmake/Findpump_ac.cmake in")

if(NOT pump_ac_FOUND)
    add_subdirectory(${pump_ROOT_DIR}/modules/pump_ac/)
endif()

set(pump_ac_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/modules/pump_ac/include)
message("<pump_ac> pump_ac_INCLUDE_DIRS=${pump_ac_INCLUDE_DIRS}")

set(pump_ac_LIBRARIES "" CACHE STRING INTERNAL FORCE)
message("<pump_ac> pump_ac_LIBRARIES=${pump_ac_LIBRARIES}")

set(pump_ac_FOUND TRUE)
message("<pump_ac> pump_ac_FOUND=${pump_ac_FOUND}")

message("<pump_ac> /cmake/Findpump_ac.cmake out")