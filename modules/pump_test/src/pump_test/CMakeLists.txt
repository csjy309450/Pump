message("<pump::pump_test> /modules/pump_test/src/pump_test/CMakeLists.txt in")

# find header dir
find_sub_dir(${pump_test_INNER_PROJECT_ROOT_DIR}/src/pump_test pump_test_INNER_INCLUDE_DIR)
set(pump_test_INNER_INCLUDE_DIR ${pump_test_INNER_INCLUDE_DIR} ${pump_test_INNER_DEP_INC_DIRS})
# find header and src code
find_header_in_dir(${pump_test_INNER_PROJECT_ROOT_DIR}/include/pump_test pump_test_INNER_HEADERS_1)
find_header_in_dir(${pump_test_INNER_PROJECT_ROOT_DIR}/src/pump_test pump_test_INNER_HEADERS_2)
set(pump_test_INNER_HEADERS ${pump_test_INNER_HEADERS_1} ${pump_test_INNER_HEADERS_2})
find_src_in_dir(${pump_test_INNER_PROJECT_ROOT_DIR}/src/pump_test pump_test_INNER_SRCS)
message("<pump::pump_test> pump_test_INNER_HEADERS>=${pump_test_INNER_HEADERS}")
message("<pump::pump_test> pump_test_INNER_SRCS>=${pump_test_INNER_SRCS}")

message("<pump::pump_test>  /modules/pump_test/src/pump_test/CMakeLists.txt out")