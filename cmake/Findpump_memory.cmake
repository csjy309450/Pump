message("<pump::pump_memory>  ${PROJECT_SOURCE_DIR}/modules/pump_memory/CMakeLists.txt out")

message("<pump::pump_memory>  /cmake/Findpump_memory.cmake in")

if(NOT pump_memory_FOUND)
    add_subdirectory(${pump_ROOT_DIR}/modules/pump_memory/)
endif()

set(pump_memory_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/modules/pump_memory/include)
message("<pump::pump_memory>  pump_memory_INCLUDE_DIRS=${pump_memory_INCLUDE_DIRS}")

set(pump_memory_LIBRARIES "" CACHE STRING INTERNAL FORCE)
message("<pump::pump_memory>  pump_memory_LIBRARIES=${pump_memory_LIBRARIES}")

set(pump_memory_FOUND TRUE)
message("<pump::pump_memory>  pump_memory_FOUND=${pump_memory_FOUND}")

message("<pump::pump_memory>  /cmake/Findpump_memory.cmake out")