message("<pump::pump_test> /cmake/Findpump_test.cmake in")

if(NOT pump_test_FOUND)
    add_subdirectory(${pump_ROOT_DIR}/modules/pump_test/)
endif()

set(pump_test_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/modules/pump_test/include)
message("<pump::pump_test> pump_test_INCLUDE_DIRS=${pump_test_INCLUDE_DIRS}")

set(pump_test_LIBRARIES "" CACHE STRING INTERNAL FORCE)
message("<pump::pump_test> pump_test_LIBRARIES=${pump_test_LIBRARIES}")

set(pump_test_FOUND TRUE)
message("<pump::pump_test> pump_test_FOUND=${pump_test_FOUND}")

message("<pump::pump_test> /cmake/Findpump_test.cmake out")