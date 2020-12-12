message("<pump::pump_event> /cmake/Findpump_event.cmake in")

if(NOT pump_event_FOUND)
    add_subdirectory(${pump_ROOT_DIR}/modules/pump_event/)
endif()

set(pump_event_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/modules/pump_event/include)
message("<pump::pump_event> pump_event_INCLUDE_DIRS=${pump_event_INCLUDE_DIRS}")

# set(pump_event_LIBRARIES "" CACHE STRING INTERNAL FORCE)
message("<pump::pump_event> pump_event_LIBRARIES=${pump_event_LIBRARIES}")

set(pump_event_FOUND TRUE)
message("<pump::pump_event> pump_event_FOUND=${pump_event_FOUND}")

message("<pump::pump_event> /cmake/Findpump_event.cmake out")