﻿#include "ffmpeg.h"

/*********************************************************************
 * 函数功能：FFmpeg线程预设
 * 参 数：QObject *parent
 * 返回值： 空
 * 类 别：FFmpegThread
 *********************************************************************/
FFmpegThread::FFmpegThread(QObject *parent) : QThread(parent)
{
    setObjectName("FFmpegThread");
    stopped = false;
    isPlay = false;

    frameFinish = false;
    videoWidth = 0;
    videoHeight = 0;
    oldWidth = 0;
    oldHeight = 0;
    videoStreamIndex = -1;
    audioStreamIndex = -1;

    url = "rtsp://192.168.1.128:554/1";

    buffer = NULL;
    avPacket = NULL;
    avFrame = NULL;
    avFrame2 = NULL;
    avFrame3 = NULL;
    avFormatContext = NULL;
    videoCodec = NULL;
    audioCodec = NULL;
    swsContext = NULL;

    options = NULL;
    videoDecoder = NULL;
    audioDecoder = NULL;

    //初始化注册,一个软件中只注册一次即可
    FFmpegThread::initlib();
}
/*********************************************************************
 * 函数功能：初始化FFmpeg 的lib依赖
 * 参 数：空
 * 返回值： 空
 * 类 别：FFmpegThread
 *********************************************************************/
//一个软件中只需要初始化一次就行
void FFmpegThread::initlib()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    static bool isInit = false;
    if (!isInit) {
        //注册库中所有可用的文件格式和解码器
        //av_register_all();已弃用
        //注册所有设备,主要用于本地摄像机播放支持
#ifdef ffmpegdevice
        avdevice_register_all();
#endif
        //初始化网络流格式,使用网络流时必须先执行
        avformat_network_init();

        isInit = true;
        qDebug() << TIMEMS << "init ffmpeg lib ok" << " version:" << FFMPEG_VERSION;
#if 0
        //输出所有支持的解码器名称
        QStringList listCodeName;
        AVCodec *code = av_codec_next(NULL);
        while (code != NULL) {
            listCodeName << code->name;
            code = code->next;
        }

        qDebug() << TIMEMS << listCodeName;
#endif
    }
}
/*********************************************************************
 * 函数功能：初始化FFmpeg
 * 参 数：空
 * 返回值： 空
 * 类 别：FFmpegThread
 *********************************************************************/
