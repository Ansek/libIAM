IF /I "%~1"=="vb" (
    set p="C:\\\\Program Files (x86)\\\\IAM\\\\lib\\\\iam_plugins"
) ELSE (
    set p="D:\\\\SUAI\\\\IAM\\\\lib\\\\iam_plugins"
)
cd build &&^
cmake ..\..\.. -D CMAKE_C_COMPILER=gcc -D CMAKE_MAKE_PROGRAM=mingw32-make -G "MinGW Makefiles" -D IAM_INCLUDE_PLUGINS=ON -D IAM_BUILD_EXAMPLES=ON -D IAM_BUILD_TESTS=ON -D IAM_PLUGINS_DIR=%p% &&^
cmake --build . &&^
ctest -C Debug -VV
cd ..