#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QLabel>
#include <QDateTimeEdit>
#include <QTimer>

#include <QFileDialog>
#include <QMessageBox>
#include <QWheelEvent>

#include <QMimeDatabase>
#include "QSL.h"

extern std::string model_name;
extern std::string class_name;
QT_BEGIN_NAMESPACE
namespace Ui { class mainwindow; }
QT_END_NAMESPACE

class mainwindow : public QMainWindow
{
    Q_OBJECT

public:
    mainwindow(QWidget *parent = nullptr);
    ~mainwindow();


private slots:
//串口程序函数
    //控件函数
    void on_btnSerialCheck_clicked();
    void on_openBt_clicked();

    void on_clearBt_clicked();
    void on_closeBt_clicked();

    void on_sendBt_clicked();
    //自定义函数
    void messlot();
    void setNumOnLabel(QLabel *lbl, QString strS, long num);

    void StatusInit();
    void TimeUpdate();

//OpenCv功能函数，定义于opencv.cpp
    //控件函数
    void on_loadmodel_clicked();
    void on_loadclass_clicked();
    void on_streamloader_clicked();

    void on_inference_clicked();
    void on_origin_clicked();

    void on_clear_all_clicked();
    void on_puased_clicked();

    void on_load_camera_clicked();
    void on_load_record_clicked();
    void on_load_record_gif_clicked();

    //自定义函数
    bool Init_source();
    void play_ori_Timer();
    void play_inf_Timer();

    void on_UrlOpenBT_clicked();

    void on_UrlOpenBT_2_clicked();

    void on_BT_DATABASE_clicked();



    void on_BT_NETW_clicked();

private:
    Ui::mainwindow *ui;
  //串口使用
    QSerialPort c_serial;

    QLabel *ShowTimeLabel;
    QLabel  *lblSendSum;
    QLabel  *lblRecvSum;
    QLabel  *lblSaveSum;
    long  ComSendSum, ComRecvSum,CvSaveSum;

  //opencv使用
    QString filename;           //读取文件名
    QString modelname;          //读取模型名
    QString classname;          //读取类别名
    QString savename;           //保存文件名

    bool  puased_flag=true;

    QMimeDatabase db;
    QMimeType fileinfo;         //完成文件类型判别

    QTimer *ori_timer;
    QTimer *inf_timer;
};
#endif // MAINWINDOW_H


