message("<pump::pump_http> /modules/pump_http/src/pump_http/CMakeLists.txt in")

# find header dir
find_sub_dir(${pump_http_INNER_PROJECT_ROOT_DIR}/src/pump_http pump_http_INNER_INCLUDE_DIR)
set(pump_http_INNER_INCLUDE_DIR ${pump_http_INNER_INCLUDE_DIR} ${pump_http_INNER_DEP_INC_DIRS})
# include_directories(${pump_http_INNER_INCLUDE_DIR})
# find header and src code
find_header_in_dir(${pump_http_INNER_PROJECT_ROOT_DIR}/include/pump_http pump_http_INNER_HEADERS_1)
find_header_in_dir(${pump_http_INNER_PROJECT_ROOT_DIR}/src/pump_http pump_http_INNER_HEADERS_2)
set(pump_http_INNER_HEADERS ${pump_http_INNER_HEADERS_1} ${pump_http_INNER_HEADERS_2})
find_src_in_dir(${pump_http_INNER_PROJECT_ROOT_DIR}/src/pump_http pump_http_INNER_SRCS)
message("<pump::pump_http> pump_http_INNER_HEADERS>: ${pump_http_INNER_HEADERS}")
message("<pump::pump_http> pump_http_INNER_SRCS>: ${pump_http_INNER_SRCS}")

message("<pump::pump_http> /modules/pump_http/src/pump_http/CMakeLists.txt out")