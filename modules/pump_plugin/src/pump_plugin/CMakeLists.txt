message("<pump::pump_plugin> /modules/pump_plugin/src/pump_plugin/CMakeLists.txt in")

# find header dir
find_sub_dir(${pump_plugin_INNER_PROJECT_ROOT_DIR}/src/pump_plugin pump_plugin_INNER_INCLUDE_DIR)
set(pump_plugin_INNER_INCLUDE_DIR ${pump_plugin_INNER_INCLUDE_DIR} ${pump_plugin_INNER_DEP_INC_DIRS})
# include_directories(${pump_plugin_INNER_INCLUDE_DIR})
# find header and src code
find_header_in_dir(${pump_plugin_INNER_PROJECT_ROOT_DIR}/include/pump_plugin pump_plugin_INNER_HEADERS_1)
find_header_in_dir(${pump_plugin_INNER_PROJECT_ROOT_DIR}/src/pump_plugin pump_plugin_INNER_HEADERS_2)
set(pump_plugin_INNER_HEADERS ${pump_plugin_INNER_HEADERS_1} ${pump_plugin_INNER_HEADERS_2})
find_src_in_dir(${pump_plugin_INNER_PROJECT_ROOT_DIR}/src/pump_plugin pump_plugin_INNER_SRCS)
message("<pump::pump_plugin> pump_plugin_INNER_HEADERS>: ${pump_plugin_INNER_HEADERS}")
message("<pump::pump_plugin> pump_plugin_INNER_SRCS>: ${pump_plugin_INNER_SRCS}")

message("<pump::pump_plugin> /modules/pump_plugin/src/pump_plugin/CMakeLists.txt out")