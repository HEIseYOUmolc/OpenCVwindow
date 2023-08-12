#pragma execution_character_set("utf-8")
#include "gifwidget.h"
#define deskGeometry qApp->primaryScreen()->geometry()
#define deskGeometry2 qApp->primaryScreen()->availableGeometry()
//实例化
Opencv gifcv;//opencv分类的函数
cv::VideoWriter gif_writer;
Ui::mainwindow *gui = nullptr;

void ui_ginit(Ui::mainwindow *ui_ptr)
{
    gui = ui_ptr;
}
/*********************************************************************
 * 函数功能：生成截图相关的功能
 * 参 数：无
 * 返回值： 无
 * 类 别：UI界面record_gif按钮
 *********************************************************************/
void mainwindow::on_load_record_gif_clicked()
{
    //设置截图窗口置顶显示
    GifWidget::Instance()->setWindowFlags(GifWidget::Instance()->windowFlags() | Qt::WindowStaysOnTopHint);
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
    GifWidget::Instance()->show();
}
QScopedPointer<GifWidget> GifWidget::self;
/*********************************************************************
 * 函数功能：自锁
 * 参 数：  无
 * 返回值： 无
 * 类 别：GifWidget
 *********************************************************************/
GifWidget *GifWidget::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new GifWidget);
        }
    }

    return self.data();
}
/*********************************************************************
 * 函数功能：初始化控制界面和格式
 * 参 数：  无
 * 返回值： 无
 * 类 别：GifWidget
 *********************************************************************/
GifWidget::GifWidget(QWidget *parent) : QDialog(parent)
{
    this->initControl();
    this->initForm();
}

