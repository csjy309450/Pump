message("<pump::pump_macro> /modules/pump_macro/dependence/dlibs/CMakeLists.txt in")

# containing dlibs in project
# message("pump_OS_PLATFORM="${pump_OS_PLATFORM})
file(GLOB_RECURSE pump_macro_INNER_DEP_DLIBFILES
        "${pump_macro_INNER_PROJECT_ROOT_DIR}/dependence/dlibs/${pump_OS_PLATFORM}_${pump_CPUARCH_FLAGE}/${pump_BUILD_TYPE}/*")
# different platform are libs extension differently.
if (${pump_OS_PLATFORM} MATCHES "win")
    file(GLOB_RECURSE pump_macro_INNER_DEP_DLIBS
            "${pump_macro_INNER_PROJECT_ROOT_DIR}/dependence/dlibs/${pump_OS_PLATFORM}_${pump_CPUARCH_FLAGE}/${pump_BUILD_TYPE}/*.lib")
elseif(${pump_OS_PLATFORM} MATCHES "linux")
    file(GLOB_RECURSE pump_macro_INNER_DEP_DLIBS
            "${pump_macro_INNER_PROJECT_ROOT_DIR}/dependence/dlibs/${pump_OS_PLATFORM}_${pump_CPUARCH_FLAGE}/${pump_BUILD_TYPE}/*.so")
endif (${pump_OS_PLATFORM} MATCHES "win")
message("<pump::pump_macro> pump_macro_INNER_DEP_DLIBS=${pump_macro_INNER_DEP_DLIBS}")

# copy dlibs to output dir
if(MSVC)
    file(COPY ${pump_macro_INNER_DEP_DLIBFILES} DESTINATION ${pump_BINARY_DIR}/${pump_BUILD_TYPE})
else()
    file(COPY ${pump_macro_INNER_DEP_DLIBFILES} DESTINATION ${pump_BINARY_DIR})
endif()

message("<pump::pump_macro> /modules/pump_macro/dependence/dlibs/CMakeLists.txt out")

