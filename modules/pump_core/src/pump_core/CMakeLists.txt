message("<pump::pump_core> /modules/pump_core/src/pump_core/CMakeLists.txt in")

# find header dir
find_sub_dir(${pump_core_INNER_PROJECT_ROOT_DIR}/src/pump_core pump_core_INNER_INCLUDE_DIR)
set(pump_core_INNER_INCLUDE_DIR ${pump_core_INNER_INCLUDE_DIR} ${pump_core_INNER_DEP_INC_DIRS})
# include_directories(${pump_core_INNER_INCLUDE_DIR})
# find header and src code
find_header_in_dir(${pump_core_INNER_PROJECT_ROOT_DIR}/include/pump_core pump_core_INNER_HEADERS_1)
find_header_in_dir(${pump_core_INNER_PROJECT_ROOT_DIR}/src/pump_core pump_core_INNER_HEADERS_2)
set(pump_core_INNER_HEADERS ${pump_core_INNER_HEADERS_1} ${pump_core_INNER_HEADERS_2})
find_src_in_dir(${pump_core_INNER_PROJECT_ROOT_DIR}/src/pump_core pump_core_INNER_SRCS)
message("<pump::pump_core> pump_core_INNER_HEADERS=${pump_core_INNER_HEADERS}")
message("<pump::pump_core> pump_core_INNER_SRCS=${pump_core_INNER_SRCS}")

message("<pump::pump_core> /modules/pump_core/src/pump_core/CMakeLists.txt out")