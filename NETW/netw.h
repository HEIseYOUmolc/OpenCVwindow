#ifndef NETW_H
#define NETW_H

#include <QWidget>
#include <mainwindow.h>
#include <TCPClient.h>
#include <TCPServer.h>
#include <HTTP.h>
#include <UDP.h>
#include <UDPMulticast.h>
namespace Ui {
class netw;
}

class netw : public QWidget
{
    Q_OBJECT

public:
    explicit netw(QWidget *parent = nullptr);
    ~netw();
    quint64 basize;
private slots:

    void on_BT_TCPClient_clicked();

    void on_BT_TCPServer_clicked();

    void on_BT_Multicast_clicked();

    void on_BT_UDP_clicked();

    void on_BT_HTTP_clicked();

    void video_tcp_receive_show();

    void video_udp_receive_show();

    void video_receive_wait();

    void video_tcp_ready_listen();



protected:

    void ShowImage(QByteArray ba);
private:
    Ui::netw *ui;
    QUdpSocket *receiver;
    QTcpSocket *tcpsocket;
    QTcpSocket *clientSocket;
    QTcpServer *server;//TCP服务器
    QMovie *movie;
    QTimer *packetReceiveTimer;


};

#endif // NETW_H