//一个软件中只需要初始化一次就行
bool FFmpegThread::init()
{
    //在打开码流前指定各种参数比如:探测时间/超时时间/最大延时等
    //设置缓存大小,1080p可将值调大
    av_dict_set(&options, "buffer_size", "8192000", 0);
    //以tcp方式打开,如果以udp方式打开将tcp替换为udp
    av_dict_set(&options, "rtsp_transport", "tcp", 0);
    //设置超时断开连接时间,单位微秒,3000000表示3秒
    av_dict_set(&options, "stimeout", "3000000", 0);
    //设置最大时延,单位微秒,1000000表示1秒
    av_dict_set(&options, "max_delay", "1000000", 0);
    //自动开启线程数
    av_dict_set(&options, "threads", "auto", 0);

    //打开视频流
    avFormatContext = avformat_alloc_context();

    int result = avformat_open_input(&avFormatContext, url.toStdString().data(), NULL, &options);
    if (result < 0) {
        qDebug() << TIMEMS << "open input error" << url;
        return false;
    }
    //buffer=nullptr;
    //释放设置参数
    if (options != NULL) {
        av_dict_free(&options);
    }

    //获取流信息
    result = avformat_find_stream_info(avFormatContext, NULL);
    if (result < 0) {
        qDebug() << TIMEMS << "find stream info error";
        return false;
    }

    //----------视频流部分开始,打个标记方便折叠代码----------
    if (1) {
        videoStreamIndex = av_find_best_stream(avFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);//设置为空解码器
        if (videoStreamIndex < 0) {
            qDebug() << TIMEMS << "find video stream index error";
            return false;
        }

        //获取视频流
        AVStream *videoStream = avFormatContext->streams[videoStreamIndex];

        //获取视频流解码器,或者指定解码器
        videoPara=videoStream->codecpar;                                    //原codec被舍弃替换为codecpar
        videoCodec = avcodec_alloc_context3(nullptr);

        if ( avcodec_parameters_to_context(videoCodec, videoPara)  < 0) { //转换,debug此步执行完成后---崩溃
            // 创建失败，处理错误
            return 0;
        }
        videoDecoder =(AVCodec*)avcodec_find_decoder(videoCodec->codec_id); //默认类型

        //videoDecoder = avcodec_find_decoder_by_name("h264_qsv");
        if (videoDecoder == NULL) {
            qDebug() << TIMEMS << "video decoder not found";
            return false;
        }

        //设置加速解码
        videoCodec->lowres = videoDecoder->max_lowres;
        videoCodec->flags2 |= AV_CODEC_FLAG2_FAST;

        //打开视频解码器
        result = avcodec_open2(videoCodec, videoDecoder, NULL);
        if (result < 0) {
            qDebug() << TIMEMS << "open video codec error";
            return false;
        }

        //获取分辨率大小
        videoWidth = videoCodec->width;
        videoHeight = videoCodec->height;

        //如果没有获取到宽高则返回
        if (videoWidth == 0 || videoHeight == 0) {
            qDebug() << TIMEMS << "find width height error";
            return false;
        }

        QString videoInfo = QString("视频流信息 -> 索引: %1  解码: %2  格式: %3  时长: %4 秒  分辨率: %5*%6")
                            .arg(videoStreamIndex).arg(videoDecoder->name).arg(avFormatContext->iformat->name)
                            .arg((avFormatContext->duration) / 1000000).arg(videoWidth).arg(videoHeight);
        qDebug() << TIMEMS << videoInfo;
    }
    //----------视频流部分开始----------

    //----------音频流部分开始,打个标记方便折叠代码----------
    if (1) {
        //循环查找音频流索引
        audioStreamIndex = -1;
        for (uint i = 0; i < avFormatContext->nb_streams; i++) {
            if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {//原codec被舍弃替换为codecpar
                audioStreamIndex = i;
                break;
            }
        }
        //有些没有音频流,所以这里不用返回
        if (audioStreamIndex == -1) {
            qDebug() << TIMEMS << "find audio stream index error";
        } else {
            //获取音频流
            AVStream *audioStream = avFormatContext->streams[audioStreamIndex];
            audioPara = audioStream->codecpar;
            audioCodec=avcodec_alloc_context3(nullptr);

            if ( avcodec_parameters_to_context(audioCodec,audioPara) < 0) { //转换
                // 创建失败，处理错误
                return 0;
            }
            //获取音频流解码器,或者指定解码器
            audioDecoder = (AVCodec*)avcodec_find_decoder(audioCodec->codec_id);
            //audioDecoder = avcodec_find_decoder_by_name("aac");
            if (audioDecoder == NULL) {
                qDebug() << TIMEMS << "audio codec not found";
                return false;
            }

            //打开音频解码器
            result = avcodec_open2(audioCodec, audioDecoder, NULL);
            if (result < 0) {
                qDebug() << TIMEMS << "open audio codec error";
                return false;
            }

            QString audioInfo = QString("音频流信息 -> 索引: %1  解码: %2  比特率: %3  声道数: %4  采样: %5")
                                .arg(audioStreamIndex).arg(audioDecoder->name).arg(avFormatContext->bit_rate)
                                    .arg(audioCodec->ch_layout.nb_channels).arg(audioCodec->sample_rate);
            qDebug() << TIMEMS << audioInfo;
        }
    }
    //----------音频流部分结束----------

    //预分配好内存
    avPacket = av_packet_alloc();
    avFrame  = av_frame_alloc();
    avFrame2 = av_frame_alloc();
    avFrame3 = av_frame_alloc();

    //比较上一次文件的宽度高度,当改变时,需要重新分配内存
    if (oldWidth != videoWidth || oldHeight != videoHeight) {
        int byte = av_image_get_buffer_size (AV_PIX_FMT_RGB32, videoWidth, videoHeight,1);
        buffer = (uint8_t *)av_malloc(byte * sizeof(uint8_t));
        oldWidth = videoWidth;
        oldHeight = videoHeight;
    }

    //定义像素格式
    AVPixelFormat srcFormat = AV_PIX_FMT_YUV420P;
    AVPixelFormat dstFormat = AV_PIX_FMT_RGB32;
    //通过解码器获取解码格式
    srcFormat = videoCodec->pix_fmt;

    //默认最快速度的解码采用的SWS_FAST_BILINEAR参数,可能会丢失部分图片数据,可以自行更改成其他参数
    int flags = SWS_FAST_BILINEAR;

    //开辟缓存存储一帧数据
    //以下两种方法都可以,avpicture_fill已经逐渐被废弃
    //avpicture_fill((AVPicture *)avFrame3, buffer, dstFormat, videoWidth, videoHeight);
    av_image_fill_arrays(avFrame3->data, avFrame3->linesize, buffer, dstFormat, videoWidth, videoHeight, 1);

    //图像转换
    swsContext = sws_getContext(videoWidth, videoHeight, srcFormat, videoWidth, videoHeight, dstFormat, flags, NULL, NULL, NULL);

    //输出视频信息
    //av_dump_format(avFormatContext, 0, url.toStdString().data(), 0);

    //qDebug() << TIMEMS << "init ffmpeg finsh";
    return true;
}

