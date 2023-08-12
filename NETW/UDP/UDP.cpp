#include "UDP.h"
#include "ui_UDP.h"
#include    <QtNetwork>

/*********************************************************************
 * 函数功能：获取当前本地IP地址
 * 参 数：  无
 * 返回值： QString localIP
 * 类 别：UDP
 *********************************************************************/
QString UDP::getLocalIP()
{
    QString hostName=QHostInfo::localHostName();            //获取本地主机名
    QHostInfo   hostInfo=QHostInfo::fromName(hostName);     // 根据主机名获取主机信息
    QString   localIP="";                                   // 初始化本地 IP 地址字符串
    QList<QHostAddress> addList=hostInfo.addresses();       // 获取主机信息中的 IP 地址列表
    if (addList.isEmpty())                                  // 如果 IP 地址列表为空，返回空的 IP 地址字符串
        return localIP;

    foreach (QHostAddress aHost,addList)                    // 遍历 IP 地址列表，寻找 IPv4 地址
        if (QAbstractSocket::IPv4Protocol==aHost.protocol())
        {
            localIP=aHost.toString();
            break;
        }
    return localIP;
}
/*********************************************************************
 * 函数功能：完成ui界面设置
 * 参 数：  无
 * 返回值： 无
 * 类 别：UDP
 *********************************************************************/
UDP::UDP(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UDP)
{
    ui->setupUi(this);
    labSocketState=new QLabel("Socket状态：");
    labSocketState->setMinimumWidth(200);
    ui->statusBar->addWidget(labSocketState);

    QString localIP=getLocalIP();   //本机IP
    this->setWindowTitle(this->windowTitle()+"----本机IP地址："+localIP);
    ui->comboTargetIP->addItem(localIP);

    udpSocket=new QUdpSocket(this);   //创建socket

    //udpcSocket = new QUdpSocket(this);

    udpSocket->bind(QHostAddress::Any,8888);
    udp_timer = new QTimer(this);
    connect(udp_timer,SIGNAL(timeout()),this,SLOT(VideoSend()));

    connect(udpSocket,&QUdpSocket::stateChanged,this,&UDP::do_socketStateChange);
    do_socketStateChange(udpSocket->state());  //执行一次，显示当前状态

    connect(udpSocket,SIGNAL(readyRead()), this,SLOT(do_socketReadyRead()));
    //播放
    movie = new QMovie(":/images/images/loading.gif");
    ui->label_VIDEO->setMovie(movie); // 1. 设置要显示的 GIF 动画图片
    movie->start();              // 2. 启动动画
}

UDP::~UDP()
{
    delete ui;
}
void UDP::VideoSend()
{
    QHostAddress dstip = (QHostAddress)(ui->comboTargetIP->currentText());
    quint16 dstport = ui->spinTargetPort->text().toInt();
    cv::Mat frame;
    camera.read(frame);
    cv::cvtColor(frame,frame,cv::COLOR_BGR2RGB);
    QImage image((unsigned char *)(frame.data),frame.cols,frame.rows,QImage::Format_RGB888);
    ui->label_VIDEO->setPixmap(QPixmap::fromImage(image));
    ui->label_VIDEO->resize(image.width(),image.height());
    QByteArray byte;
    QBuffer buff(&byte);
    buff.open(QIODevice::WriteOnly);
    image.save(&buff,"JPEG");
    QByteArray ss = qCompress(byte,5);
    udpSocket->writeDatagram(ss,dstip,dstport);
}
/*********************************************************************
 * 函数功能：根据socketState切换左下角的label信息，反馈状态信息
 * 参 数：  无
 * 返回值： 无
 * 类 别：UDP
 *********************************************************************/
void UDP::do_socketStateChange(QAbstractSocket::SocketState socketState)
{
    switch(socketState)
    {
    case QAbstractSocket::UnconnectedState:
        labSocketState->setText("socket状态：UnconnectedState");
        break;
    case QAbstractSocket::HostLookupState:
        labSocketState->setText("socket状态：HostLookupState");
        break;
    case QAbstractSocket::ConnectingState:
        labSocketState->setText("socket状态：ConnectingState");
        break;
    case QAbstractSocket::ConnectedState:
        labSocketState->setText("socket状态：ConnectedState");
        break;
    case QAbstractSocket::BoundState:
        labSocketState->setText("socket状态：BoundState");
        break;
    case QAbstractSocket::ClosingState:
        labSocketState->setText("socket状态：ClosingState");
        break;
    case QAbstractSocket::ListeningState:
        labSocketState->setText("socket状态：ListeningState");
    }
}
/*********************************************************************
 * 函数功能：读取收到的数据报
 * 参 数：  无
 * 返回值： 无
 * 类 别：UDP
 *********************************************************************/
