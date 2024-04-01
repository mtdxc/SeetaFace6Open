@echo off

set "modules=OpenRoleZoo SeetaAuthorize TenniS FaceBoxes FaceRecognizer6 FaceTracker6 Landmarker PoseEstimator6 QualityAssessor3 SeetaAgePredictor SeetaEyeStateDetector SeetaGenderPredictor SeetaMaskDetector FaceAntiSpoofingX6"
rem 由于旧代码是基于vs2015来编译的，而对于新的vs的vcvarsall.bat位置已变化，因此给出make.bat来生成库
rem 对于不同的vs版本，只需修改如下路径就行，
set vcvarsall_bin="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
rem 如要编译x86的库，请将此变量改成x86
set platform=x64
call %vcvarsall_bin% %platform%

for %%d in (%modules%) do (
    pushd %%d\craft
    call build.win.vc14.%platform%.cmd
    popd
)

for %%d in (%modules%) do (
    pushd %%d\craft
    call build.win.vc14.%platform%.debug.cmd
    popd
)
