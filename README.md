# Pump: Light-Weighted C++ Framework

## 1.RESOURCES

- Homepage: https://github.com/csjy309450/Pump/wiki

## 2.REQUIRMENT

- boost ( 1.56.0 )
- glog
- gcc ( >= 5.4.0 ) 
- cmake ( >= VERSION 3.5 )

> All dependence was packged in project. Please unzip boost header by yourself in /modules/pump_core/dependence/include/win

## 3.BUILDING AND INSTALLATION

### CMake (Windows)

Install CMake: https://www.cmake.org

```shell
$ md build && cd build
$ cmake -G "Visual Studio 12 2013" ..   # Or use any generator you want to use. Run cmake --help for a list
$ cmake --build . --config Release # Or "start pump.sln" and build with menu in Visual Studio.
```

### CMake (Unix)

```shell
$ mkdir build && cd build
$ cmake ..     # Default to Unix Makefiles.
$ make
```

Should set BOOST_ROOT, Boost_USE_STATIC_LIBS, BOOST_LIBRARYDIR in some environment, as following

```shell
$ cmake .. -G "Visual Studio 12 2013"  -DBOOST_ROOT=[...] -DBoost_USE_STATIC_LIBS=[...] -DBOOST_LIBRARYDIR=[...]
```