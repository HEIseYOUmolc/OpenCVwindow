#ifndef UDPMULTICAST_H
#define UDPMULTICAST_H

#include    <QMainWindow>
#include    <QtNetwork>
#include    <QUdpSocket>
#include    <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class UDPMulticast; }
QT_END_NAMESPACE

class UDPMulticast : public QMainWindow
{
    Q_OBJECT

private:
    QLabel  *labSocketState;
    QUdpSocket  *udpSocket;         //socket连接
    QHostAddress    groupAddress;   //组播地址
    QString getLocalIP();           //获取本机IP地址
public:
    UDPMulticast(QWidget *parent = nullptr);
    ~UDPMulticast();

private slots:
    //自定义槽函数
    void    do_socketStateChange(QAbstractSocket::SocketState socketState);
    void    do_socketReadyRead();//读取socket传入的数据
    //
    void on_actStart_triggered();

    void on_actStop_triggered();

    void on_actClear_triggered();

    void on_actHostInfo_triggered();

    void on_btnMulticast_clicked();

private:
    Ui::UDPMulticast *ui;
};

#endif // UDPMULTICAST_H
