#include "netw.h"
#include "ui_netw.h"

netw::netw(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::netw)
{
    ui->setupUi(this);

    //udp 接收端
    receiver=new QUdpSocket(this);   //创建socket
    receiver->bind(QHostAddress::Any,8888);
    connect(receiver,SIGNAL(readyRead()),this,SLOT(video_udp_receive_show()));//读取到数据执行


    //tcp服务器 
    server = new QTcpServer(this);
    server->listen(QHostAddress::Any, 8888);
    connect(server, SIGNAL(newConnection()), this, SLOT(video_tcp_ready_listen()));
    ui->info->insertPlainText("等待客户端连接...\n");

    //空闲时加载动画
    movie = new QMovie(":/images/images/loading.gif");
    ui->label->setMovie(movie); // 1. 设置要显示的 GIF 动画图片
    movie->start();              // 2. 启动动画
    // 设置一个定时器，用于检测数据包是否连续接收
    packetReceiveTimer = new QTimer(this);
    packetReceiveTimer->setInterval(3000);  // 设置定时器间隔，单位为毫秒
    connect(packetReceiveTimer,SIGNAL(timeout()), this, SLOT(video_receive_wait()));
    packetReceiveTimer->start();
}


netw::~netw()
{
    delete ui;
}
/*********************************************************************
 * 函数功能：打开网络调试功能
 * 参 数：  无
 * 返回值： 无
 * 类 别：UI界面BT_NETW按钮
 *********************************************************************/
void mainwindow::on_BT_NETW_clicked()
{
    netw *netwUi = new netw;
    netwUi->show();
}

void netw::on_BT_TCPClient_clicked()
{
    TCPClient *tcpclient = new TCPClient;
    tcpclient->show();
}

void netw::on_BT_TCPServer_clicked()
{
    TCPServer *tcpserver = new TCPServer;
    tcpserver->show();
}


void netw::on_BT_Multicast_clicked()
{
    UDPMulticast *udpmulticast=new UDPMulticast;
    udpmulticast->show();   
}
void netw::on_BT_UDP_clicked()
{
    UDP *udp=new UDP;
    udp->show();
}


void netw::on_BT_HTTP_clicked()
{
    HTTP *http=new HTTP;
    http->show();

}
void netw::video_tcp_ready_listen()
{
    ui->info->insertPlainText("客户端连接成功！\n");
    // 在这里处理接收到的数据包
    //movie->stop();
    basize=0;
    clientSocket = server->nextPendingConnection();
    QString message = QString("Received type:tcp data from address:%1 port:%2").arg(clientSocket->peerAddress().toString()).arg(clientSocket->peerPort());
    ui->addressLabel->setText(message);
    // 在此处添加处理客户端连接的代码
    connect(clientSocket,SIGNAL(readyRead()),this,SLOT(video_tcp_receive_show()));
    connect(clientSocket, &QTcpSocket::disconnected, this, &QTcpSocket::deleteLater);
}

void netw::video_tcp_receive_show()
{
    // 修改前代码...
    QByteArray databy;//存放从服务器接收到的字符串
    // 在这里处理接收到的数据包
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_6_5);

    if (basize==0)
    {
        //判断接收的数据是否有两字节（文件大小信息）
        //如果有则保存到basize变量中，没有则返回，继续接收数据
        if (clientSocket->bytesAvailable()<(int)sizeof(quint64))
        {
            qDebug() << "没有足够数据";
            return;
        }
        in>>basize;// 从数据流中读取图像大小
    }
    //如果没有得到一幅图像的全部数据，则返回继续接收数据
    if (clientSocket->bytesAvailable() < basize)
    {
        return;
    }
    //如果没有得到全部数据，则返回继续接收数据
    in>>databy;//将接收到的数据存放到变量中
    basize=0;
    ShowImage(databy);	//显示当前接收到的这一幅图像
}
void netw::ShowImage(QByteArray ba)	//从接收到了字节流中，执行与服务器断相反的操作：解压缩、解释为图像数据
{
    QString ss = QString::fromLatin1(ba.data(), ba.size());
    QByteArray rc = QByteArray::fromBase64(ss.toLocal8Bit());
    QByteArray rdc = qUncompress(rc);

    QImage img;
    img.loadFromData(rdc,"JPEG");//解释为jpg格式的图像

    movie->stop();
    ui->label->setPixmap(QPixmap::fromImage(img));
    ui->label->resize(img.size());

    update();
}

void netw::video_udp_receive_show()
{   
    // 在这里处理接收到的数据包
    movie->stop();
    quint64 size = receiver->pendingDatagramSize();
    QByteArray buff;
    buff.resize(size);
    QHostAddress adrr;
    quint16 port;
    receiver->readDatagram(buff.data(),buff.size(),&adrr,&port);
    QString message = QString("Received type:udp data from address:%1 port:%2").arg(adrr.toString()).arg(port);
    ui->addressLabel->setText(message);
    buff = qUncompress(buff);
    QBuffer buffer(&buff);
    QImageReader reader(&buffer,"JPEG");//可读入磁盘文件、设备文件中的图像、以及其他图像数据如pixmap和image，相比较更加专业。
    //buffer属于设备文件一类，
    QImage image = reader.read();//read()方法用来读取设备图像，也可读取视频，读取成功返回QImage*，否则返回NULL
    ui->label->setPixmap(QPixmap::fromImage(image));
    ui->label->resize(image.width(),image.height());
}
void netw::video_receive_wait()
{
    ui->label->setMovie(movie); // 1. 设置要显示的 GIF 动画图片
    movie->start();
}




