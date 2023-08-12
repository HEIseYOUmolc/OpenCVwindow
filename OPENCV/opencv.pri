# OPENCV.pri

#深度学习相关
INCLUDEPATH += $$PWD/opencv
include ($$PWD/opencv/opencv.pri)#opencv功能实现库自己编写，inference为官方库

INCLUDEPATH += $$PWD/screen
include ($$PWD/screen/screen.pri)#screen功能实现库参考飞扬青云大佬博客编写

INCLUDEPATH += $$PWD/vlc
include ($$PWD/vlc/vlc.pri)#播放流媒体的库

INCLUDEPATH += $$PWD/FFmpeg
include ($$PWD/ffmpeg/ffmpeg.pri)#播放流媒体的库，使用2023年7月1日编译的6.0版本FFmpeg，功能不完善