bool GifWidget::eventFilter(QObject *watched, QEvent *event)
{
    static QPoint mousePoint;
    static bool mousePressed = false;

    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    if (mouseEvent->type() == QEvent::MouseButtonPress) {
        if (mouseEvent->button() == Qt::LeftButton) {
            mousePressed = true;
            mousePoint = mouseEvent->globalPos() - this->pos();
            return true;
        }
    } else if (mouseEvent->type() == QEvent::MouseButtonRelease) {
        mousePressed = false;
        return true;
    } else if (mouseEvent->type() == QEvent::MouseMove) {
        if (mousePressed) {
            this->move(mouseEvent->globalPos() - mousePoint);
            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}
/*********************************************************************
 * 函数功能：更新大小事件------由右下角调整
 * 参 数：  无
 * 返回值： 无
 * 类 别：GifWidget
 *********************************************************************/
void GifWidget::resizeEvent(QResizeEvent *e)
{
    //拉动右下角改变大小自动赋值
    txtWidth->setText(QString::number(widgetMain->width()));
    txtHeight->setText(QString::number(widgetMain->height()));
    QDialog::resizeEvent(e);
}
/*********************************************************************
 * 函数功能：执行绘图操作
 * 参 数：  QPaintEvent *
 * 返回值： 无
 * 类 别：GifWidget const
 *********************************************************************/
void GifWidget::paintEvent(QPaintEvent *)
{
    int width = txtWidth->text().toInt();
    int height = txtHeight->text().toInt();
    rectGif = QRect(borderWidth, widgetTop->height(), width - (borderWidth * 2), height);

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(bgColor);
    painter.drawRoundedRect(this->rect(), 5, 5);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.fillRect(rectGif, Qt::SolidPattern);
}
/*********************************************************************
 * 函数功能：获取边框宽度
 * 参 数：  无
 * 返回值： int
 * 类 别：GifWidget const
 *********************************************************************/
int GifWidget::getBorderWidth() const
{
    return this->borderWidth;
}
/*********************************************************************
 * 函数功能：获取背景颜色
 * 参 数：  无
 * 返回值：QColor
 * 类 别：GifWidget const
 *********************************************************************/
QColor GifWidget::getBgColor() const
{
    return this->bgColor;
}
/*********************************************************************
 * 函数功能：初始化控制界面
 * 参 数：  无
 * 返回值： 无
 * 类 别：GifWidget
 *********************************************************************/
void GifWidget::initControl()
{

    this->setObjectName("GifWidget");
    this->resize(800, 600);                                     //设置大小
    this->setSizeGripEnabled(true);
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSpacing(0);
    verticalLayout->setContentsMargins(11, 11, 11, 11);
    verticalLayout->setObjectName("verticalLayout");
    verticalLayout->setContentsMargins(0, 0, 0, 0);

    widgetTop = new QWidget(this);
    widgetTop->setObjectName("widgetTop");
    widgetTop->setMinimumSize(QSize(0, 35));
    widgetTop->setMaximumSize(QSize(16777215, 35));

    QHBoxLayout *layoutTop = new QHBoxLayout(widgetTop);
    layoutTop->setSpacing(0);
    layoutTop->setContentsMargins(11, 11, 11, 11);
    layoutTop->setObjectName("layoutTop");
    layoutTop->setContentsMargins(0, 0, 0, 0);

    QPushButton *btnIcon = new QPushButton(widgetTop);
    btnIcon->setObjectName("btnIcon");
    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(btnIcon->sizePolicy().hasHeightForWidth());
    btnIcon->setSizePolicy(sizePolicy);
    btnIcon->setMinimumSize(QSize(35, 0));
    btnIcon->setFlat(true);
    layoutTop->addWidget(btnIcon);

    QLabel *labTitle = new QLabel(widgetTop);
    labTitle->setObjectName("labTitle");
    layoutTop->addWidget(labTitle);

    QSpacerItem *horizontalSpacer = new QSpacerItem(87, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layoutTop->addItem(horizontalSpacer);

    //btnClose按钮
    QPushButton *btnClose = new QPushButton(widgetTop);
    btnClose->setObjectName("btnClose");
    sizePolicy.setHeightForWidth(btnClose->sizePolicy().hasHeightForWidth());
    btnClose->setSizePolicy(sizePolicy);
    btnClose->setMinimumSize(QSize(35, 0));
    btnClose->setFocusPolicy(Qt::NoFocus);
    btnClose->setFlat(true);
    layoutTop->addWidget(btnClose);
    verticalLayout->addWidget(widgetTop);
    //主要窗口
    widgetMain = new QWidget(this);
    widgetMain->setObjectName("widgetMain");
    QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(widgetMain->sizePolicy().hasHeightForWidth());
    widgetMain->setSizePolicy(sizePolicy1);
    verticalLayout->addWidget(widgetMain);
    //底部窗口参数组件--上下限设定
    widgetBottom = new QWidget(this);
    widgetBottom->setObjectName("widgetBottom");
    widgetBottom->setMinimumSize(QSize(0, 45));
    widgetBottom->setMaximumSize(QSize(16777215, 45));
    //底部窗口参数组件--水平布局
    QHBoxLayout *layoutBottom = new QHBoxLayout(widgetBottom);
    layoutBottom->setSpacing(6);
    layoutBottom->setContentsMargins(11, 11, 11, 11);
    layoutBottom->setObjectName("layoutBottom");
    layoutBottom->setContentsMargins(9, 9, -1, -1);
    //底部窗口参数组件--FPS组件-》添加至layoutBottom
    QLabel *labFps = new QLabel(widgetBottom);
    labFps->setObjectName("labFps");
    layoutBottom->addWidget(labFps);

    txtFps = new QLineEdit(widgetBottom);
    txtFps->setObjectName("txtFps");
    txtFps->setMaximumSize(QSize(50, 16777215));
    txtFps->setAlignment(Qt::AlignCenter);
    layoutBottom->addWidget(txtFps);
    //底部窗口参数组件--宽度组件-》添加至layoutBottom
    QLabel *labWidth = new QLabel(widgetBottom);
    labWidth->setObjectName("labWidth");
    layoutBottom->addWidget(labWidth);

    txtWidth = new QLineEdit(widgetBottom);
    txtWidth->setObjectName("txtWidth");
    txtWidth->setEnabled(true);
    txtWidth->setMaximumSize(QSize(50, 16777215));
    txtWidth->setAlignment(Qt::AlignCenter);
    layoutBottom->addWidget(txtWidth);
    //底部窗口参数组件--高度组件-》添加至layoutBottom
    QLabel *labHeight = new QLabel(widgetBottom);
    labHeight->setObjectName("labHeight");
    layoutBottom->addWidget(labHeight);

    txtHeight = new QLineEdit(widgetBottom);
    txtHeight->setObjectName("txtHeight");
    txtHeight->setEnabled(true);
    txtHeight->setMaximumSize(QSize(50, 16777215));
    txtHeight->setAlignment(Qt::AlignCenter);
    layoutBottom->addWidget(txtHeight);
    //底部窗口参数组件--格式选择框组件-》添加至layoutBottom
    QLabel *labFormat = new QLabel(widgetBottom);
    labFormat->setObjectName("labFormat");
    layoutBottom->addWidget(labFormat);
    FormatcomBox = new QComboBox(widgetBottom);
    FormatcomBox->setMaximumSize(QSize(50, 16777215));
    FormatcomBox->setObjectName("FormatcomBox");
    FormatcomBox->addItem("mp4");               //创建框中的内容
    FormatcomBox->addItem("flv");
    FormatcomBox->addItem("avi");
    FormatcomBox->addItem("ogv");
    layoutBottom->addWidget(FormatcomBox);

    //底部窗口参数组件--状态组件-》添加至layoutBottom
    labStatus = new QLabel(widgetBottom);
    labStatus->setObjectName("labStatus");
    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(labStatus->sizePolicy().hasHeightForWidth());
    labStatus->setSizePolicy(sizePolicy2);
    labStatus->setAlignment(Qt::AlignCenter);
    layoutBottom->addWidget(labStatus);
    //底部窗口参数组件--开始组件-》添加至layoutBottom
    btnStart = new QPushButton(widgetBottom);
    btnStart->setObjectName("btnStart");
    sizePolicy.setHeightForWidth(btnStart->sizePolicy().hasHeightForWidth());
    btnStart->setSizePolicy(sizePolicy);
    layoutBottom->addWidget(btnStart);
    verticalLayout->addWidget(widgetBottom);
    //底部窗口参数组件--检测组件-》添加至layoutBottom
    btnInf = new QPushButton(widgetBottom);
    btnInf->setObjectName("btnInf");
    sizePolicy.setHeightForWidth(btnStart->sizePolicy().hasHeightForWidth());
    btnInf->setSizePolicy(sizePolicy);
    layoutBottom->addWidget(btnInf);
    verticalLayout->addWidget(widgetBottom);
    //底部窗口参数组件--设置各标签的名称
    labTitle->setText("录屏检测工具");
    labFps->setText("帧率");
    labWidth->setText("宽度");
    labHeight->setText("高度");
    labFormat->setText("存储格式");
    btnStart->setText("录制");
    btnInf->setText("检测");
    this->setWindowTitle(labTitle->text());
    //设置Icon和Close的UI界面
    btnIcon->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    btnClose->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));

    connect(btnClose, SIGNAL(clicked(bool)), this, SLOT(closeAll()));                           //连接关闭按钮框点击 -》关闭所有函数
    connect(btnStart, SIGNAL(clicked(bool)), this, SLOT(record()));                             //连接开始按钮点击  -》录制函数
    connect(btnInf, SIGNAL(clicked(bool)), this, SLOT(inference()));                            //连接检测按钮点击  -》检测函数
    connect(txtWidth, SIGNAL(editingFinished()), this, SLOT(resizeForm()));                     //连接宽度框的改变  -》调整大小函数
    connect(txtHeight, SIGNAL(editingFinished()), this, SLOT(resizeForm()));                    //连接高度框的改变  -》调整大小函数

}
/*********************************************************************
 * 函数功能：初始化格式
 * 参 数：  无
 * 返回值： 无
 * 类 别：GifWidget
 *********************************************************************/
void GifWidget::initForm()
{

    borderWidth = 3;                              //预设边框宽度
    bgColor = QColor(34, 163, 169);               //预设背景颜色

    fps = 10;                                     //预设FPS
    txtFps->setText(QString::number(fps));        //将预设FPS显示

    gifWriter = 0;                                //清空写缓冲

    timer = new QTimer(this);                     //预设定时器
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(saveImage()));                                 //连接定时器    -》调整大小函数

    inftimer = new QTimer(this);                     //预设定时器
    inftimer->setInterval(100);
    connect(inftimer, SIGNAL(timeout()), this, SLOT(saveandinfImage()));                                 //连接定时器    -》调整大小函数

    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->installEventFilter(this);

    QStringList qss;
    qss.append("QLabel{color:#ffffff;}");
    qss.append("#btnClose,#btnIcon{border:none;border-radius:0px;}");
    qss.append("#btnClose:hover{background-color:#ff0000;}");
    qss.append("#btnClose{border-top-right-radius:5px;}");
    qss.append("#labTitle{font:bold 16px;}");
    qss.append("#labStatus{font:15px;}");
    this->setStyleSheet(qss.join(""));
}
/*********************************************************************
 * 函数功能：处理图片保存gif图片
 * 参 数：  无
 * 返回值： 无
 * 类 别：GifWidget
 *********************************************************************/