void UDP::do_socketReadyRead()
{
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray   datagram;
        datagram.resize(udpSocket->pendingDatagramSize());

        QHostAddress    peerAddr;
        quint16 peerPort;
        udpSocket->readDatagram(datagram.data(),datagram.size(),&peerAddr,&peerPort);
        QString str=datagram.data();

        QString peer="[From "+peerAddr.toString()+":"+QString::number(peerPort)+"] ";
        ui->textEdit->appendPlainText(peer+str);
    }
}
/*********************************************************************
 * 函数功能："绑定端口"按钮
 * 参 数：  无
 * 返回值： 无
 * 类 别：UDP
 *********************************************************************/
void UDP::on_actStart_triggered()
{
    quint16 port=ui->spinBindPort->value();  //本机UDP端口
    if (udpSocket->bind(port))   //绑定端口成功
    {
        ui->textEdit->appendPlainText("**已成功绑定");
        ui->textEdit->appendPlainText("**绑定端口："
                        +QString::number(udpSocket->localPort()));
        ui->actStart->setEnabled(false);
        ui->actStop->setEnabled(true);
        ui->btnSend->setEnabled(true);
        ui->btnBroadcast->setEnabled(true);
    }
    else
        ui->textEdit->appendPlainText("**绑定失败");
}
/*********************************************************************
 * 函数功能："解除绑定"按钮
 * 参 数：  无
 * 返回值： 无
 * 类 别：UDP
 *********************************************************************/
void UDP::on_actStop_triggered()
{
    udpSocket->abort();       //解除绑定，复位socket
    ui->actStart->setEnabled(true);
    ui->actStop->setEnabled(false);
    ui->btnSend->setEnabled(false);
    ui->btnBroadcast->setEnabled(false);
    ui->textEdit->appendPlainText("**已解除绑定");
}


/*********************************************************************
 * 函数功能："清除文本框" 按钮
 * 参 数：  无
 * 返回值： 无
 * 类 别：UDP
 *********************************************************************/
void UDP::on_actClear_triggered()
{
    ui->textEdit->clear();
}
/*********************************************************************
 * 函数功能："发送消息" 按钮
 * 参 数：  无
 * 返回值： 无
 * 类 别：UDP
 *********************************************************************/
void UDP::on_btnSend_clicked()
{
    QString     targetIP=ui->comboTargetIP->currentText();  //目标IP
    QHostAddress  targetAddr(targetIP);
    quint16     targetPort=ui->spinTargetPort->value();     //目标port
    QString  msg=ui->editMsg->text();       //发送的消息内容

    QByteArray  str=msg.toUtf8();
    udpSocket->writeDatagram(str,targetAddr,targetPort); //发出数据报
    ui->textEdit->appendPlainText("[out] "+msg);
    ui->editMsg->clear();
    ui->editMsg->setFocus();
}
/*********************************************************************
 * 函数功能："广播消息" 按钮
 * 参 数：  无
 * 返回值： 无
 * 类 别：UDP
 *********************************************************************/
void UDP::on_btnBroadcast_clicked()
{
    quint16     targetPort=ui->spinTargetPort->value();   //目标端口
    QString  msg=ui->editMsg->text();
    QByteArray  str=msg.toUtf8();
    udpSocket->writeDatagram(str,QHostAddress::Broadcast,targetPort);

    ui->textEdit->appendPlainText("[broadcast] "+msg);
    ui->editMsg->clear();
    ui->editMsg->setFocus();
}



void UDP::on_BTOPEN_clicked()
{
    camera.open(0);
    udp_timer->start(33);
    movie->stop();
}

void UDP::on_BTCLOSE_clicked()
{
    udp_timer->stop();
    camera.release();
    //重新开启加载图片
    ui->label_VIDEO->setMovie(movie);
    movie->start();
    //this->close();
}

