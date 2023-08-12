#include "opencv.h"

//实例化
Ui::mainwindow *ui = nullptr;
VideoCapture cap;
Opencv opencv;//opencv分类的函数
VideoWriter cv_writer;

/*********************************************************************
 * 函数功能：（自设函数）Qimage格式转换为Mat
 * 参 数：  无
 * 返回值： 无
 * 备  注： 格式转换，用于Opencv处理
 *********************************************************************/
Mat Opencv::qim2mat(QImage & qim)
{
    qim = qim.convertToFormat(QImage::Format_RGB888);
    Mat mat = Mat(qim.height(), qim.width(),
                  CV_8UC3,(void*)qim.constBits(),qim.bytesPerLine());
    cvtColor(mat, mat, COLOR_RGB2BGR);
    return mat;
}
/*********************************************************************
 * 函数功能：（自设函数）Mat格式转换为Qimage
 * 参 数：  无
 * 返回值： 无
 * 备  注： 格式转换，用于ui界面显示
 *********************************************************************/
QImage Opencv::mat2qim(Mat & mat)
{
    cvtColor(mat, mat, COLOR_BGR2RGB);
    QImage qim((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step,
               QImage::Format_RGB888);
    return qim;
}
/*********************************************************************
 * 函数功能：（自设函数）界面函数初始化
 * 参 数：  无
 * 返回值： 无
 * 备  注： 非常重要的函数，因为此函数才可以调用ui界面
 *********************************************************************/
void ui_init(Ui::mainwindow *cui)
{
    ui = cui;
}
/*********************************************************************
 * 函数功能：（自设函数）检测初始化源是否成功
 * 参 数：  无
 * 返回值： bool：true | false
 * 类 别：openCV
 *********************************************************************/
bool mainwindow::Init_source()
{
    if(modelname.isEmpty())
    {
        QMessageBox::information(this,"提示","模型打开失败!");
        QMessageBox::information(this,"提示","请重新设置!");
        mainwindow::on_loadmodel_clicked();
    }
    if(filename.isEmpty())
    {
        QMessageBox::information(this,"提示","播放源打开失败!");
        QMessageBox::information(this,"提示","请重新设置!");
        mainwindow::on_streamloader_clicked();
    }

    if(classname.isEmpty())
    {
        QMessageBox::information(this,"提示","类型设置失败!");
        QMessageBox::information(this,"提示","请重新设置!");
        mainwindow::on_loadclass_clicked();
    }
    return true;
}
/*********************************************************************
 * 函数功能：（自设函数）读取设定路径的图片
 * 参 数：QString path
 * 返回值： Mat
 * 类 别：openCV
 *********************************************************************/
Mat Opencv::Mat_read(QString path)
{
    Mat frame;
    frame = cv::imread(path.toLocal8Bit().data());
    if ( frame.empty() )
    {
        /*return void*/;
    }
    return frame;
}

/*********************************************************************
 * 函数功能：（自设函数）通过设定的路径推理得出推断后的图片
 * 参 数：QString path,Mat frame
 * 返回值： Mat
 * 类 别：openCV
 *********************************************************************/
Mat Opencv::Inference_c(std::string path,std::string cpath,Mat frame)
{
    QString detection_info,detectionS;
    //初始化模型、分类、是否应用GPU
    //判断是否用GPU
    if(ui->chkGPU->checkState() != false)
    {runOnGPU=true;}
    else{runOnGPU=false;}

    static Inference inf(path, cv::Size(640, 480), cpath, runOnGPU);// 静态变量保证后续调用时，它将被复用而不会再次初始化。

    // Inference starts here...
    startTime = clock();
    std::vector<Detection> output = inf.runInference(frame);
    endTime = clock();
    // Inference ends here...

    int detections = output.size();
    //编辑框输出信息
    detectionS= QString("%1").arg(detections);
    detection_info= "Number of detections:"+detectionS;//显示检测到的数量
    //如果时间戳打开

    if(ui->chkTimRecv->checkState() != false)
    {
        //需要添加时间时才获取时间
        QString StrTimeDate;
        //获取串口数据接收的系统时间，备后续使用
        QDateTime curDateTime=QDateTime::currentDateTime();
        StrTimeDate = curDateTime.toString("[yyyy-MM-dd hh:mm:ss.zzz] ");
        detection_info.prepend(StrTimeDate);//前面添加时间
    }
    ui->recvEdit->insertPlainText("Inf："+detection_info+"\n");
    //在图中标注
    for (int i = 0; i < detections; ++i)
    {
        Detection detection = output[i];

        cv::Rect box = detection.box;
        cv::Scalar color = detection.color;

        // Detection box
        cv::rectangle(frame, box, color, 2);

        // Detection box text
        std::string classString = detection.className + ' ' + std::to_string(detection.confidence).substr(0, 4);
        ui->recvEdit->insertPlainText(QString::fromStdString(classString+'\n'));//输出在文本框
        cv::Size textSize = cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 1, 2, 0);
        cv::Rect textBox(box.x, box.y - 40, textSize.width + 10, textSize.height + 20);

        cv::rectangle(frame, textBox, color, cv::FILLED);
        cv::putText(frame, classString, cv::Point(box.x + 5, box.y - 10), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 0), 2, 0);
    }
    //添加FPS
    if(ui->chkInfFPS->checkState() != false)
    {
        std::string FPS = "FPS:"+ std::to_string(1000/(endTime - startTime));                                 //字符FPS: 加上帧数转字符
        cv::putText(frame, FPS,cv::Point (10,25), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255, 0, 0), 2, 0);
    }

    if(ui->chkInfTime->checkState() != false)
    {
       // totalTime = clock();
       std::string inftime="Inf_time:"+std::to_string(endTime - startTime)+"ms"+'\n';
       ui->recvEdit->insertPlainText(QString::fromStdString(inftime));//输出在文本框
       //std::string totaltime="Total_time:"+std::to_string(totalTime - startTime)+"ms"+'\n';
       //ui->recvEdit->insertPlainText(QString::fromStdString(totaltime));//输出在文本框
    }
    return frame;
}
/*********************************************************************
 * 函数功能：从文件加载模型，存储模型路径在mainwindow.h定义的modelname
 * 参 数：  无
 * 返回值： 无
 * 类 别：UI界面loadmodel按钮
 *********************************************************************/
