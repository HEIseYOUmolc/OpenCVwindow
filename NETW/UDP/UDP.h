#ifndef UDP_H
#define UDP_H

#include <QMainWindow>

#include    <QUdpSocket>
#include    <QLabel>
#include    <QMovie>

#include <opencv2/opencv.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class UDP; }
QT_END_NAMESPACE

class UDP : public QMainWindow
{
    Q_OBJECT

private:
    QLabel  *labSocketState;    //状态栏上的标签
    QUdpSocket  *udpSocket;
    QString getLocalIP();       //获取本机IP地址

    Ui::UDP *ui;
    cv::VideoCapture camera;
    QTimer *udp_timer;
    QMovie *movie;

public:
    UDP(QWidget *parent = nullptr);
    ~UDP();

private slots:
    //自定义槽函数
    void    do_socketStateChange(QAbstractSocket::SocketState socketState);

    void    do_socketReadyRead();//读取socket传入的数据
    //
    void on_actStart_triggered();

    void on_actStop_triggered();

    void on_actClear_triggered();

    void on_btnSend_clicked();

    void on_btnBroadcast_clicked();
    //void on_actionactSend_triggered();
    void VideoSend();
    void on_BTOPEN_clicked();
    void on_BTCLOSE_clicked();
};

#endif // UDP_H
