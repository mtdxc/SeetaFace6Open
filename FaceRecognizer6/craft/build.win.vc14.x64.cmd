@echo off

set "BUILD_DIR=build.win.vc14.x64"
set "BUILD_TYPE=Release"
set "PLATFORM=x64"
set "PLATFORM_TARGET=x64"

set "ORZ_HOME=%~dp0/../../build"

set "INSTALL_DIR=%~dp0/../../build"

call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" %PLATFORM%

cd %~dp0

md "%BUILD_DIR%"

cd "%BUILD_DIR%"

md "%INSTALL_DIR%"

cmake "%~dp0.." ^
-G"Ninja" ^
-DCMAKE_BUILD_TYPE="%BUILD_TYPE%" ^
-DCONFIGURATION="%BUILD_TYPE%" ^
-DPLATFORM="%PLATFORM_TARGET%" ^
-DORZ_ROOT_DIR="%ORZ_HOME%" ^
-DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" ^
-DCMAKE_MODULE_PATH="" ^
-DSEETA_AUTHORIZE=OFF ^
-DSEETA_MODEL_ENCRYPT=ON

cmake --build . && cmake --install .

exit /b