void GifWidget::saveImage()
{
    if (!gifWriter) {
        return;
    }

    QScreen *screen = QApplication::primaryScreen();
    QPixmap pix = screen->grabWindow(0, x() + rectGif.x(), y() + rectGif.y(), rectGif.width(), rectGif.height());
    QImage image = pix.toImage().convertToFormat(QImage::Format_RGBA8888);

    gif.GifWriteFrame(gifWriter, image.bits(), rectGif.width(), rectGif.height(), fps);
    count++;
    labStatus->setText(QString("正在录制 第 %1 帧").arg(count));
}
/*********************************************************************
 * 函数功能：(自设函数)处理图片检测图片根据选择保存图片
 * 参 数：  无
 * 返回值： 无
 * 类 别：GifWidget
 *********************************************************************/
void GifWidget::saveandinfImage()
{
    QScreen *screen = QApplication::primaryScreen();
    QPixmap pix = screen->grabWindow(0, x() + rectGif.x(), y() + rectGif.y(), rectGif.width(), rectGif.height());
    QImage image = pix.toImage();

    //转CV格式检测
    Mat frame=gifcv.qim2mat(image);
    gifcv.Inference_c(model_name,class_name,frame);
    //保存
    gif_writer<<frame;
    //转QT格式输出
    QImage infimg=gifcv.mat2qim(frame);
    gui->label_inference->setPixmap(QPixmap::fromImage(infimg.scaled(gui->label_inference->size())));

    count++;
    labStatus->setText(QString("正在录制 第 %1 帧").arg(count));
}
/*********************************************************************
 * 函数功能：（自设函数）推理
 * 参 数：  无
 * 返回值： 无
 * 类 别：GifWidget
 *********************************************************************/
