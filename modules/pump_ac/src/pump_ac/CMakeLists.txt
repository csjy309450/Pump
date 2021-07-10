message("<pump::pump_ac> /modules/pump_ac/src/pump_ac/CMakeLists.txt in")

# find header dir
find_sub_dir(${pump_ac_INNER_PROJECT_ROOT_DIR}/src/pump_ac pump_ac_INNER_INCLUDE_DIR)
set(pump_ac_INNER_INCLUDE_DIR ${pump_ac_INNER_INCLUDE_DIR} ${pump_ac_INNER_DEP_INC_DIRS})
# include_directories(${pump_ac_INNER_INCLUDE_DIR})
# find header and src code
find_header_in_dir(${pump_ac_INNER_PROJECT_ROOT_DIR}/include/pump_ac pump_ac_INNER_HEADERS_1)
find_header_in_dir(${pump_ac_INNER_PROJECT_ROOT_DIR}/src/pump_ac pump_ac_INNER_HEADERS_2)
set(pump_ac_INNER_HEADERS ${pump_ac_INNER_HEADERS_1} ${pump_ac_INNER_HEADERS_2})
find_src_in_dir(${pump_ac_INNER_PROJECT_ROOT_DIR}/src/pump_ac pump_ac_INNER_SRCS)
message("<pump::pump_ac> pump_ac_INNER_HEADERS>: ${pump_ac_INNER_HEADERS}")
message("<pump::pump_ac> pump_ac_INNER_SRCS>: ${pump_ac_INNER_SRCS}")

message("<pump::pump_ac> /modules/pump_ac/src/pump_ac/CMakeLists.txt out")