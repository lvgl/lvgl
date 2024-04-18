vcpkg install vcpkg-tool-ninja libpng freetype
if %errorlevel% neq 0 exit /b %errorlevel%
pip install pypng lz4
if %errorlevel% neq 0 exit /b %errorlevel%