void GifWidget::inference()
{
    if (btnInf->text() == "检测")
    {
        //先弹出文件保存对话框
        if(gui->chkSave->checkState()!=false)
        {
            savename = QFileDialog::getSaveFileName(this, "选择保存位置", qApp->applicationDirPath() + "/", "视频(*.mp4)");
            if (savename.isEmpty())
            {
                return;
            }
            int width = txtWidth->text().toInt();
            int height = txtHeight->text().toInt();
            fps = txtFps->text().toInt();
            gif_writer.open(savename.toStdString(), gif_writer.fourcc('m', 'p', '4', 'v'), fps, cv::Size(width, height),true);
        }
        if (!gif_writer.isOpened()) {
            gui->recvEdit->insertPlainText("非保存模式");
        }

        count = 0;
        labStatus->setText("开始检测...");
        btnInf->setText("停止");

        //延时启动
        inftimer->setInterval(1000 / fps);//设置更新时间
        QTimer::singleShot(1000, inftimer, SLOT(start()));
    }
    else//按钮转变为停止时
    {
        //停止保存
        if (gif_writer.isOpened())
        {
            gif_writer.release();
        }
        inftimer->stop();
        //更新UI界面状态
        labStatus->setText(QString("录制完成 共 %1 帧").arg(count));
        btnInf->setText("检测");
        //打开录制的文件
        QDesktopServices::openUrl(QUrl::fromLocalFile(savename));
    }
}
/*********************************************************************
 * 函数功能：记录
 * 参 数：  无
 * 返回值： 无
 * 类 别：GifWidget
 *********************************************************************/
