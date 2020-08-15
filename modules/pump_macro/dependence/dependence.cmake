#########################################################
# @file:    dependence.cmake
# @author:  yangzheng
# @brief:   modular pump_macro dependence configure.
#########################################################
message("<pump::pump_macro> /modules/pump_macro/dependence.cmake in")

# containing header dir to project
set(pump_macro_INNER_DEP_INC_DIRS
        ${pump_macro_INNER_PROJECT_ROOT_DIR}/dependence/include/${pump_OS_PLATFORM}
        ${pump_macro_INNER_PROJECT_ROOT_DIR}/src
        )
message("<pump::pump_macro> pump_macro_INNER_DEP_INC_DIRS=${pump_macro_INNER_DEP_INC_DIRS}")

# containing 3rdpart libs to project
include(${pump_macro_INNER_PROJECT_ROOT_DIR}/dependence/dlibs/CMakeLists.txt)
include(${pump_macro_INNER_PROJECT_ROOT_DIR}/dependence/slibs/CMakeLists.txt)
set(pump_macro_INNER_DEP_LIBS
        ${pump_macro_INNER_DEP_SLIBS}
        ${pump_macro_INNER_DEP_DLIBS}
        )
message("<pump::pump_macro> pump_macro_INNER_DEP_LIBS=${pump_macro_INNER_DEP_LIBS}")

message("<pump::pump_macro> /modules/pump_macro/dependence.cmake out")