#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include    <QMainWindow>

#include    <QTcpSocket>
#include    <QLabel>
#include    <QDataStream> // Include the necessary header for data stream
#include    <QBuffer>     // Include the necessary header for buffer
#include    <QTimer>
#include    <QMovie>
#include <opencv2/opencv.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class TCPClient; }
QT_END_NAMESPACE

class TCPClient : public  QMainWindow
{
    Q_OBJECT
private:
    QTcpSocket  *tcpClient;   //socket
    QLabel  *labSocketState;  //状态栏显示标签

    cv::VideoCapture camera;
    QTimer *tcp_timer;
    QString  getLocalIP();    //获取本机IP地址
    QMovie *movie;
public:
    TCPClient(QWidget *parent = nullptr);
    ~TCPClient();

private slots:
//自定义槽函数
    void    do_connected();
    void    do_disconnected();
    void    do_socketStateChange(QAbstractSocket::SocketState socketState);
    void    do_socketReadyRead();//读取socket传入的数据
    void    VideoSend();
//
    void on_actConnect_triggered();

    void on_actDisconnect_triggered();

    void on_actClear_triggered();

    void on_btnSend_clicked();



    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::TCPClient *ui;
};

#endif // TCPCLIENT_H
