#include "vlc.h"


/*********************************************************************
 * 函数功能：线程预设
 * 参 数：QObject *parent
 * 返回值： 空
 * 类 别：VlcThread
 *********************************************************************/
VlcThread::VlcThread(QObject *parent) : QThread(parent)
{
    setObjectName("VlcThread");

    stopped = false;
    isPlay = false;

    url = "rtsp://192.168.1.200:554/1";

    vlcInst = NULL;
    vlcMedia = NULL;
    vlcPlayer = NULL;

    static bool isInit = false;
    if (!isInit) {
        isInit = true;
        qDebug() << TIMEMS << "init vlc lib ok" << " version:" << libvlc_get_version();
    }
}
/*********************************************************************
 * 函数功能：运行
 * 参 数：空
 * 返回值： 空
 * 类 别：VlcThread
 *********************************************************************/
void VlcThread::run()
{
    while (!stopped) {
        msleep(1);
    }

    //线程结束后释放资源
    free();
    stopped = false;
    isPlay = false;
    //qDebug() << TIMEMS << "stop vlc1 thread";
}
/*********************************************************************
 * 函数功能：设置视频流地址
 * 参 数：QWidget *parent
 * 返回值： 空
 * 类 别：VlcThread
 *********************************************************************/
void VlcThread::setUrl(const QString &url)
{
    this->url = url;
}
/*********************************************************************
 * 函数功能：设置视频流地址
 * 参 数：QWidget *parent
 * 返回值： 空
 * 类 别：VlcThread
 *********************************************************************/
void VlcThread::setOption(const QString &option)
{
    if (vlcMedia != NULL) {
        QByteArray data = option.toUtf8();
        const char *arg = data.constData();
        libvlc_media_add_option(vlcMedia, arg);
    }
}
/*********************************************************************
 * 函数功能：设置初始化视频对象
 * 参 数：QWidget *parent
 * 返回值： 空
 * 类 别：VlcThread
 *********************************************************************/
bool VlcThread::init()
{
    const char *tempArg = "";
    const char *vlc_args[9] = {"-I", "dummy", "--no-osd", "--no-stats", "--ignore-config", "--no-video-on-top", "--no-video-title-show", "--no-snapshot-preview", tempArg};
    vlcInst = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);//设置初始化参数
    if (vlcInst == NULL) {
        return false;
    }

    vlcMedia = libvlc_media_new_location(vlcInst, url.toUtf8().constData());
    vlcPlayer = libvlc_media_player_new_from_media(vlcMedia);
    if (vlcPlayer == NULL) {
        return false;
    }

    //设置播放句柄
    VlcWidget *w = (VlcWidget *)this->parent();
#if defined(Q_OS_WIN)
    libvlc_media_player_set_hwnd(vlcPlayer, (void *)w->winId());
#elif defined(Q_OS_LINUX)
    libvlc_media_player_set_xwindow(vlcPlayer, w->winId());
#elif defined(Q_OS_MAC)
    libvlc_media_player_set_nsobject(vlcPlayer, (void *)w->winId());
#endif

    //设置硬件加速 none auto any d3d11va dxva2
    setOption(QString(":avcodec-hw=%1").arg("none"));
    //设置通信协议 tcp udp
    setOption(QString(":rtsp-%1").arg("tcp"));
    //设置缓存时间 默认500毫秒
    setOption(QString(":network-caching=%1").arg(300));

    libvlc_media_player_play(vlcPlayer);
    qDebug() << TIMEMS << "init vlc finsh";
    return true;
}
/*********************************************************************
 * 函数功能：播放视频对象
 * 参 数：空
 * 返回值： 空
 * 类 别：VlcThread
 *********************************************************************/
void VlcThread::play()
{
    isPlay = true;
    this->init();
}
/*********************************************************************
 * 函数功能： 暂停播放
 * 参 数：空
 * 返回值： 空
 * 类 别：VlcThread
 *********************************************************************/
void VlcThread::pause()
{
    if (vlcPlayer != NULL) {
        libvlc_media_player_pause(vlcPlayer);
    }    
}
/*********************************************************************
 * 函数功能：继续播放
 * 参 数：空
 * 返回值： 空
 * 类 别：VlcThread
 *********************************************************************/