void GifWidget::record()
{
    if (btnStart->text() == "录制")
    {
        if (0 != gifWriter)
        {
            delete gifWriter;
            gifWriter = 0;
        }

        //先弹出文件保存对话框
        //fileName = qApp->applicationDirPath() + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss.gif");//调试使用
        saveName = QFileDialog::getSaveFileName(this, "选择保存位置", qApp->applicationDirPath() + "/", "gif图片(*.gif)");
        if (saveName.isEmpty())
        {
            return;
        }

        int width = txtWidth->text().toInt();
        int height = txtHeight->text().toInt();
        fps = txtFps->text().toInt();

#ifdef Q_OS_WIN
        //windows上需要先转码
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
        QTextCodec *code = QTextCodec::codecForName("utf-8");
#else
        QTextCodec *code = QTextCodec::codecForName("gbk");
#endif
        const char *name = code->fromUnicode(saveName).constData();
#else
        const char *name = fileName.toUtf8().constData();
#endif

        gifWriter = new Gif::GifWriter;
        bool ok = gif.GifBegin(gifWriter, name, width, height, fps);
        if (!ok)
        {
            delete gifWriter;
            gifWriter = 0;
            return;
        }

        count = 0;
        labStatus->setText("开始录制...");
        btnStart->setText("停止");
        //延时启动
        timer->setInterval(1000 / fps);//设置更新时间
        QTimer::singleShot(1000, timer, SLOT(start()));
        //saveImage();
    }
    else//按钮转变为停止时
    {
        //停止保存
        timer->stop();
        gif.GifEnd(gifWriter);

        //重置写入缓冲器
        delete gifWriter;
        gifWriter = 0;
        //更新UI界面状态
        labStatus->setText(QString("录制完成 共 %1 帧").arg(count));
        btnStart->setText("录制");
        //打开录制的文件
        QDesktopServices::openUrl(QUrl::fromLocalFile(saveName));
    }
}
/*********************************************************************
 * 函数功能：清除所有判断写入完毕
 * 参 数：  无
 * 返回值： 无
 * 类 别：GifWidget
 *********************************************************************/
void GifWidget::closeAll()
{
    if (0 != gifWriter) {
        delete gifWriter;
        gifWriter = 0;
    }

    this->close();
}
/*********************************************************************
 * 函数功能：重新设置录制大小
 * 参 数：  无
 * 返回值： 无
 * 类 别：GifWidget
 *********************************************************************/
void GifWidget::resizeForm()
{
    int width = txtWidth->text().toInt();
    int height = txtHeight->text().toInt();
    this->resize(width, height + widgetTop->height() + widgetBottom->height());
}
/*********************************************************************
 * 函数功能：设置边框宽度
 * 参 数：int borderWidth
 * 返回值： 无
 * 类 别：GifWidget
 *********************************************************************/
void GifWidget::setBorderWidth(int borderWidth)
{
    if (this->borderWidth != borderWidth) {
        this->borderWidth = borderWidth;
        this->update();
    }
}
/*********************************************************************
 * 函数功能：设置背景颜色
 * 参 数：const QColor &bgColor
 * 返回值： 无
 * 类 别：GifWidget
 *********************************************************************/
void GifWidget::setBgColor(const QColor &bgColor)
{
    if (this->bgColor != bgColor) {
        this->bgColor = bgColor;
        this->update();
    }
}
