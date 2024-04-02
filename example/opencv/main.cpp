
#include <iostream>
#include <opencv2/opencv.hpp>
//用到seetaface的7个模块
#include <seeta/FaceDetector.h>
#include <seeta/FaceLandmarker.h>
#include <seeta/FaceRecognizer.h>
#include <seeta/GenderPredictor.h>
#include <seeta/AgePredictor.h>
#include <seeta/EyeStateDetector.h>
#include <seeta/FaceAntiSpoofing.h>

#pragma comment(lib, "opencv_world490.lib")
#pragma comment(lib, "opencv_img_hash490.lib")
#ifdef _DEBUG
//release 库,11个
#pragma comment(lib,"SeetaFaceDetector600d.lib") 
#pragma comment(lib,"SeetaFaceLandmarker600d.lib")

#pragma comment(lib,"SeetaFaceRecognizer610d.lib")
#pragma comment(lib,"SeetaGenderPredictor600d.lib") 
#pragma comment(lib,"SeetaAgePredictor600d.lib") 
#pragma comment(lib,"SeetaFaceAntiSpoofingX600d.lib") 
#pragma comment(lib,"SeetaEyeStateDetector200d.lib")

//这四个没用到
#pragma comment(lib,"SeetaMaskDetector200d.lib")
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

//这四个没用到
#pragma comment(lib,"SeetaMaskDetector200.lib")
#pragma comment(lib,"SeetaFaceTracking600.lib") 
#pragma comment(lib,"SeetaPoseEstimation600.lib")
#pragma comment(lib,"SeetaQualityAssessor300.lib")
#endif

using namespace seeta;
using namespace std;
using namespace cv;

void drawResult(Scalar color, const std::vector<string>& labels, int classId, float conf, int left, int top, int right, int bottom, Mat& frame)
{
    int fontHeight = 25;
    int fontTotalHeight = fontHeight * labels.size();
    int thickness = -1;
    int linestyle = LineTypes::LINE_AA;
    int baseline = 0;
    int padding = 5;

    int max_label_index = 0;
    for (int i = 1; i < labels.size(); i++) {
        if (labels[i].length() > labels[max_label_index].length())
            max_label_index = i;
    }
    cv::Size text_size = cv::getTextSize(labels[max_label_index], fontHeight, 1.0f, thickness = 0, &baseline);

    fontTotalHeight += 2 * padding + 2* labels.size();
    text_size.width += 2 * padding;


    cv::Point pt1, pt2;
    cv::Point pt_text_bg1, pt_text_bg2;
    cv::Point pt_text;


    //物体框
    pt1.x = left;
    pt1.y = top;
    pt2.x = right;
    pt2.y = bottom;


    //文本背景框
    pt_text_bg1.x = left;
    pt_text_bg1.y = top - fontTotalHeight;
    pt_text_bg2.x = std::max(left + text_size.width, right);
    pt_text_bg2.y = top;


    //文本原点(左下角)
    pt_text.x = left + padding;
    pt_text.y = top - padding;

    static int rect_line_width = 2;//std::max(1.0f, show_img->rows * .002f);
    cv::rectangle(frame, pt1, pt2, color, rect_line_width, linestyle, 0);

    cv::rectangle(frame, pt_text_bg1, pt_text_bg2, color, rect_line_width, linestyle, 0);
    cv::rectangle(frame, pt_text_bg1, pt_text_bg2, color, cv::FILLED, linestyle, 0);

    static cv::Scalar text_color = CV_RGB(255, 255, 255);

    for (int i = labels.size() - 1; i >= 0; i--) {
        putText(frame, labels[i], pt_text, fontHeight, 1.0f, text_color, thickness = 0, linestyle, true);
        pt_text.y -= (fontHeight + 2);//((labels.size()-1 -i) * fontHeight);
    }
}

//提取特征
bool extract_feature(Mat img,const FaceDetector& FD,const FaceLandmarker& FL,const FaceRecognizer& FR, float* feature)
{
  SeetaImageData simg;
  simg.height = img.rows;
  simg.width = img.cols;
  simg.channels = img.channels();
  simg.data = img.data;

  auto faces = FD.detect(simg);
  if (faces.size <= 0){
    cout << "no face detected" << endl;
    return false;
  }


  SeetaPointF points[5];
  FL.mark(simg, faces.data[0].pos, points);

  FR.Extract(simg, points, feature);
  return true;
}


