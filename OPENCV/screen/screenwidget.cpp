#pragma execution_character_set("utf-8")

#include "screenwidget.h"

#define deskGeometry qApp->primaryScreen()->geometry()
#define deskGeometry2 qApp->primaryScreen()->availableGeometry()

#define STRDATETIME qPrintable (QDateTime::currentDateTime().toString("[yyyy-MM-dd-HH-mm-ss]"))


Ui::mainwindow *sui = nullptr;

void ui_sinit(Ui::mainwindow *ui_ptr)
{
    sui = ui_ptr;
}
//实例化
Opencv screencv;//opencv分类的函数
QString screen_modelname;//使用的模型函数
QString screen_classname;//使用的类别函数
/*********************************************************************
 * 函数功能：截图相关的功能
 * 参 数：无
 * 返回值： 无
 * 类 别：UI界面record按钮
 *********************************************************************/
void mainwindow::on_load_record_clicked()
{
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
    ScreenWidget::Instance()->showFullScreen();
}
Screen::Screen(QSize size)
{
    maxWidth = size.width();
    maxHeight = size.height();

    startPos = QPoint(-1, -1);
    endPos = startPos;
    leftUpPos = startPos;
    rightDownPos = startPos;
    status = SELECT;
}

int Screen::width()
{
    return maxWidth;
}

int Screen::height()
{
    return maxHeight;
}

Screen::STATUS Screen::getStatus()
{
    return status;
}

void Screen::setStatus(STATUS status)
{
    this->status = status;
}

void Screen::setEnd(QPoint pos)
{
    endPos = pos;
    leftUpPos = startPos;
    rightDownPos = endPos;
    cmpPoint(leftUpPos, rightDownPos);
}

void Screen::setStart(QPoint pos)
{
    startPos = pos;
}

QPoint Screen::getEnd()
{
    return endPos;
}

QPoint Screen::getStart()
{
    return startPos;
}

QPoint Screen::getLeftUp()
{
    return leftUpPos;
}

QPoint Screen::getRightDown()
{
    return rightDownPos;
}

bool Screen::isInArea(QPoint pos)
{
    if (pos.x() > leftUpPos.x() && pos.x() < rightDownPos.x() && pos.y() > leftUpPos.y() && pos.y() < rightDownPos.y()) {
        return true;
    }

    return false;
}

void Screen::move(QPoint p)
{
    int lx = leftUpPos.x() + p.x();
    int ly = leftUpPos.y() + p.y();
    int rx = rightDownPos.x() + p.x();
    int ry = rightDownPos.y() + p.y();

    if (lx < 0) {
        lx = 0;
        rx -= p.x();
    }

    if (ly < 0) {
        ly = 0;
        ry -= p.y();
    }

    if (rx > maxWidth)  {
        rx = maxWidth;
        lx -= p.x();
    }

    if (ry > maxHeight) {
        ry = maxHeight;
        ly -= p.y();
    }
    leftUpPos = QPoint(lx, ly);
    rightDownPos = QPoint(rx, ry);
    startPos = leftUpPos;
    endPos = rightDownPos;
}

void Screen::cmpPoint(QPoint &leftTop, QPoint &rightDown)
{
    QPoint l = leftTop;
    QPoint r = rightDown;

    if (l.x() <= r.x()) {
        if (l.y() <= r.y()) {
        } else {
            leftTop.setY(r.y());
            rightDown.setY(l.y());
        }
    } else {
        if (l.y() < r.y()) {
            leftTop.setX(r.x());
            rightDown.setX(l.x());
        } else {
            QPoint tmp;
            tmp = leftTop;
            leftTop = rightDown;
            rightDown = tmp;
        }
    }
}

QScopedPointer<ScreenWidget> ScreenWidget::self;
ScreenWidget *ScreenWidget::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new ScreenWidget);
        }
    }

    return self.data();
}
/*********************************************************************
 * 函数功能：截图右键菜单
 * 参 数：无
 * 返回值： 无
 * 类 别：UI界面截图后生成的右键菜单
 *********************************************************************/
ScreenWidget::ScreenWidget(QWidget *parent) : QWidget(parent)
{
    //this->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);

    menu = new QMenu(this);
    menu->addAction("保存当前截图", this, SLOT(saveScreen()));
    menu->addAction("检测并保存当前截图", this, SLOT(saveandinfScreen()));
    menu->addAction("保存全屏截图", this, SLOT(saveFullScreen()));
    menu->addAction("截图另存为", this, SLOT(saveScreenOther()));
    menu->addAction("全屏另存为", this, SLOT(saveFullOther()));
    menu->addAction("退出截图", this, SLOT(hide()));

    //取得屏幕大小
    screen = new Screen(deskGeometry.size());
    //保存全屏图像
    fullScreen = new QPixmap();
}

void ScreenWidget::paintEvent(QPaintEvent *)
{
    int x = screen->getLeftUp().x();
    int y = screen->getLeftUp().y();
    int w = screen->getRightDown().x() - x;
    int h = screen->getRightDown().y() - y;

    QPainter painter(this);

    QPen pen;
    pen.setColor(Qt::green);
    pen.setWidth(2);
    pen.setStyle(Qt::DotLine);
    painter.setPen(pen);
    painter.drawPixmap(0, 0, *bgScreen);

    if (w != 0 && h != 0) {
        painter.drawPixmap(x, y, fullScreen->copy(x, y, w, h));
    }

    painter.drawRect(x, y, w, h);

    pen.setColor(Qt::yellow);
    painter.setPen(pen);
    painter.drawText(x + 2, y - 8, tr("截图范围：( %1 x %2 ) - ( %3 x %4 )  图片大小：( %5 x %6 )")
                     .arg(x).arg(y).arg(x + w).arg(y + h).arg(w).arg(h));
}