void mainwindow::on_loadmodel_clicked()
{
    modelname=QFileDialog::getOpenFileName(this,"选择模型文件","C:/Users/lc/Documents/OpenCVwindow/resource",
                                             "Model File(*.onnx;);;All files(*.*)");
    if(modelname.isEmpty())
    {
        QMessageBox::information(this,"提示","模型打开失败!");
        return;
    }
    //信息处理并显示
    ui->recvEdit->insertPlainText("设置模型路径:"+modelname+"\n");
    model_name=modelname.toStdString();
    //std::cout << "\n" << model<<std::endl;
}
/*********************************************************************
 * 函数功能：从文件加载类别，存储模型路径在mainwindow.h定义的classname
 * 参 数：  无
 * 返回值： 无
 * 类 别：UI界面loadclass按钮
 *********************************************************************/
void mainwindow::on_loadclass_clicked()
{
    classname=QFileDialog::getOpenFileName(this,"选择类别文件","C:/Users/lc/Documents/OpenCVwindow/resource",
                                             "Class File(*.txt;);;All files(*.*)");
    if(classname.isEmpty())
    {
        QMessageBox::information(this,"提示","类别打开失败!");
        return;
    }
    //信息处理并显示
    ui->recvEdit->insertPlainText("设置类别路径:"+classname+"\n");
    class_name=classname.toStdString();
    //打开文件输出文件内类型
    ui->recvEdit->insertPlainText("设置检测类别:\n");
    std::ifstream inputFile(class_name);
    if (inputFile.is_open())
    {
      std::string classLine;
      while (std::getline(inputFile, classLine))
           ui->recvEdit->insertPlainText(QString::fromStdString(classLine)+"\n");
      inputFile.close();
    }
}
/*********************************************************************
 * 函数功能：从文件设定播放源设定，存储播放源路径在mainwindow.h定义的filename
 * 参 数：  无
 * 返回值： 无
 * 类 别：UI界面streamloader按钮
 *********************************************************************/
void mainwindow::on_streamloader_clicked()
{
    filename=QFileDialog::getOpenFileName(this,"打开图像文件","C:/Users/lc/Documents/OpenCVwindow/resource",
    "Image File(*.bmp;*.png;*.jpg;);;Video File(*.mp4;*.avi;*.rmvb;*.mkv*;*.flv;*.rmvb);;Text File(*.txt;*.doc;*.docx);;All files(*.*)");

    if(filename.isEmpty())
    {
        QMessageBox::information(this,"提示","文件打开失败!");
        return;
    }
    //信息处理并显示
    ui->recvEdit->insertPlainText("设置文件路径:"+filename+"\n");
    fileinfo = db.mimeTypeForFile(filename);//获取文件信息
    if(fileinfo.name().startsWith("video/"))
    {
        cap.open(filename.toLocal8Bit().data());
    }
}
/*********************************************************************
 * 函数功能：（自设函数）播放原始源显示函数
 * 参 数：  无
 * 返回值： 无
 * 类 别：opencv
 *********************************************************************/
