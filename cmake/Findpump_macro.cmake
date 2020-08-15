message("<pump::pump_macro> /cmake/Findpump_macro.cmake in")

if(NOT pump_macro_FOUND)
    add_subdirectory(${pump_ROOT_DIR}/modules/pump_macro/)
endif()

set(pump_macro_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/modules/pump_macro/include)
message("<pump::pump_macro> pump_macro_INCLUDE_DIRS=${pump_macro_INCLUDE_DIRS}")

set(pump_macro_LIBRARIES "" CACHE STRING INTERNAL FORCE)
message("<pump::pump_macro> pump_macro_LIBRARIES=${pump_macro_LIBRARIES}")

set(pump_macro_FOUND TRUE)
message("<pump::pump_macro> pump_macro_FOUND=${pump_macro_FOUND}")

message("<pump::pump_macro> /cmake/Findpump_macro.cmake out")