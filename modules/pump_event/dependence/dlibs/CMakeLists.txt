# containing dlibs in project
#message("pump_OS_PLATFORM="${pump_OS_PLATFORM})
file(GLOB_RECURSE pump_core_INNER_DEP_DLIBFILES
        "${pump_core_INNER_PROJECT_ROOT_DIR}/dependence/dlibs/${pump_OS_PLATFORM}_${pump_CPUARCH_FLAGE}/${pump_BUILD_TYPE}/*")
# 不同平台库后缀不同
if (${pump_OS_PLATFORM} MATCHES "win")
    file(GLOB_RECURSE pump_core_INNER_DEP_DLIBS
            "${pump_core_INNER_PROJECT_ROOT_DIR}/dependence/dlibs/${pump_OS_PLATFORM}_${pump_CPUARCH_FLAGE}/${pump_BUILD_TYPE}/*.lib")
elseif(${pump_OS_PLATFORM} MATCHES "linux")
    file(GLOB_RECURSE pump_core_INNER_DEP_DLIBS
            "${pump_core_INNER_PROJECT_ROOT_DIR}/dependence/dlibs/${pump_OS_PLATFORM}_${pump_CPUARCH_FLAGE}/${pump_BUILD_TYPE}/*.so")
endif (${pump_OS_PLATFORM} MATCHES "win")
message("pump_core_INNER_DEP_DLIBS=${pump_core_INNER_DEP_DLIBS}")

# copy dlibs to output dir
if(MSVC)
    file(COPY ${pump_core_INNER_DEP_DLIBFILES} DESTINATION ${pump_BINARY_DIR}/${pump_BUILD_TYPE})
else()
    file(COPY ${pump_core_INNER_DEP_DLIBFILES} DESTINATION ${pump_BINARY_DIR})
endif()

