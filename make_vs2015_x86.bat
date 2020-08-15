if exist "vs2015_x86" (
cd vs2015_x86
del /a/f/q "CMakeCache.txt"
) else (
mkdir vs2015_x86
cd vs2015_x86
)
cmake .. -G "Visual Studio 14 2015"
@pause