const char* get_eye_status(EyeStateDetector::EYE_STATE state)
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


const char* get_fas_status(FaceAntiSpoofing::Status status) {
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


int main()
{
  string ModelPath = "sf3.0_models/";

  //1.人脸检测模型初始化
  ModelSetting FD_setting;
  FD_setting.append(ModelPath + "face_detector.csta");
  FD_setting.set_device(ModelSetting::CPU);
  FD_setting.set_id(0);
  FaceDetector FD(FD_setting);

  //2.人脸关键点模型初始化
  ModelSetting PD_setting;
  PD_setting.append(ModelPath + "face_landmarker_pts5.csta");
  FaceLandmarker FL(PD_setting);

  //3.人脸识别模型初始化
  ModelSetting fr_setting;
  fr_setting.append(ModelPath + "face_recognizer.csta");
  FaceRecognizer FR(fr_setting);

  //4.性别检测模型初始化
  ModelSetting gb_setting(ModelPath + "gender_predictor.csta");
  GenderPredictor GP(gb_setting);

  //5.年龄检测模型初始化
  ModelSetting ap_setting(ModelPath + "age_predictor.csta");
  AgePredictor AP(ap_setting);

  //6.眼睛状态模型初始化
  ModelSetting setting;
  setting.append(ModelPath + "eye_state.csta");
  EyeStateDetector EBD(setting);

  //7.活体检测模型初始化
  ModelSetting anti_setting;
  anti_setting.append(ModelPath + "fas_first.csta");
  anti_setting.append(ModelPath + "fas_second.csta");
  FaceAntiSpoofing FAS(anti_setting);
  FAS.SetThreshold(0.3, 0.90);//设置默认阈值，另外一组阈值为(0.7, 0.55)
  FAS.SetBoxThresh(0.9);


  //建立人脸数据库的人脸特征向量：这里只有两张人脸1.jpg(刘德华)，2.jpg(薇娅)
  vector<pair<string, shared_ptr<float> > > feature_db;
  shared_ptr<float> feature1(new float[FR.GetExtractFeatureSize()]);
  Mat ldh = imread("1.jpg");
  extract_feature(ldh,FD, FL, FR,feature1.get());
  feature_db.emplace_back(pair<string, shared_ptr<float>>("刘德华", feature1));

  shared_ptr<float> feature2(new float[FR.GetExtractFeatureSize()]);
  Mat wy = imread("2.jpg");
  extract_feature(wy,FD, FL, FR, feature2.get());
  feature_db.emplace_back(pair<string, shared_ptr<float>>("薇娅", feature2));

  namedWindow("SeetaFaceAntiSpoofing", 0);

  Mat frame;
  VideoCapture capture(0);// "F:/20201204-WY-LDH-cut.mp4");// ");
  VideoWriter writer;
  cv::resize(ldh, ldh, cv::Size(120, 160));
  cv::resize(wy, wy, cv::Size(120, 160));

  if (!capture.isOpened())
  {
    cout << "fail to open!" << endl;
    return -1;
  }


  while (true)
  {
    if (!capture.read(frame)){
      cout << "can not read any frame" << endl;
      break;
    }
    //ImageData image = frame;
    SeetaImageData image;
    image.height = frame.rows;
    image.width = frame.cols;
    image.channels = frame.channels();
    image.data = frame.data;

    auto faces = FD.detect(image);
    cout << "faces.size:" << faces.size << endl;
    for (int i = 0; i < faces.size; i++)
    {
      vector<string> labels;
      Scalar color(0x00, 0xA0, 0x00);
      //----人脸----
      auto face = faces.data[i].pos;


      //----关键点检测----
      vector<SeetaPointF> points(FL.number());
      FL.mark(image, face, points.data());


      //----人脸识别----
      unique_ptr<float[]> feature(new float[FR.GetExtractFeatureSize()]);
      FR.Extract(image, points.data(), feature.get());


      //人脸识别
      float threshold = 0.60;
      int64_t target_index = -1;
      float max_sim = 0;
      for (size_t index = 0; index < feature_db.size(); ++index){
        auto& pair_name_feat = feature_db[index];
        float current_sim = FR.CalculateSimilarity(feature.get(), pair_name_feat.second.get());
        if (current_sim > max_sim){
          max_sim = current_sim;
          target_index = index;
        }
      }
      if (max_sim > threshold) 
        labels.push_back(feature_db[target_index].first+"（相似度:"+ to_string(max_sim *100).substr(0,5) +"）");
      else 
        labels.push_back("查无此人");
    
      //----性别----
      GenderPredictor::GENDER gender;
      GP.PredictGenderWithCrop(image, points.data(), gender);
      string gender_str = (string("性别：") + (gender == GenderPredictor::GENDER::MALE ? "男" : "女"));


      //----年龄----
      int age;
      AP.PredictAgeWithCrop(image, points.data(), age);
      labels.push_back(gender_str + string("，年龄：") + to_string(age));


      //----眼睛状态----
      EyeStateDetector::EYE_STATE leftstate, rightstate;
      EBD.Detect(image, points.data(), leftstate, rightstate);
      labels.push_back(string("左眼：") + get_eye_status(leftstate)+ string("，右眼：") + get_eye_status(rightstate));


      //活体检测
      auto status = FAS.Predict(image, face, points.data());//PredictVideo
      float clarity;
      float reality;

      FAS.GetPreFrameScore(&clarity, &reality);
      labels.push_back(string("活体检测：") + get_fas_status(status));
      if (status == FaceAntiSpoofing::SPOOF)
        color = Scalar(0x00, 0x00, 0xB0);
      drawResult(color, labels, 0, 0.0f, face.x, face.y, face.x + face.width, face.y + face.height, frame);
    }

    Scalar title_color(0x00, 0x8C, 0xFF);
    //绘制人脸库
    int space = 6;
    frame(cv::Rect(frame.cols / 2 - ldh.cols - space / 2 - space, frame.rows - ldh.rows - space -space, ldh.cols * 2+space+ 2*space, ldh.rows + 2 * space)) = title_color;
    ldh.copyTo(frame(cv::Rect(frame.cols / 2 - ldh.cols - space/2,   frame.rows -ldh.rows - space, ldh.cols, ldh.rows)));
    wy.copyTo(frame(cv::Rect(frame.cols / 2 + space / 2,             frame.rows - wy.rows - space, wy.cols, wy.rows)));
    /*
    int baseline = 0;
    int fontHeight = 45;
    cv::Size text_size;
    if (ft2) text_size = ft2->getTextSize("人脸库(2张)", fontHeight, -1, &baseline);
    if (ft2)ft2->putText(frame, "人脸库(2张)", cv::Point(frame.cols/2 - text_size.width/2, frame.rows  - ldh.rows/2), fontHeight, title_color, -1, 16, true);
    
    fontHeight = 60;
    //绘制最上面的title
    string title = "SeetaFace人脸库体验";
    text_size = ft2->getTextSize(title, fontHeight, -1, &baseline);
    ft2->putText(frame, title, cv::Point(frame.cols / 2 - text_size.width / 2, 60), fontHeight, title_color, -1, 16, true);


    fontHeight = 30;
    title = "用到：人脸,关键点,识别,性别,年龄,眼睛,活体；未用到：口罩,追踪,姿态,质量评估";
    text_size = ft2->getTextSize(title, fontHeight, -1, &baseline);
    ft2->putText(frame, title, cv::Point(frame.cols / 2 - text_size.width / 2, 60+50), fontHeight, title_color, -1, 16, true);
    */
    //写入文件
    if (!writer.isOpened()) {
      writer.open("F:/setaface.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, cv::Size(frame.cols, frame.rows), true);
    }
    if (writer.isOpened()) {
      writer.write(frame);
    }

    //窗口显示
    imshow("SeetaFaceAntiSpoofing", frame);

    //Esc键退出
    if (waitKey(1) == 27)
      break;
  }


  writer.release();
  capture.release();
  return 0;
}