void mainwindow::play_ori_Timer()
{
    Mat origin;//读取视频
    cap >> origin;
    //转格式，显示
    QImage oriimg =opencv.mat2qim(origin);
    ui->label_origin->setPixmap(QPixmap::fromImage(oriimg.scaled(ui->label_origin->size())));

}
/*********************************************************************
 * 函数功能：（自设函数）播放推理源显示函数
 * 参 数：  无
 * 返回值： 无
 * 类 别：opencv
 *********************************************************************/
void mainwindow::play_inf_Timer()
{
    Mat frame;//读取视频
    cap >> frame;
    // Inference starts here...
    opencv.Inference_c(model_name,class_name,frame);
    if(cv_writer.isOpened())//设定保存模式
    {
        cv_writer<<frame;
    }
    //转格式，显示
    QImage infimg =opencv.mat2qim(frame); 
    ui->label_inference->setPixmap(QPixmap::fromImage(infimg.scaled(ui->label_inference->size())));
    // Inference ends here...
}
/*********************************************************************
 * 函数功能：驱动原始源显示功能
 * 参 数：  无
 * 返回值： 无
 * 类 别：UI界面origin按钮
 *********************************************************************/
void mainwindow::on_origin_clicked()
{
    if(filename.isEmpty())
    {
        if(cap.isOpened())
        {
            ori_timer->start(33);
            return;
        }
        else
        {
            QMessageBox::information(this,"提示","播放源未设置!");
            QMessageBox::information(this,"提示","请重新设置!");
            mainwindow::on_streamloader_clicked();
        }
    }
    //播放
    if(fileinfo.name().startsWith("image/"))
    {
        Mat frame=opencv.Mat_read(filename);        //读取图片
        //转格式，显示
        QImage infimg =opencv.mat2qim(frame);
        ui->label_origin->setPixmap(QPixmap::fromImage(infimg.scaled(ui->label_inference->size())));
    }
    else if(fileinfo.name().startsWith("video/"))
    {
        if(cap.isOpened())
        {
            ori_timer->start(33);
        }
    }
    else if(fileinfo.name().startsWith("audio/"))
    {
        QMessageBox::information(this,"提示","这是一个音频,不支持该类型识别");
    }
    else if(fileinfo.name().startsWith("text/"))
    {
        QMessageBox::information(this,"提示","文件类型为文本;若为类型设定文件，请选择类别选择按钮重新打开");
    }
}
/*********************************************************************
 * 函数功能：驱动检测源显示inference功能
 * 参 数：  无
 * 返回值： 无
 * 备  注： UI界面inference按钮
 *********************************************************************/
void mainwindow::on_inference_clicked()
{
    //判定预设
    Init_source();
    //文件类型判别
    if(fileinfo.name().startsWith("image/"))
    {
        Mat frame=opencv.Mat_read(filename);
        // Inference starts here...
        opencv.Inference_c(model_name,class_name,frame);

        //保存
        if(ui->chkSave->checkState()!=false)
        {
            //数据流统计
            CvSaveSum++;
            setNumOnLabel(lblSaveSum, "Save:", CvSaveSum);
            //设置保存参数
            savename= QString("%1/%2_image_capture.png").arg(qApp->applicationDirPath()).arg(CvSaveSum);
            imwrite(savename.toStdString(), frame);
        }
        //转格式，显示
        QImage infimg = opencv.mat2qim(frame);
        ui->label_inference->setPixmap(QPixmap::fromImage(infimg.scaled(ui->label_inference->size())));
        // Inference ends here...
    }
    else if(fileinfo.name().startsWith("video/"))
    {
        if(cap.isOpened())
        {
            if(ui->chkSave->checkState()!=false)
            {
                //数据流统计
                CvSaveSum++;
                setNumOnLabel(lblSaveSum, "Save:", CvSaveSum);
                //设置保存参数
                savename= QString("%1/%2_video_capture.mp4").arg(qApp->applicationDirPath()).arg(CvSaveSum);
                int fps=10;
                cv_writer.open(savename.toStdString(), cv_writer.fourcc('m', 'p', '4', 'v'), fps, cv::Size(cap.get(CAP_PROP_FRAME_WIDTH),cap.get(CAP_PROP_FRAME_HEIGHT)),true);
            }
            inf_timer->start(33);
        }

    }
    else if(fileinfo.name().startsWith("audio/"))
    {
        QMessageBox::information(this,"提示","这是一个音频,不支持该类型识别");
    }
    else if(fileinfo.name().startsWith("text/"))
    {
        QMessageBox::information(this,"提示","文件类型为文本;若为类型设定文件，请选择类别选择按钮重新打开");
    }
}
/*********************************************************************
 * 函数功能：加载摄像头源
 * 参 数：  无
 * 返回值： 无
 * 类 别：UI界面camera按钮
 *********************************************************************/