void VlcThread::next()
{
    if (vlcPlayer != NULL) {
        libvlc_media_player_pause(vlcPlayer);
    }
}
/*********************************************************************
 * 函数功能：释放对象
 * 参 数：空
 * 返回值： 空
 * 类 别：VlcThread
 *********************************************************************/
void VlcThread::free()
{
    if (vlcInst != NULL) {
        libvlc_release(vlcInst);
        vlcInst = NULL;
    }

    if (vlcMedia != NULL) {
        libvlc_media_release(vlcMedia);
        vlcMedia = NULL;
    }

    if (vlcPlayer != NULL) {
        libvlc_media_player_release(vlcPlayer);
        vlcPlayer = NULL;
    }

    //qDebug() << TIMEMS << "close vlc ok";
}
/*********************************************************************
 * 函数功能：停止采集线程
 * 参 数：空
 * 返回值： 空
 * 类 别：VlcThread
 *********************************************************************/
void VlcThread::stop()
{
    stopped = true;
}
/*********************************************************************
 * 函数功能：实时视频显示窗体类
 * 参 数：空
 * 返回值： 空
 * 类 别：VlcThread
 *********************************************************************/
VlcWidget::VlcWidget(QWidget *parent) : QWidget(parent)
{
    thread = new VlcThread(this);
}
/*********************************************************************
 * 函数功能：设定QT界面
 * 参 数：空
 * 返回值： 空
 * 类 别：VlcThread
 *********************************************************************/
VlcWidget::~VlcWidget()
{
    close();
}
/*********************************************************************
 * 函数功能：设定QT界面
 * 参 数：空
 * 返回值： 空
 * 类 别：VlcThread
 *********************************************************************/
void VlcWidget::setUrl(const QString &url)
{
    thread->setUrl(url);
}
/*********************************************************************
 * 函数功能：设定QT界面
 * 参 数：空
 * 返回值： 空
 * 类 别：VlcThread
 *********************************************************************/
void VlcWidget::open()
{
    //qDebug() << TIMEMS << "1open video" << objectName();
    clear();

    thread->play();
    thread->start();
}
/*********************************************************************
 * 函数功能：暂停
 * 参 数：空
 * 返回值： 空
 * 类 别：VlcThread
 *********************************************************************/
void VlcWidget::pause()
{
    thread->pause();
}
/*********************************************************************
 * 函数功能：设定QT界面
 * 参 数：QWidget *parent
 * 返回值： 空
 * 类 别：VlcThread
 *********************************************************************/
void VlcWidget::next()
{
    thread->next();
}
/*********************************************************************
 * 函数功能：设定QT界面
 * 参 数：QWidget *parent
 * 返回值： 空
 * 类 别：VlcThread
 *********************************************************************/
void VlcWidget::close()
{
    //qDebug() << TIMEMS << "close video" << objectName();
    if (thread->isRunning()) {
        thread->stop();
        thread->quit();
        thread->wait(3000);
    }

    QTimer::singleShot(5, this, SLOT(clear()));
}
/*********************************************************************
 * 函数功能：设定QT界面
 * 参 数：QWidget *parent
 * 返回值： 空
 * 类 别：VlcThread
 *********************************************************************/
void VlcWidget::restart()
{
    //qDebug() << TIMEMS << "restart video" << objectName();
    close();
    QTimer::singleShot(10, this, SLOT(open()));
}
/*********************************************************************
 * 函数功能：清除
 * 参 数：QWidget *parent
 * 返回值： 空
 * 类 别：VlcThread
 *********************************************************************/
void VlcWidget::clear()
{
    update();
}
/*********************************************************************
 * 函数功能：返回读取图像信息
 * 参 数：QWidget *parent
 * 返回值： 空
 * 类 别：VlcThread
 *********************************************************************/
 void VlcWidget::test()
{
    //qDebug() << TIMEMS << "测试函数成功" << objectName();

//    QBuffer buffer(VlcFrame->plane);

//    buffer.open(QIODevice::ReadOnly);
//    QImageReader reader(&buffer,"JPG");
//    QImage img = reader.read();
//    if(!img.isNull()){
//        qDebug() << TIMEMS << "图像存在";
//    }
//    else{
//        qDebug() << TIMEMS << "图像为空";
//    }
    //thread->receiveImage(img)
}

