QT       += core gui serialport core5compat sql network\
    quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    main.cpp \
    mainwindow.cpp \

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
#opencv路径设置
win32:CONFIG(release, debug|release): LIBS += -LE:/opencv/build_qt/x64/vc15/lib/ -lopencv_world470
else:win32:CONFIG(debug, debug|release): LIBS += -LE:/opencv/build_qt/x64/vc15/lib/ -lopencv_world470d

INCLUDEPATH += E:/opencv/build_qt/x64/vc15
DEPENDPATH += E:/opencv/build_qt/x64/vc15


###############################################
# 手动添加的OpenCV相关文件设置
###############################################
INCLUDEPATH +=E:\opencv\build_qt\include \
 E:\opencv\build_qt\include\opencv2
###############################################

RESOURCES +=


#扩展库
INCLUDEPATH += $$PWD/OPENCV
include ($$PWD/OPENCV/OPENCV.pri)

INCLUDEPATH += $$PWD/QSL
include ($$PWD/QSL/QSL.pri)

INCLUDEPATH += $$PWD/NETW
include ($$PWD/NETW/NETW.pri)





