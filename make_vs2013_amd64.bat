if exist "vs2013_amd64" (
cd vs2013_amd64
del /a/f/q "CMakeCache.txt"
) else (
mkdir vs2013_amd64
cd vs2013_amd64
)
cmake .. -G "Visual Studio 12 2013 Win64"
@pause