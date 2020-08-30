message("<pump::pump_function> /cmake/Findpump_function.cmake in")

if(NOT pump_function_FOUND)
    add_subdirectory(${pump_ROOT_DIR}/modules/pump_function/)
endif()

set(pump_function_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/modules/pump_function/include)
message("<pump::pump_function> pump_function_INCLUDE_DIRS=${pump_function_INCLUDE_DIRS}")

# set(pump_function_LIBRARIES "" CACHE STRING INTERNAL FORCE)
message("<pump::pump_function> pump_function_LIBRARIES=${pump_function_LIBRARIES}")

set(pump_function_FOUND TRUE)
message("<pump::pump_function> pump_function_FOUND=${pump_function_FOUND}")

message("<pump::pump_function> /cmake/Findpump_function.cmake out")