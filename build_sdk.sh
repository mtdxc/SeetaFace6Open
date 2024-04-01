#!/bin/bash
platform=x64
modules=(OpenRoleZoo SeetaAuthorize TenniS FaceBoxes FaceRecognizer6 FaceTracker6 Landmarker PoseEstimator6 QualityAssessor3 SeetaAgePredictor SeetaEyeStateDetector SeetaGenderPredictor SeetaMaskDetector FaceAntiSpoofingX6)
for d in ${modules[*]}; do
    echo $d
    pushd $d/craft
    sh build.linux.$platform.sh
    popd
done
