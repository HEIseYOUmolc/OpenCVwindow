#ffmpeg6.0编译
#ffmpeg参考飞云青云大佬博客参考代码，尝试实现音频失败
#################################################################


HEADERS += $$PWD/ffmpeghead.h
HEADERS += $$PWD/ffmpeg.h
SOURCES += $$PWD/ffmpeg.cpp

###################FFmpeg官方编译依赖选择###########################

INCLUDEPATH +=$$PWD/ffmpeglib/include

LIBS += $$PWD/ffmpeglib/lib/avcodec.lib \
        $$PWD/ffmpeglib/lib/avfilter.lib \
        $$PWD/ffmpeglib/lib/avformat.lib \
        $$PWD/ffmpeglib/lib/avutil.lib \
        $$PWD/ffmpeglib/lib/postproc.lib \
        $$PWD/ffmpeglib/lib/swresample.lib \
        $$PWD/ffmpeglib/lib/swscale.lib
