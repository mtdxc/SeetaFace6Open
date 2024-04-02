
#include <iostream>
#include <opencv2/opencv.hpp>
//用到seetaface的7个模块
#include "FaceEngine2.h"

#pragma comment(lib, "opencv_world490.lib")
#pragma comment(lib, "opencv_img_hash490.lib")

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

int main()
{

  FaceEngine2 engine;
  engine.init("sf3.0_models/", "face_recognizer.csta");

  //建立人脸数据库的人脸特征向量：这里只有两张人脸1.jpg(刘德华)，2.jpg(薇娅)
  Mat ldh = imread("1.jpg");
  engine.addFaceDb("刘德华", ldh.data, ldh.cols, ldh.rows);

  Mat wy = imread("2.jpg");
  engine.addFaceDb("薇娅", wy.data, wy.cols, wy.rows);

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

    engine.updateRgb(frame.data, frame.cols, frame.rows);
    cout << "faces.size:" << engine.face_size() << endl;
    for (int i = 0; i < engine.face_size(); i++)
    {
      vector<string> labels;
      Scalar color(0x00, 0xA0, 0x00);
      //----人脸----
      auto face = engine.get(i);

      if (face->name.length()) 
        labels.push_back(face->name +"（相似度:"+ to_string(face->name_score *100).substr(0,5) +"）");
      else 
        labels.push_back("查无此人");
    
      //----性别----
      int gender;
      engine.getGender(i, gender);
      string gender_str = (string("性别：") + (gender == GenderPredictor::GENDER::MALE ? "男" : "女"));


      //----年龄----
      int age;
      engine.getAge(i, age);
      labels.push_back(gender_str + string("，年龄：") + to_string(age));


      //----眼睛状态----
      int leftstate, rightstate;
      engine.getEyeStat(i, leftstate, rightstate);
      labels.push_back(string("左眼：") + engine.get_eye_status(leftstate)+ string("，右眼：") + engine.get_eye_status(rightstate));


      //活体检测
      AntiSpoofing anti;
      engine.getAnti(i, anti);
      labels.push_back(string("活体检测：") + engine.get_fas_status(anti.status));
      if (anti.status == FaceAntiSpoofing::SPOOF)
        color = Scalar(0x00, 0x00, 0xB0);
      drawResult(color, labels, 0, 0.0f, face->pos.x, face->pos.y, face->pos.x + face->pos.width, face->pos.y + face->pos.height, frame);
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


