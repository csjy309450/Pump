if exist "vs2015_amd64" (
cd vs2015_amd64
del /a/f/q "CMakeCache.txt"
) else (
mkdir vs2015_amd64
cd vs2015_amd64
)
cmake .. -G "Visual Studio 14 2015 Win64"
@pause