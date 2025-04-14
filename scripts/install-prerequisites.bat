vcpkg install vcpkg-tool-ninja libpng freetype opengl glfw3 glew
if %errorlevel% neq 0 exit /b %errorlevel%
pip install pypng lz4 kconfiglib pcpp
if %errorlevel% neq 0 exit /b %errorlevel%
