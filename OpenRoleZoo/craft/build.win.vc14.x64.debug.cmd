@echo off

set "BUILD_DIR=build.win.vc14.x64.debug"
set "BUILD_TYPE=Debug"
set "PLATFORM=amd64"
set "PLATFORM_TARGET=x64"

set "WITH_SSL=OFF"

set "INSTALL_DIR=../../../build"

call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" %PLATFORM%

cd %~dp0

md "%BUILD_DIR%"

cd "%BUILD_DIR%"

md "%INSTALL_DIR%"

cmake "%~dp0.." ^
-G"Ninja" ^
-DCMAKE_BUILD_TYPE="%BUILD_TYPE%" ^
-DPLATFORM="%PLATFORM_TARGET%" ^
-DORZ_WITH_OPENSSL="%WITH_SSL%" ^
-DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%"

cmake --build . && cmake --install .

exit /b