void FFmpegThread::run()
{
    qint64 startTime = av_gettime();

    while (!stopped) {

        //根据标志位执行初始化操作
        if (isPlay) {

            this->init();

            isPlay = false;
            continue;
        }

        frameFinish = av_read_frame(avFormatContext, avPacket); //读取一帧数据
        if (frameFinish >= 0) {//确保读取到数据
            //判断当前包是视频还是音频
            int index = avPacket->stream_index;
            if (index == videoStreamIndex)
            {
                frameFinish = avcodec_send_packet(videoCodec, avPacket);//发送包输入解码器
                if (frameFinish < 0) {
                    continue;//成功，继续
                }

                frameFinish = avcodec_receive_frame(videoCodec, avFrame2);//接收数据来自解码器输出
                if (frameFinish < 0) {
                    continue;//成功，继续
                }

                if (frameFinish >= 0) {//
                    //将数据转成一张图片
                    sws_scale(swsContext, (const uint8_t *const *)avFrame2->data, avFrame2->linesize, 0, videoHeight, avFrame3->data, avFrame3->linesize);

                    //以下两种方法都可以
                    //QImage image(avFrame3->data[0], videoWidth, videoHeight, QImage::Format_RGB32);
                    QImage image((uchar *)buffer, videoWidth, videoHeight, QImage::Format_RGB32);
                    if (!image.isNull()) {
                        emit receiveImage(image);
                    }

                    usleep(1);
                }

                //延时(不然文件会立即全部播放完)
                AVRational timeBase = {1, AV_TIME_BASE};
                int64_t ptsTime = av_rescale_q(avPacket->dts, avFormatContext->streams[videoStreamIndex]->time_base, timeBase);
                int64_t nowTime = av_gettime() - startTime;
                if (ptsTime > nowTime) {
                    av_usleep(ptsTime - nowTime);
                }
            }
            /*-------------------------------------------------音频解码--------------------------------------------------------------------------*/
            else if (index == audioStreamIndex) {

//                frameFinish = avcodec_send_packet(audioCodec, avPacket);
//                if (frameFinish < 0) {
//                    continue;
//                }

//                frameFinish = avcodec_receive_frame(audioCodec, avFrame2);
//                if (frameFinish < 0) {
//                    continue;
//                }
//                 audioDevice =  audioDevices->defaultAudioOutput();

//                if (frameFinish >= 0) {//正常接收音频数据开始解码
//                    //将数据转成一张图片

//                    int result = swr_convert(swrContext, &buffer, avFrame2->nb_samples, (const uint8_t **)avFrame2->data, avFrame2->nb_samples);//程序崩溃于此

//                    if (result) {
//                        int outsize = av_samples_get_buffer_size(NULL, audioCodec->ch_layout.nb_channels, avFrame2->nb_samples, AV_SAMPLE_FMT_S16, 0);

//                     // ((char *)buffer, outsize);
//                    }

//                    usleep(1);
//                }

//                //延时(不然文件会立即全部播放完)
//                AVRational timeBase = {1, AV_TIME_BASE};
//                int64_t ptsTime = av_rescale_q(avPacket->dts, avFormatContext->streams[videoStreamIndex]->time_base, timeBase);
//                int64_t nowTime = av_gettime() - startTime;
//                if (ptsTime > nowTime) {
//                    av_usleep(ptsTime - nowTime);
//                  }
                //解码音频流,自行处理
            }
        }

        av_packet_unref(avPacket);
        av_freep(avPacket);
        usleep(1);
    }

    //线程结束后释放资源
    free();
    stopped = false;
    isPlay = false;
    qDebug() << TIMEMS << "stop ffmpeg thread";
}

