message("<pump::pump_function> /modules/pump_function/src/pump_function/CMakeLists.txt in")

# find header dir
find_sub_dir(${pump_function_INNER_PROJECT_ROOT_DIR}/src/pump_function pump_function_INNER_INCLUDE_DIR)
set(pump_function_INNER_INCLUDE_DIR ${pump_function_INNER_INCLUDE_DIR} ${pump_function_INNER_DEP_INC_DIRS})
# find header and src code
find_header_in_dir(${pump_function_INNER_PROJECT_ROOT_DIR}/include/pump_function pump_function_INNER_HEADERS_1)
find_header_in_dir(${pump_function_INNER_PROJECT_ROOT_DIR}/src/pump_function pump_function_INNER_HEADERS_2)
set(pump_function_INNER_HEADERS ${pump_function_INNER_HEADERS_1} ${pump_function_INNER_HEADERS_2})
find_src_in_dir(${pump_function_INNER_PROJECT_ROOT_DIR}/src/pump_function pump_function_INNER_SRCS)
message("<pump::pump_function> pump_function_INNER_HEADERS=${pump_function_INNER_HEADERS}")
message("<pump::pump_function> pump_function_INNER_SRCS=${pump_function_INNER_SRCS}")

message("<pump::pump_function> /modules/pump_function/src/pump_function/CMakeLists.txt out")
