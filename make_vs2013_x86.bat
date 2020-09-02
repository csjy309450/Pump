if exist "vs2013_x86" (
cd vs2013_x86
del /a/f/q "CMakeCache.txt"
) else (
mkdir vs2013_x86
cd vs2013_x86
)
cmake .. -G "Visual Studio 12 2013"