void ScreenWidget::showEvent(QShowEvent *)
{
    QPoint point(-1, -1);
    screen->setStart(point);
    screen->setEnd(point);

    QScreen *pscreen = QApplication::primaryScreen();
    *fullScreen = pscreen->grabWindow(0, 0, 0, screen->width(), screen->height());

    //设置透明度实现模糊背景
    QPixmap pix(screen->width(), screen->height());
    pix.fill((QColor(160, 160, 160, 200)));
    bgScreen = new QPixmap(*fullScreen);
    QPainter p(bgScreen);
    p.drawPixmap(0, 0, pix);
}

/*********************************************************************
 * 函数功能：保存选定区域
 * 参 数：无
 * 返回值： 无
 * 类 别：UI界面截图右键菜单函数
 *********************************************************************/
void ScreenWidget::saveScreen()
{
    int x = screen->getLeftUp().x();
    int y = screen->getLeftUp().y();
    int w = screen->getRightDown().x() - x;
    int h = screen->getRightDown().y() - y;

    QString fileName = QString("%1/screen_%2.png").arg(qApp->applicationDirPath()).arg(STRDATETIME);
    fullScreen->copy(x, y, w, h).save(fileName, "png");
    close();
}
/*********************************************************************
 * 函数功能：选定区域，并使用模型推理保存
 * 参 数：无
 * 返回值： 无
 * 类 别：UI界面截图右键菜单函数
 *********************************************************************/
void ScreenWidget::saveandinfScreen()
{
    int x = screen->getLeftUp().x();
    int y = screen->getLeftUp().y();
    int w = screen->getRightDown().x() - x;
    int h = screen->getRightDown().y() - y;
    //定义中间Qimage变量;
    QImage infimg,oriimg;
    //设置默认保存文件名
    QString fileName = QString("%1/screen_capture%2.png").arg(qApp->applicationDirPath()).arg(STRDATETIME);
    //保存
    fullScreen->copy(x, y, w, h).save(fileName, "png");
    //格式转换
    oriimg=fullScreen->copy(x, y, w, h).toImage();
    //显示原图像
    sui->label_origin->setPixmap(QPixmap::fromImage(oriimg.scaled(sui->label_origin->size())));
    Mat frame=screencv.qim2mat(oriimg);//转换格式有问题
    //判断模型和类型是否正确
    //推断转为QIMG格式显示
    frame=screencv.Inference_c(model_name,class_name,frame);
    infimg=screencv.mat2qim(frame);
   //cv::imshow("Screenshot Image",frame);
    sui->label_inference->setPixmap(QPixmap::fromImage(infimg.scaled(sui->label_inference->size())));
    close();
}
//全屏保存
void ScreenWidget::saveFullScreen()
{
    QString fileName = QString("%1/full_%2.png").arg(qApp->applicationDirPath()).arg(STRDATETIME);
    fullScreen->save(fileName, "png");
    close();
}

void ScreenWidget::saveScreenOther()
{
    QString name = QString("%1.png").arg(STRDATETIME);
    QString fileName = QFileDialog::getSaveFileName(this, "保存图片", name, "png Files (*.png)");
    if (!fileName.endsWith(".png")) {
        fileName += ".png";
    }
    if (fileName.length() > 0) {
        int x = screen->getLeftUp().x();
        int y = screen->getLeftUp().y();
        int w = screen->getRightDown().x() - x;
        int h = screen->getRightDown().y() - y;
        fullScreen->copy(x, y, w, h).save(fileName, "png");
        close();
    }
}

void ScreenWidget::saveFullOther()
{
    QString name = QString("%1.png").arg(STRDATETIME);
    QString fileName = QFileDialog::getSaveFileName(this, "保存图片", name, "png Files (*.png)");
    if (!fileName.endsWith(".png")) {
        fileName += ".png";
    }

    if (fileName.length() > 0) {
        fullScreen->save(fileName, "png");
        close();
    }
}

void ScreenWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (screen->getStatus() == Screen::SELECT) {
        screen->setEnd(e->pos());
    } else if (screen->getStatus() == Screen::MOV) {
        QPoint p(e->x() - movPos.x(), e->y() - movPos.y());
        screen->move(p);
        movPos = e->pos();
    }

    this->update();
}

void ScreenWidget::mousePressEvent(QMouseEvent *e)
{
    int status = screen->getStatus();

    if (status == Screen::SELECT) {
        screen->setStart(e->pos());
    } else if (status == Screen::MOV) {
        if (screen->isInArea(e->pos()) == false) {
            screen->setStart(e->pos());
            screen->setStatus(Screen::SELECT);
        } else {
            movPos = e->pos();
            this->setCursor(Qt::SizeAllCursor);
        }
    }

    this->update();
}
void ScreenWidget::mouseReleaseEvent(QMouseEvent *)
{
    if (screen->getStatus() == Screen::SELECT) {
        screen->setStatus(Screen::MOV);
    } else if (screen->getStatus() == Screen::MOV) {
        this->setCursor(Qt::ArrowCursor);
    }
}

void ScreenWidget::contextMenuEvent(QContextMenuEvent *)
{
    this->setCursor(Qt::ArrowCursor);
    menu->exec(cursor().pos());
}
