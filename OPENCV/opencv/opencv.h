#ifndef OPENCV_H
#define OPENCV_H

/**
 * opencv相关功能控件
 * 1. 加载模型源控件弹出菜单选择模型。
 * 2. 加载检测源控件弹出菜单选择文件。
 * 3. 加载摄像头控件检测摄像头。
 * 4. 加载桌面源控件检测桌面。
 * 5. 加载录屏源控件检测实时桌面。
 * 6. 暂停控件控制长播放源暂停。
 * 7. 清除检测清空所有检测记录。
 */

//#include <QObject>
//#include <QQuickItem>
//#include <QSharedDataPointer>
//#include <QWidget>
//#include <QDeclarativeItem>
//基础功能调用
#include <QMainWindow>
#include <QDateTimeEdit>
#include <QMessageBox>
#include <opencv2/opencv.hpp>
//ui界面调用
#include "mainwindow.h"
#include "ui_mainwindow.h"
//调用功能
#include "inference.h"

using namespace std;
using namespace cv;

void ui_init(Ui::mainwindow *cui);  //借用mainwindow的ui界面的控件指针初始化
//全局变量
extern std::string model_name;
extern std::string class_name;
class Opencv
{
   // Q_OBJECT

//    QML_ELEMENT
public:
//    Opencv();
//    Opencv(const Opencv &);
//    Opencv &operator=(const Opencv &);
//    ~Opencv();
    Mat qim2mat(QImage & qim);
    QImage mat2qim(Mat & mat);
    Mat Mat_read(QString path);             //此函数用于输入地址OpenCV读取图片，作resize(640,480)
    Mat Inference_c(std::string path,std::string cpath,Mat frame);//此函数用于输入模型地址和原始图，返回检测后的图片
private slots:

private:
    std::clock_t startTime, endTime,totalTime;//用于保存时间，以计算fps
    bool runOnGPU = true;                     //设置默认使用GPU
};

#endif // OPENCV_H
