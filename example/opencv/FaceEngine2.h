#pragma once
#include <map>
#include <memory>
#include <mutex>
#include <seeta/FaceDetector.h>
#include <seeta/FaceLandmarker.h>
#include <seeta/FaceRecognizer.h>
#include <seeta/GenderPredictor.h>
#include <seeta/AgePredictor.h>
#include <seeta/EyeStateDetector.h>
#include <seeta/FaceAntiSpoofing.h>
#include <seeta/MaskDetector.h>
using namespace seeta;
struct AntiSpoofing {
    int status;
    float clarity = 0, reality = 0;
};
struct DbInfo {
    std::string name;
    float score = 0;
};
struct FaceInfo : public SeetaFaceInfo {
    using Ptr = std::shared_ptr<FaceInfo>;
    FaceInfo(const SeetaFaceInfo& fi) : SeetaFaceInfo(fi){}
    DbInfo db;
    // 5点信息
    std::vector<SeetaPointF> points;
    std::shared_ptr<int> age;
    std::shared_ptr<float> mask;
    std::vector<float> features;
    std::shared_ptr<int> gender;
    std::shared_ptr<AntiSpoofing> anti;
    std::shared_ptr<std::pair<int, int>> eye;
    std::shared_ptr<seeta::ImageData> img;
    seeta::ImageData crop;
    int cmpCrop(int width, int height, int channel);
};

class FaceEngine2 {
    std::string model_dir_;
    std::shared_ptr<FaceDetector> fd;
    std::shared_ptr<FaceLandmarker> fl5;
    std::shared_ptr<FaceRecognizer> fr;
    seeta::ModelSetting getSetting(const char* name);

    std::vector<FaceInfo::Ptr> faces_;
    std::shared_ptr<seeta::ImageData> image_;
    // 人脸数据库
    std::map<std::string, std::shared_ptr<float>> face_db_;
    std::mutex face_lock_;
public:
    FaceEngine2() {}
    bool init(const char* dir, const char* fr_model = "face_recognizer.csta");
    const char* model_dir() const { return model_dir_.c_str(); }
    bool addFaceDb(const char* name, const SeetaImageData& data);
    bool addFaceDb(const char* name, void* rgb, int width, int height);
    bool delFaceDb(const char* name);
    void updateRgb(std::shared_ptr<seeta::ImageData> data);
    void updateRgb(void* rgb, int width, int height);
    int db_size() const { return face_db_.size(); }
    int face_size() const { return faces_.size(); }
    FaceInfo::Ptr get(int pos) {
        FaceInfo::Ptr ret = nullptr;
        if (pos >= 0 && pos < faces_.size())
            ret = faces_[pos];
        return ret;
    }

    // 提取人脸特征: 其中 feature 数组大小必须大于 fr->GetExtractFeatureSize();
    bool getFeature(int pos, float** feature);
    // 年龄检测
    std::shared_ptr<seeta::AgePredictor> age_;
    bool getAge(int pos, int& age);
    // 性别检测
    std::shared_ptr<seeta::GenderPredictor> gender_;
    bool getGender(int pos, int& ret);
    // 人眼检测
    std::shared_ptr<seeta::EyeStateDetector> eye_;
    bool getEyeStat(int pos, int& left, int& right);
    // 活体检测
    std::shared_ptr<seeta::FaceAntiSpoofing> anti_;
    bool getAnti(int pos, AntiSpoofing& ret);
    // 口罩检测
    std::shared_ptr<seeta::MaskDetector> mask_;
    bool getMask(int pos, float& ret);

    static const char* get_eye_status(int state);
    static const char* get_fas_status(int status);
};
