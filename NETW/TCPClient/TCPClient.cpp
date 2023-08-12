#include "TCPClient.h"
#include "ui_TCPClient.h"

#include    <QHostAddress>
#include    <QHostInfo>

QString TCPClient::getLocalIP()
{
    QString hostName=QHostInfo::localHostName();  //本地主机名
    QHostInfo   hostInfo=QHostInfo::fromName(hostName);
    QString   localIP="";
    QList<QHostAddress> addList=hostInfo.addresses();
    if (addList.isEmpty())
        return localIP;

    foreach(QHostAddress aHost, addList)
        if (QAbstractSocket::IPv4Protocol==aHost.protocol())
        {
            localIP=aHost.toString();
            break;
        }
    return localIP;
}

TCPClient::TCPClient(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TCPClient)
{
    ui->setupUi(this);

    tcpClient=new QTcpSocket(this);    //创建socket变量
    tcp_timer = new QTimer(this);
    connect(tcp_timer, SIGNAL(timeout()), this, SLOT(VideoSend()));

    labSocketState=new QLabel("Socket状态：");  //状态栏标签
    labSocketState->setMinimumWidth(250);
    ui->statusBar->addWidget(labSocketState);

    QString localIP=getLocalIP();      //获取本机IP
    this->setWindowTitle(this->windowTitle()+"----本机IP地址："+localIP);
    ui->comboServer->addItem(localIP);


    connect(tcpClient,SIGNAL(connected()),   this,SLOT(do_connected()));
    connect(tcpClient,SIGNAL(disconnected()),this,SLOT(do_disconnected()));

    connect(tcpClient,&QTcpSocket::stateChanged,this,&TCPClient::do_socketStateChange);

    connect(tcpClient,SIGNAL(readyRead()),   this,SLOT(do_socketReadyRead()));
    movie = new QMovie(":/images/images/loading.gif");
    ui->label_video->setMovie(movie); // 1. 设置要显示的 GIF 动画图片
    movie->start();              // 2. 启动动画
}

TCPClient::~TCPClient()
{
    delete ui;
}
void TCPClient::on_pushButton_clicked()
{
    camera.open(0);
    tcp_timer->start(33);
    ui->pushButton->setEnabled(false);
}


void TCPClient::VideoSend()
{
    if (!tcpClient->isOpen()) {
        on_actConnect_triggered();//连接服务端
        if (!tcpClient->waitForConnected(5000)) {
            qDebug() << "Error: Could not connect to the server.";
            return;
        }
    }

    cv::Mat frame;
    camera>>(frame);
    cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    QByteArray byte;
    QBuffer buff(&byte);
    QImage image((unsigned char *)(frame.data), frame.cols, frame.rows, QImage::Format_RGB888);
    image.save(&buff, "JPEG");
    QByteArray ss=qCompress(byte,1);
    QByteArray vv=ss.toBase64();

    // Send the compressed image data via TCP
    // QDataStream dataStream(tcpClient);
    QByteArray ba;
    QDataStream out(&ba,QIODevice::WriteOnly);
    //QByteArray compressedData = qCompress(byte, 1);
    out.setVersion(QDataStream::Qt_6_5);

    out<<(quint64)0;
    out<<vv;
    out.device()->seek(0);
    out<<(quint64)(ba.size()-sizeof(quint64));
    tcpClient->write(ba);
    ui->label_video->setPixmap(QPixmap::fromImage(image));
    ui->label_video->resize(image.size());

    update();
}
void TCPClient::do_connected()
{ //connected()信号的槽函数
    ui->textEdit->appendPlainText("**已连接到服务器");
    ui->textEdit->appendPlainText("**peer address:"+
                                   tcpClient->peerAddress().toString());
    ui->textEdit->appendPlainText("**peer port:"+
                                   QString::number(tcpClient->peerPort()));
    ui->actConnect->setEnabled(false);
    ui->actDisconnect->setEnabled(true);
}

void TCPClient::do_disconnected()
{//disConnected()信号的槽函数
    ui->textEdit->appendPlainText("**已断开与服务器的连接");
    ui->actConnect->setEnabled(true);
    ui->actDisconnect->setEnabled(false);
}

void TCPClient::do_socketReadyRead()
{//readyRead()信号的槽函数
    while(tcpClient->canReadLine())
        ui->textEdit->appendPlainText("[in] "+tcpClient->readLine());
}

void TCPClient::do_socketStateChange(QAbstractSocket::SocketState socketState)
{//stateChange()信号的槽函数
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

void TCPClient::on_actConnect_triggered()
{//“连接服务器”按钮
    QString   addr=ui->comboServer->currentText();
    quint16   port=ui->spinPort->value();
    tcpClient->connectToHost(addr,port);
    //    tcpClient->connectToHost(QHostAddress::LocalHost,port);
}

void TCPClient::on_actDisconnect_triggered()
{//“断开连接”按钮
    if (tcpClient->state()==QAbstractSocket::ConnectedState)
        tcpClient->disconnectFromHost();
}

void TCPClient::on_actClear_triggered()
{
    ui->textEdit->clear();
}

void TCPClient::on_btnSend_clicked()
{//“发送数据”按钮
    QString  msg=ui->editMsg->text();
    ui->textEdit->appendPlainText("[out] "+msg);
    ui->editMsg->clear();
    ui->editMsg->setFocus();

    QByteArray  str=msg.toUtf8();
    str.append('\n');
    tcpClient->write(str);
}


void TCPClient::on_pushButton_2_clicked()
{
    tcpClient->disconnectFromHost();
    tcp_timer->stop();
    camera.release();
    //重新开启加载图片
    ui->label_video->setMovie(movie);
    movie->start();
    ui->pushButton->setEnabled(true);
}

