IF /I "%~1"=="vb" (
    set p=
) ELSE (
    set p=--prefix="D:/SUAI/IAM"
)
cd build &&^
cmake --install . %p% &&^
.\examples\plugin_viewer\plugin_viewer.exe
cd ..