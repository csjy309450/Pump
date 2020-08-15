message("<pump> /cmake/platforms/CMakeLists.txt in")

message("<pump> CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
message("<pump> pump_CPUARCH_FLAGE=${pump_CPUARCH_FLAGE}")
if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(pump_OS_PLATFORM "linux")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(pump_OS_PLATFORM "win")
endif()
message("<pump> pump_OS_PLATFORM=${pump_OS_PLATFORM}")

message("<pump> /cmake/platforms/CMakeLists.txt out")