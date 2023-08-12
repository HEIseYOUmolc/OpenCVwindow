#ifndef SCREENWIDGET_H
#define SCREENWIDGET_H

/**
 * 全局截屏控件
 * 1. 鼠标右键弹出菜单。
 * 2. 支持全局截屏。
 * 3. 支持局部截屏。
 * 4. 支持截图区域拖动。
 * 5. 支持图片另存为。
 */

#include <QWidget>
#include <QMenu>
#include <QPoint>
#include <QSize>
#include "qmutex.h"
#include "qapplication.h"
#include "qpainter.h"
#include "qfiledialog.h"
#include "qevent.h"
#include "qdatetime.h"
#include "qstringlist.h"
#include "qdebug.h"

#include "qscreen.h"
#include "opencv.h"



void ui_sinit(Ui::mainwindow *ui_ptr);//借用mainwindow的ui界面的控件指针初始化

class Screen
{
public:
    enum STATUS {SELECT, MOV, SET_W_H};
    Screen() {}
    Screen(QSize size);

    void setStart(QPoint pos);
    void setEnd(QPoint pos);
    QPoint getStart();
    QPoint getEnd();

    QPoint getLeftUp();
    QPoint getRightDown();

    STATUS getStatus();
    void setStatus(STATUS status);

    int width();
    int height();

    //检测坐标点是否在截图区域内
    bool isInArea(QPoint pos);
    //按坐标移动截图区域
    void move(QPoint p);

private:
    //记录 截图区域 左上角、右下角
    QPoint leftUpPos, rightDownPos;
    //记录 鼠标开始位置、结束位置
    QPoint startPos, endPos;
    //记录屏幕大小
    int maxWidth, maxHeight;
    //三个状态: 选择区域、移动区域、设置width height
    STATUS status;

    //比较两位置，判断左上角、右下角
    void cmpPoint(QPoint &s, QPoint &e);
};


class ScreenWidget : public QWidget
{
    Q_OBJECT
public:
    static ScreenWidget *Instance();
    explicit ScreenWidget(QWidget *parent = 0);

private:
    Ui::mainwindow *screenui;
    static QScopedPointer<ScreenWidget> self;
    QMenu *menu;            //右键菜单对象
    Screen *screen;         //截屏对象
    QPixmap *fullScreen;    //保存全屏图像
    QPixmap *bgScreen;      //模糊背景图
    QPoint movPos;          //坐标

protected:
    void contextMenuEvent(QContextMenuEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
    void showEvent(QShowEvent *);

private slots:
    void saveandinfScreen();//自设
    void saveScreen();
    void saveFullScreen();
    void saveScreenOther();
    void saveFullOther(); 
};
#endif // SCREENWIDGET_H
