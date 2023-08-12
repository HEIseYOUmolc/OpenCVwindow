#参考王维波老师QT6c++实现
#类似于第9章第三节例程，修改程序结构加入工程#数据库的库，参考王维波老师的QT6c++开发指南
#################################################################
HEADERS += $$PWD/QSL.h \
    $$PWD/scan.h
SOURCES += $$PWD/QSL.cpp \
    $$PWD/scan.cpp
HEADERS += $$PWD/tdialogdata.h
SOURCES += $$PWD/tdialogdata.cpp

FORMS    += $$PWD/tdialogdata.ui \
    $$PWD/scan.ui
FORMS    += $$PWD/QSL.ui

RESOURCES +=$$PWD/res.qrc


