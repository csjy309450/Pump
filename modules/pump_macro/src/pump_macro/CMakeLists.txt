message("<pump::pump_macro> /modules/pump_macro/src/pump_macro/CMakeLists.txt in")

# find header dir
find_sub_dir(${pump_macro_INNER_PROJECT_ROOT_DIR}/src/pump_macro pump_macro_INNER_INCLUDE_DIR)
set(pump_macro_INNER_INCLUDE_DIR ${pump_macro_INNER_INCLUDE_DIR} ${pump_macro_INNER_DEP_INC_DIRS})
# find header and src code
find_header_in_dir(${pump_macro_INNER_PROJECT_ROOT_DIR}/src/pump_macro pump_macro_INNER_HEADERS)
find_src_in_dir(${pump_macro_INNER_PROJECT_ROOT_DIR}/src/pump_macro pump_macro_INNER_SRCS)
message("<pump::pump_macro> pump_macro_INNER_HEADERS=${pump_macro_INNER_HEADERS}")
message("<pump::pump_macro> pump_macro_INNER_SRCS=${pump_macro_INNER_SRCS}")

message("<pump::pump_macro> /modules/pump_macro/src/pump_macro/CMakeLists.txt out")