void mainwindow::on_load_camera_clicked()
{
    //预设条件判定
    if(modelname.isEmpty())
    {
        QMessageBox::information(this,"提示","模型打开失败!");
        QMessageBox::information(this,"提示","请重新设置!");
        mainwindow::on_loadmodel_clicked();
    }
    if(classname.isEmpty())
    {
        QMessageBox::information(this,"提示","类型设置失败!");
        QMessageBox::information(this,"提示","请重新设置!");
        mainwindow::on_loadclass_clicked();
    }
    cap.open(0);

    if(cap.isOpened())
    {
        if(ui->chkSave->checkState()!=false)
        {
            //数据流统计
            CvSaveSum++;
            setNumOnLabel(lblSaveSum, "Save:", CvSaveSum);
            //设置保存参数
            //设置保存参数
            savename= QString("%1/%2_camera_capture.mp4").arg(qApp->applicationDirPath()).arg(CvSaveSum);
            int fps=10;
            cv_writer.open(savename.toStdString(), cv_writer.fourcc('m', 'p', '4', 'v'), fps, cv::Size(cap.get(CAP_PROP_FRAME_WIDTH),cap.get(CAP_PROP_FRAME_HEIGHT)),true);
        }
        inf_timer->start(33);
    }
    else
    {
        QMessageBox::information(this,"提示","摄像头播放源未设置!");
        QMessageBox::information(this,"提示","请重新设置!");
    }
}
/*********************************************************************
 * 函数功能：清除所有
 * 参 数：  无
 * 返回值： 无
 * 备  注： UI界面clear_all按钮
 *********************************************************************/
void mainwindow::on_clear_all_clicked()
{
    ui->label_origin->clear();//关闭图像
    ui->label_inference->clear();//关闭图像
    ui->recvEdit->clear();
    ori_timer->stop();
    inf_timer->stop();
    cap.release();
    cv_writer.release();
    //重置模型和类型设定
    model_name.clear();
    class_name.clear();
    modelname.clear();
    classname.clear();
    if(ui->chkSave->checkState()!=false)
    {QDesktopServices::openUrl(QUrl::fromLocalFile(savename));}
    filename.clear();
}
/*********************************************************************
 * 函数功能：暂停
 * 参 数：  无
 * 返回值： 无
 * 备  注： UI界面puased按钮
 *********************************************************************/
void mainwindow::on_puased_clicked()
{
    if(puased_flag==true)
    {   ori_timer->stop();
        inf_timer->stop();
        puased_flag=false;
    }
    else if(ui->playWidget->isVisible())
    {
        ui->playWidget->pause();
    }
    else
    {
        ori_timer->start(33);
        inf_timer->start(33);
        puased_flag=true;
    }
}

/*********************************************************************
 * 函数功能：使用vlc-qt加载url
 * 参 数：  无
 * 返回值： 无
 * 类 别：UI界面UrlOpenBT按钮
 *********************************************************************/
void mainwindow::on_UrlOpenBT_clicked()
{

    if (ui->UrlOpenBT->text() == "vlc打开URL")
    {
        ui->UrlOpenBT->setText("关闭URL");
        QString url = ui->cboxUrl->currentText().trimmed();
        ui->playWidget->setUrl(url);
        ui->playWidget->open();
        ui->playWidget->show();
        ui->label_origin->hide();    
    }
    else
    {
        ui->UrlOpenBT->setText("vlc打开URL");
        ui->playWidget->close();
        ui->playWidget->hide();
        ui->label_origin->show();
    }
}
void mainwindow::on_UrlOpenBT_2_clicked()
{
    if (ui->UrlOpenBT_2->text() == "FFmpeg")
    {
        ui->UrlOpenBT_2->setText("关闭URL");
        QString url = ui->cboxUrl->currentText().trimmed();
        ui->playWidget_2->setUrl(url);
        ui->playWidget_2->open();
        ui->playWidget_2->show();
        ui->label_origin->hide();
    }
    else
    {
        ui->UrlOpenBT_2->setText("FFmpeg");
        ui->playWidget_2->close();
        ui->playWidget_2->hide();
        ui->label_origin->show();
    }
}
