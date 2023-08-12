#ifndef FFMPEGHEAD_H
#define FFMPEGHEAD_H

//必须加以下内容,否则编译不能通过,为了兼容C和C99标准
#ifndef INT64_C
#define INT64_C
#define UINT64_C
#endif

//引入ffmpeg头文件
// 需要使用C来对C++进行支持
// 注意注意注意，这里的C是大写的！不是小写的！小写会报错！
//以下注释根据文件顺序
extern "C"
{
    //①avcodec:编解码(最重要的库)
#include <libavcodec/avcodec.h>

    //②avdevice:各种设备的输入输出
#include <libavdevice/avdevice.h>

    //③avfilter:过滤库,滤镜特效处理
#include <libavfilter/avfilter.h>

    //④avformat:封装格式处理
#include <libavformat/avformat.h>

    //⑤avutil:工具库（大部分库都需要这个库的支持）
#include <libavutil/opt.h>
#include <libavutil/time.h>
#include <libavutil/frame.h>
#include <libavutil/pixdesc.h>
#include <libavutil/avassert.h>
#include <libavutil/imgutils.h>
#include <libavutil/ffversion.h>//定义版本

    //⑥postproc:后加工
//#include <libpostproc/postprocess.h>该实例暂无用到

    //⑦swresample:音频采样数据格式转换
#include <libswresample/swresample.h>

    //⑧swscale:视频像素数据格式转换
#include <libswscale/swscale.h>
#ifdef ffmpegdevice
#include "libavdevice/avdevice.h"
#endif

#ifndef gcc45
#include "libavutil/hwcontext.h"
#include "libavutil/hwcontext_qsv.h"
#endif
}


#include "qdatetime.h"
#pragma execution_character_set("utf-8")

#define TIMEMS          qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))
#define TIME            qPrintable(QTime::currentTime().toString("HH:mm:ss"))
#define QDATE           qPrintable(QDate::currentDate().toString("yyyy-MM-dd"))
#define QTIME           qPrintable(QTime::currentTime().toString("HH-mm-ss"))
#define DATETIME        qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
#define STRDATETIME     qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))
#define STRDATETIMEMS   qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss-zzz"))

#endif // FFMPEGHEAD_H