void FFmpegThread::setUrl(const QString &url)
{
    this->url = url;
}

void FFmpegThread::free()
{
    if (swsContext != NULL) {
        sws_freeContext(swsContext);
        swsContext = NULL;
    }

    if (avPacket != NULL) {
        av_packet_unref(avPacket);
        avPacket = NULL;
    }

    if (avFrame != NULL) {
        av_frame_free(&avFrame);
        avFrame = NULL;
    }

    if (avFrame2 != NULL) {
        av_frame_free(&avFrame2);
        avFrame2 = NULL;
    }

    if (avFrame3 != NULL) {
        av_frame_free(&avFrame3);
        avFrame3 = NULL;
    }

    if (videoCodec != NULL) {
        avcodec_close(videoCodec);
        videoCodec = NULL;
    }

    if (audioCodec != NULL) {
        avcodec_close(audioCodec);
        audioCodec = NULL;
    }

    if (avFormatContext != NULL) {
        avformat_close_input(&avFormatContext);
        avFormatContext = NULL;
    }

    av_dict_free(&options);
    //qDebug() << TIMEMS << "close ffmpeg ok";
}
//void FFmpegThread::initAudioDevice(int sampleRate, int sampleSize, int channelCount)
//{

//    QAudioFormat format;
//    format.setCodec("audio/pcm");
//    format.setSampleRate(sampleRate);
//    format.setSampleSize(sampleSize * 8);
//    format.setChannelCount(channelCount);
//    format.setSampleType(QAudioFormat::SignedInt);
//    format.setByteOrder(QAudioFormat::LittleEndian);

//   // QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
//    //audioDeviceOk = info.isFormatSupported(format);
//    //if (audioDeviceOk) {
//       // audioOutput = new QAudioOutput(format);
//       // audioDevice = audioOutput->start();
//    //} //else {
//        qDebug() << TIMEMS << "Raw audio format not supported by backend, cannot play audio.";
//    //}
//}
void FFmpegThread::play()
{
    //通过标志位让线程执行初始化
    isPlay = true;
}

void FFmpegThread::pause()
{
    qDebug("pause开始测试");
    //通过标志位让线程执行暂停
        isPlay = false;
        stopped = true;
}

void FFmpegThread::next()
{
    qDebug("next开始测试");
        isPlay = true;
        stopped = false;

}

void FFmpegThread::stop()
{
    //通过标志位让线程停止
    stopped = true;
}

//实时视频显示窗体类
FFmpegWidget::FFmpegWidget(QWidget *parent) : QWidget(parent)
{
    thread = new FFmpegThread(this);
    connect(thread, SIGNAL(receiveImage(QImage)), this, SLOT(updateImage(QImage)));
    image = QImage();
}

FFmpegWidget::~FFmpegWidget()
{
    close();
}

void FFmpegWidget::paintEvent(QPaintEvent *)
{
    if (image.isNull()) {
        return;
    }

    //qDebug() << TIMEMS << "paintEvent" << objectName();
    QPainter painter(this);
#if 0
    //image = image.scaled(this->size(), Qt::KeepAspectRatio);
    //按照比例自动居中绘制
    int pixX = rect().center().x() - image.width() / 2;
    int pixY = rect().center().y() - image.height() / 2;
    QPoint point(pixX, pixY);
    painter.drawImage(point, image);
#else
    painter.drawImage(this->rect(), image);
#endif
}

void FFmpegWidget::updateImage(const QImage &image)
{
    //this->image = image.copy();
    this->image = image;
    this->update();
}

void FFmpegWidget::setUrl(const QString &url)
{
    thread->setUrl(url);
}

void FFmpegWidget::open()
{
    qDebug() << TIMEMS << "open video" << objectName();
    clear();

    thread->play();

    thread->start();

}

void FFmpegWidget::pause()
{
    thread->pause();
}

void FFmpegWidget::next()
{
    thread->next();
}

void FFmpegWidget::close()
{
    //qDebug() << TIMEMS << "close video" << objectName();
    if (thread->isRunning()) {
        thread->stop();
        thread->quit();
        thread->wait(500);
    }

    QTimer::singleShot(1, this, SLOT(clear()));
}

void FFmpegWidget::clear()
{
    image = QImage();
    update();
}
