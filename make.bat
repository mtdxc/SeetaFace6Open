@echo off

set "modules=OpenRoleZoo SeetaAuthorize TenniS FaceBoxes FaceRecognizer6 FaceTracker6 Landmarker PoseEstimator6 QualityAssessor3 SeetaAgePredictor SeetaEyeStateDetector SeetaGenderPredictor SeetaMaskDetector FaceAntiSpoofingX6"
rem 由于旧代码是基于vs2015来编译的，而对于新的vs的vcvarsall.bat位置已变化，因此给出make.bat来生成库
rem 对于不同的vs版本，只需修改如下路径就行，
rem 此外jom -j16 install可改成 
rem msbuild.exe /m:8 ALL_BUILD.vcxproj
rem msbuild.exe /m:8 INSTALL.vcxproj
rem 此外vs版本需要，可能还需修改 OpenRoleZoo\include\orz\mem\pot.h 增加 #include <functional>
set vcvarsall_bin="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
call %vcvarsall_bin% x64

for %%d in (%modules%) do (
    pushd %%d\craft
    call build.win.vc14.x64.cmd
    popd
)

call %vcvarsall_bin% x86

for %%d in (%modules%) do (
    pushd %%d\craft
    call build.win.vc14.x86.cmd
    popd
)