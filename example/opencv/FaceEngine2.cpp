#include "FaceEngine2.h"
using namespace seeta;

#ifdef _DEBUG
//release 库,11个
#pragma comment(lib,"SeetaFaceDetector600d.lib") 
#pragma comment(lib,"SeetaFaceLandmarker600d.lib")

#pragma comment(lib,"SeetaFaceRecognizer610d.lib")
#pragma comment(lib,"SeetaGenderPredictor600d.lib") 
#pragma comment(lib,"SeetaAgePredictor600d.lib") 
#pragma comment(lib,"SeetaFaceAntiSpoofingX600d.lib") 
#pragma comment(lib,"SeetaEyeStateDetector200d.lib")
#pragma comment(lib,"SeetaMaskDetector200d.lib")

//这几个没用到
#pragma comment(lib,"SeetaFaceTracking600d.lib") 
#pragma comment(lib,"SeetaPoseEstimation600d.lib")
#else
//release 库,11个
#pragma comment(lib,"SeetaFaceDetector600.lib") 
#pragma comment(lib,"SeetaFaceLandmarker600.lib")

#pragma comment(lib,"SeetaFaceRecognizer610.lib")
#pragma comment(lib,"SeetaGenderPredictor600.lib") 
#pragma comment(lib,"SeetaAgePredictor600.lib") 
#pragma comment(lib,"SeetaFaceAntiSpoofingX600.lib") 
#pragma comment(lib,"SeetaEyeStateDetector200.lib")
#pragma comment(lib,"SeetaMaskDetector200.lib")

//这几个没用到
#pragma comment(lib,"SeetaFaceTracking600.lib") 
#pragma comment(lib,"SeetaPoseEstimation600.lib")
//#pragma comment(lib,"SeetaQualityAssessor300.lib")
#endif

ModelSetting FaceEngine2::getSetting(const char* name) {
    ModelSetting setting;
    setting.append(model_dir_ + name);
    return setting;
}

bool FaceEngine2::init(const char* dir, const char* fr_model) {
    if (dir) {
        model_dir_ = dir;
        char end = *model_dir_.rbegin();
        if (end != '\\' || end != '/') {
            model_dir_.push_back('/');
        }
    }
    fd = std::make_shared<FaceDetector>(getSetting("face_detector.csta"));
    fl5 = std::make_shared<FaceLandmarker>(getSetting("face_landmarker_pts5.csta"));
    fr = std::make_shared<FaceRecognizer>(getSetting(fr_model));
    return true;
}

bool FaceEngine2::addFaceDb(const char* name, void* rgb, int width, int height) {
    if (!fr || !name || !rgb) return false;
    SeetaImageData data;
    data.width = width;
    data.height = height;
    data.channels = 3;
    data.data = (unsigned char*)rgb;
    return addFaceDb(name, data);
}

bool FaceEngine2::addFaceDb(const char* name, const SeetaImageData& data) {
    if (!fr || !name) return false;
    std::unique_lock<decltype(face_lock_)> l(face_lock_);
    auto faces = fd->detect(data);
    if (!faces.size) {
        printf("no face!\n");
        return false;
    }
    auto pts = fl5->mark(data, faces.data[0].pos);
    std::shared_ptr<float> features(
        new float[fr->GetExtractFeatureSize()],
        std::default_delete<float[]>());
    if (fr->Extract(data, pts.data(), features.get())) {
        face_db_[name] = features;
        return true;
    }
    return false;
}

bool FaceEngine2::delFaceDb(const char* name) {
    bool ret = false;
    std::unique_lock<decltype(face_lock_)> l(face_lock_);
    auto it = face_db_.find(name);
    if (face_db_.end() != it) {
        face_db_.erase(it);
        ret = true;
    }
    return ret;
}

void FaceEngine2::updateRgb(void* rgb, int width, int height) {
    SeetaImageData data;
    data.width = width;
    data.height = height;
    data.channels = 3;
    data.data = (unsigned char*)rgb;
    return updateRgb(std::make_shared<seeta::ImageData>(data));
}

void FaceEngine2::updateRgb(std::shared_ptr<seeta::ImageData> data)
{
    if (!fd || !fl5) return;
    if (image_ == data) return;
    image_ = data;
    faces_.clear();
    std::unique_lock<decltype(face_lock_)> l(face_lock_);
    auto faces = fd->detect(*image_);
    for (size_t i = 0; i < faces.size; i++)
    {
        auto fi = std::make_shared<FaceInfo>(faces.data[i]);
        fi->img = image_;
        fi->points = fl5->mark(*image_, fi->pos);
        faces_.push_back(fi);
        if (face_db_.size() && fr) {
            float* features = nullptr;
            if (getFeature(i, &features) && features) {
                DbInfo& db = fi->db;
                // std::unique_lock<decltype(face_lock_)> l(face_lock_);
                for (auto it : face_db_) {
                    float score = fr->CalculateSimilarity(it.second.get(), features);
                    if (score > db.score) {
                        db.score = score;
                        db.name = it.first;
                    }
                }
            }
        }
    }
}

