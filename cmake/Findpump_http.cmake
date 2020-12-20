message("<pump::pump_http> /cmake/Findpump_http.cmake in")

if(NOT pump_http_FOUND)
    add_subdirectory(${pump_ROOT_DIR}/modules/pump_http/)
endif()

set(pump_http_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/modules/pump_http/include)
message("<pump::pump_http> pump_http_INCLUDE_DIRS=${pump_http_INCLUDE_DIRS}")

set(pump_http_LIBRARIES "" CACHE STRING INTERNAL FORCE)
message("<pump::pump_http> pump_http_LIBRARIES=${pump_http_LIBRARIES}")

set(pump_http_FOUND TRUE)
message("<pump::pump_http> pump_http_FOUND=${pump_http_FOUND}")

message("<pump::pump_http> /cmake/Findpump_http.cmake out")