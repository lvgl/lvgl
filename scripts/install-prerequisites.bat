vcpkg install vcpkg-tool-ninja libpng freetype opengl glfw3
if %errorlevel% neq 0 exit /b %errorlevel%
pip install pypng lz4 kconfiglib
if %errorlevel% neq 0 exit /b %errorlevel%
