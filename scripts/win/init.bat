mkdir build
IF /I "%~1"=="vb" (
    git config --global --add safe.directory "%%(prefix)///VBoxSvr/router/libIAM/scripts/win/build/_deps/unity-src"
    git config --global --add safe.directory "%%(prefix)///VBoxSvr/router/libIAM/scripts/win/build/_deps/fff-src"
    git config --global --add safe.directory "%%(prefix)///VBoxSvr/router/libIAM/scripts/win/build/_deps/jansson-src"    
)