bool FaceEngine2::getFeature(int pos, float** feature) {
    auto face = get(pos);
    if (!face || !fr) return false;
    if (face->features.size() < fr->GetExtractFeatureSize())
        face->features.resize(fr->GetExtractFeatureSize());
    *feature = face->features.data();
    face->crop = fr->CropFaceV2(*face->img, face->points.data());
    return fr->ExtractCroppedFace(face->crop, face->features.data());
    //return fr->Extract(*face->img, face->points.data(), face->features.data());
}

bool FaceEngine2::getAge(int pos, int& age) {
    auto face = get(pos);
    if (!face) return false;
    bool ret = true;
    if (face->age) {
        age = *face->age;
    }
    else {
        if (!age_) age_ = std::make_shared<AgePredictor>(getSetting("age_predictor.csta"));
        if(face->cmpCrop(age_->GetCropFaceWidth(), age_->GetCropFaceHeight(), age_->GetCropFaceChannels())) {
            ret = age_->CropFace(*face->img, face->points.data(), face->crop);
        }
        ret = age_->PredictAge(face->crop, age);
        //ret = age_->PredictAgeWithCrop(*face->img, face->points.data(), age);
        if (ret) {
            face->age = std::make_shared<int>(age);
        }
    }
    return ret;
}

bool FaceEngine2::getGender(int pos, int& val) {
    auto face = get(pos);
    if (!face) return false;
    bool ret = true;
    if (face->gender) {
        val = *face->gender;
    }
    else {
        if (!gender_) gender_ = std::make_shared<GenderPredictor>(getSetting("gender_predictor.csta"));
        GenderPredictor::GENDER g;
        if (face->cmpCrop(gender_->GetCropFaceWidth(), gender_->GetCropFaceHeight(), gender_->GetCropFaceChannels())) {
            ret = gender_->CropFace(*face->img, face->points.data(), face->crop);
        }
        ret = gender_->PredictGender(face->crop, g);
        //ret = gender_->PredictGenderWithCrop(*face->img, face->points.data(), g);
        if (ret) {
            face->gender = std::make_shared<int>(g);
            val = g;
        }
    }
    return ret;
}

bool FaceEngine2::getEyeStat(int pos, int& left, int& right) {
    auto face = get(pos);
    if (!face) return false;
    bool ret = true;
    if (face->eye) {
        left = face->eye->first;
        right = face->eye->second;
    }
    else {
        if (!eye_)
            eye_ = std::make_shared<EyeStateDetector>(getSetting("eye_state.csta"));
        EyeStateDetector::EYE_STATE l, r;
        eye_->Detect(*face->img, face->points.data(), l, r);
        left = l;
        right = r;
        face->eye.reset(new std::pair<int, int>(left, right));
        return true;
    }
}

bool FaceEngine2::getAnti(int pos, AntiSpoofing& val) {
    auto face = get(pos);
    if (!face) return false;
    bool ret = true;
    if (face->anti) {
        val = *face->anti;
    }
    else {
        if (!anti_) {
            ModelSetting anti_setting;
            anti_setting.append(model_dir_ + "fas_first.csta");
            anti_setting.append(model_dir_ + "fas_second.csta");
            anti_ = std::make_shared<FaceAntiSpoofing>(anti_setting);
            anti_->SetThreshold(0.3, 0.90);//设置默认阈值，另外一组阈值为(0.7, 0.55)
            anti_->SetBoxThresh(0.9);
        }
        val.status = anti_->Predict(*face->img, face->pos, face->points.data());
        anti_->GetPreFrameScore(&val.clarity, &val.reality);
        face->anti = std::make_shared<AntiSpoofing>(val);
    }
    return ret;
}

bool FaceEngine2::getMask(int pos, float& ret) {
    auto face = get(pos);
    if (!face) return false;
    if (face->mask) {
        ret = *face->mask;
        return true;
    }
    else {
        if (!mask_) {
            mask_ = std::make_shared<MaskDetector>(getSetting("mask_detector.csta"));
        }
        mask_->detect(*face->img, face->pos, &ret);
        face->mask = std::make_shared<float>(ret);
        return true;
    }
}

const char* FaceEngine2::get_eye_status(int state)
{
    if (state == EyeStateDetector::EYE_CLOSE)
        return "闭合";
    else if (state == EyeStateDetector::EYE_OPEN)
        return "张开";
    else if (state == EyeStateDetector::EYE_RANDOM)
        return "无法判断";
    else
        return "无法判断";
}

const char* FaceEngine2::get_fas_status(int status) {
    switch (status) {
    case FaceAntiSpoofing::REAL:
        return "真实人脸";
    case FaceAntiSpoofing::SPOOF:
        return "照片人脸";
    case FaceAntiSpoofing::FUZZY:
        return "无法判断";
    case FaceAntiSpoofing::DETECTING:
        return "正在检测";
    }
    return "无法判断";
}

inline int FaceInfo::cmpCrop(int width, int height, int channel) {
    if (crop.channels == channel && crop.width == width && crop.height == crop.height)
        return 0;
    printf("%x reset crop %dx%dx%d", width, height, channel);
    //return -1;
    crop = seeta::ImageData(width, height, channel);
    return 1;
}
