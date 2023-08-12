#ifndef GIFWIDGET_H
#define GIFWIDGET_H

/**
 * GIF录屏控件
 * 1. 可设置要录制屏幕的宽高，支持右下角直接拉动改变。
 * 2. 可设置变宽的宽度。
 * 3. 可设置录屏控件的背景颜色。
 * 4. 可设置录制的帧数。
 * 5. 录制区域可自由拖动选择。
 */


#include "qmutex.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include "qlayout.h"
#include "qpainter.h"
#include "qevent.h"
#include "qstyle.h"
#include "qpixmap.h"
#include "qtimer.h"
#include "qdatetime.h"
#include "qapplication.h"
#include "qdesktopservices.h"
#include "qfiledialog.h"
#include "qurl.h"
#include "qtextcodec.h"
#include "qdebug.h"
#include "qscreen.h"
#include <QLabel>
#include <QDialog>
#include "gif.h"

#include "opencv.h"

class QLineEdit;
class QLabel;

void ui_ginit(Ui::mainwindow *ui_ptr);//借用mainwindow的ui界面的控件指针初始化

class GifWidget : public QDialog
{
    Q_OBJECT

    Q_PROPERTY(int borderWidth READ getBorderWidth WRITE setBorderWidth)
    Q_PROPERTY(QColor bgColor READ getBgColor WRITE setBgColor)

public:
    static GifWidget *Instance();
    explicit GifWidget(QWidget *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);

private:
    static QScopedPointer<GifWidget> self;
    QWidget *widgetTop;         //标题栏
    QWidget *widgetMain;        //中间部分
    QWidget *widgetBottom;      //底部栏
    QLineEdit *txtFps;          //帧率输入框
    QLineEdit *txtWidth;        //宽度输入框
    QLineEdit *txtHeight;       //高度输入框
   //QLineEdit *txtFormat;       //格式输入框
    QComboBox *FormatcomBox;    //格式选择框
    QPushButton *btnStart;      //开始按钮
    QPushButton *btnInf;        //检测按钮
    QLabel *labStatus;          //显示状态信息

    int fps;                    //帧数 100为1s
    int borderWidth;            //边框宽度
    QColor bgColor;             //背景颜色

    int count;                  //帧数计数
    QString savename;           //保存文件名称
    QString saveName;           //保存文件
    QRect rectGif;              //截屏区域
    QTimer *timer;              //截屏定时器
    QTimer *inftimer;           //截屏检测定时器

    Gif gif;                    //gif类对象
    Gif::GifWriter *gifWriter;  //gif写入对象

public:
    int getBorderWidth()        const;
    QColor getBgColor()         const;

private slots:
    void initControl();
    void initForm();
    void saveImage();
    void saveandinfImage();
    void record();
    void inference();
    void closeAll();
    void resizeForm();

public Q_SLOTS:
    void setBorderWidth(int borderWidth);
    void setBgColor(const QColor &bgColor);
};
#endif // GIFWIDGET_H
