message("<pump::pump_memory> /modules/pump_memory/src/pump_core/CMakeLists.txt in")

# find header dir
find_sub_dir(${pump_memory_INNER_PROJECT_ROOT_DIR}/src/pump_memory pump_memory_INNER_INCLUDE_DIR)
set(pump_memory_INNER_INCLUDE_DIR ${pump_memory_INNER_INCLUDE_DIR} ${pump_memory_INNER_DEP_INC_DIRS})
include_directories(${pump_memory_INNER_INCLUDE_DIR})
# find header and src code
find_header_in_dir(${pump_memory_INNER_PROJECT_ROOT_DIR}/include/pump_memory pump_memory_INNER_HEADERS_1)
find_header_in_dir(${pump_memory_INNER_PROJECT_ROOT_DIR}/src/pump_memory pump_memory_INNER_HEADERS_2)
set(pump_memory_INNER_HEADERS ${pump_memory_INNER_HEADERS_1} ${pump_memory_INNER_HEADERS_2})
find_src_in_dir(${pump_memory_INNER_PROJECT_ROOT_DIR}/src/pump_memory pump_memory_INNER_SRCS)
message("<pump::pump_memory> pump_memory_INNER_HEADERS>: ${pump_memory_INNER_HEADERS}")
message("<pump::pump_memory> pump_memory_INNER_SRCS>: ${pump_memory_INNER_SRCS}")

message("<pump::pump_memory> /modules/pump_memory/src/pump_core/CMakeLists.txt out")