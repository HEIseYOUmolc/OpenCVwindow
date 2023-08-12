#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_netw.h"
//借用UI界面而引用
#include "opencv.h"
#include "screenwidget.h"
#include "gifwidget.h"


std::string model_name;
std::string class_name;
/*********************************************************************
 * 函数功能：设定QT界面
 * 参 数：QWidget *parent
 * 返回值： 空
 * 类 别：mainwindow主函数完成UI界面的初始化
 *********************************************************************/
mainwindow::mainwindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::mainwindow)
{
    ui->setupUi(this);
    ui_init(ui);//来自opencv的ui界面借用
    ui_sinit(ui);//来自screenwidget的ui界面借用
    ui_ginit(ui);//来自gifwidget的ui界面借用
    //初始化
    on_btnSerialCheck_clicked();
    StatusInit();
}
mainwindow::~mainwindow()
{
    delete ui;
}
/*********************************************************************
 * 函数功能：（自设函数）构建状态栏初始化,定时器功能设定
 * 参 数：  无
 * 返回值： 无
 * 备  注： UI界面状态栏初始化，使用的定时器初始化
 *********************************************************************/
void mainwindow::StatusInit()
{
    //定时器初始化
    QTimer *DateTimer = new QTimer(this);//状态栏显示时间，日期
    connect(DateTimer,&QTimer::timeout,this,[=](){TimeUpdate();});
    DateTimer->start(1000); //每隔1000ms发送timeout的信号

    ShowTimeLabel = new QLabel(this);
    lblSendSum = new QLabel(this);
    lblRecvSum = new QLabel(this);
    lblSaveSum = new QLabel(this);//创建标签对象

    lblSendSum->setMinimumSize(60, 20);
    lblRecvSum->setMinimumSize(60, 20);
    lblSaveSum->setMinimumSize(80, 20);
    ShowTimeLabel->setMinimumSize(120, 20);//设置标签大小

    CvSaveSum = 0;
    on_clearBt_clicked();   //初始化标签


    statusBar()->addPermanentWidget(lblSaveSum);
    statusBar()->addPermanentWidget(lblSendSum);
    statusBar()->addPermanentWidget(lblRecvSum);
    statusBar()->addPermanentWidget(ShowTimeLabel);//从右往左加入状态栏


    //设定定时器，编辑框变动触发一次事件，调整TextEdit光标位置
    connect(ui->recvEdit, &QPlainTextEdit::textChanged,[=]()
    {
        //保持编辑器光标在最后一行
        QTextCursor cursor=ui->recvEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->recvEdit->setTextCursor(cursor);
    });

    ori_timer = new QTimer(this);
    connect(ori_timer, SIGNAL(timeout()), this, SLOT(play_ori_Timer()));

    inf_timer = new QTimer(this);
    connect(inf_timer, SIGNAL(timeout()), this, SLOT(play_inf_Timer()));

    ui->label_origin->clear();//关闭图像  
    ui->label_inference->clear();//关闭图像
}
/*********************************************************************
 * 函数功能：（自设函数）基础刷新时间功能
 * 参 数：  无
 * 返回值： 无
 * 类 别：时间
 *********************************************************************/
void mainwindow::TimeUpdate()
{
    QDateTime CurrentTime=QDateTime::currentDateTime();
    QString strTime=CurrentTime.toString(" yyyy年MM月dd日 hh:mm:ss "); //设置显示的格式
    ShowTimeLabel->setText(strTime);
}
/*********************************************************************
 * 函数功能：刷新串口
 * 参 数：  无
 * 返回值： 无
 * 类 别：UI界面btnSerialCheck按钮
 *********************************************************************/
void mainwindow::on_btnSerialCheck_clicked()
{
    ui->serailCb->clear();              //清空串口
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->serailCb->addItem(info.portName()); //搜素串口并加入选择框内
    }
}
/*********************************************************************
 * 函数功能：打开串口
 * 参 数：  无
 * 返回值： 无
 * 类 别：UI界面openBt按钮
 *********************************************************************/
void mainwindow::on_openBt_clicked()
{
    c_serial.setPortName(ui->serailCb->currentText());
    c_serial.setBaudRate(ui->baundrateCb->currentText().toInt());
    //选择校验位
    switch(ui->checkbitCb->currentIndex())
    {
    case 0: c_serial.setParity(QSerialPort::NoParity);break;
    case 1: c_serial.setParity(QSerialPort::OddParity);break;
    case 2: c_serial.setParity(QSerialPort::EvenParity);break;
    case 3: c_serial.setParity(QSerialPort::SpaceParity);break;
    case 4: c_serial.setParity(QSerialPort::MarkParity);break;
    default: break;
    }
    //选择数据位
    switch(ui->databitCb->currentText().toInt())
    {
    case 5: c_serial.setDataBits(QSerialPort::Data5);break;
    case 6: c_serial.setDataBits(QSerialPort::Data6);break;
    case 7: c_serial.setDataBits(QSerialPort::Data7);break;
    case 8: c_serial.setDataBits(QSerialPort::Data8);break;
    }
    //选择停止位
    switch(ui->stopbitCb->currentIndex())
    {
    case 0: c_serial.setStopBits(QSerialPort::OneStop);break;
    case 1:
        QMessageBox::information(this,"提示","1.5默认数据5位!");
        c_serial.setStopBits(QSerialPort::OneAndHalfStop);
        c_serial.setDataBits(QSerialPort::Data5);
        break;
    case 2: c_serial.setStopBits(QSerialPort::TwoStop);break;
    }

    c_serial.open(QIODeviceBase::ReadWrite);

    if(c_serial.isOpen())
    {
        connect(&c_serial,&QSerialPort::readyRead,this,&mainwindow::messlot);
        ui->openBt->setEnabled(false);
    }
    else
    {
        QMessageBox::information(this,"提示","串口打开失败!");
    }
}
/*********************************************************************
 * 函数功能：（自设函数）串口接收信息的处理函数
 * 参 数：  无
 * 返回值： 无
 * 类 别：串口
 *********************************************************************/
void mainwindow::messlot()
{
    QString StrTemp,StrTemp1,StrTimeDate;
    //停止接收延时定时器，读取串口接收到的数据，并格式化数据
    //recvDelayTimer->stop();
    QByteArray ComRecvBUff=c_serial.readAll();//接收数据缓存
    StrTemp = QString::fromLocal8Bit(ComRecvBUff);

    //接收流量统计，并显示到状态栏
    ComRecvSum += ComRecvBUff.size();
    setNumOnLabel(lblRecvSum, "Rx:", ComRecvSum);

    //获取串口数据接收的系统时间，备后续使用
    QDateTime curDateTime=QDateTime::currentDateTime();
    StrTimeDate = curDateTime.toString("[yyyy-MM-dd hh:mm:ss.zzz] ");

    if(ui->chkHexDisplay->checkState() != false)//16进制模式
    {
        StrTemp =  ComRecvBUff.toHex().toUpper();//转换为16进制数，并大写

        for(int i = 0; i<StrTemp.length (); i+=2)//整理字符串，即添加空格
        {
            StrTemp1 += StrTemp.mid (i,2);
            StrTemp1 += " ";
        }
        if(ui->chkTimRecv->checkState() != false)
        {
            StrTemp1.prepend(StrTimeDate);//前面添加时间
        }
        if(ui->chkLineSend->checkState() != false)
        {
            StrTemp1.append("\x0a");// 后面添加换行
        }

        StrTemp1.prepend("Rx:");//添加接收提示信息
        ui->recvEdit->insertPlainText(StrTemp1);
    }

    else//正常显示
    {
        if(ui->chkTimRecv->checkState() != false)
        {
            StrTemp.prepend(StrTimeDate);//前面添加时间
        }
        if(ui->chkLineSend->checkState() != false)
        {
            StrTemp.append("\n");//后面添加换行
        }
        StrTemp.prepend("Rx:");//添加接收提示信息
        ui->recvEdit->insertPlainText(StrTemp);
    }
}
/*********************************************************************
 * 函数功能：关闭串口
 * 参 数：  无
 * 返回值： 无
 * 类 别：UI界面closeBt按钮
 *********************************************************************/
void mainwindow::on_closeBt_clicked()
{
    c_serial.close();
    if(c_serial.isOpen())
    {
        QMessageBox::information(this,"提示","串口关闭失败!");
    }
    else
    {
        ui->openBt->setEnabled(true);
    }
}
/*********************************************************************
 * 函数功能：（自设函数）添加数字到标签
 * 参 数：  无
 * 返回值： 无
 * 类 别：串口
 *********************************************************************/
void mainwindow::setNumOnLabel(QLabel *lbl, QString strS, long num)
{
    QString strN,str;// 标签显示
    strN= QString("%1").arg(num);
    str = strS + strN;
    lbl->setText(str);
}
/*********************************************************************
 * 函数功能：清除接收信息，信息流统计
 * 参 数：  无
 * 返回值： 无
 * 类 别：UI界面clearBt按钮
 *********************************************************************/
void mainwindow::on_clearBt_clicked()
{
    ui->recvEdit->clear();

    ComSendSum = 0;
    ComRecvSum = 0;


    setNumOnLabel(lblSaveSum, "Save:", CvSaveSum);
    setNumOnLabel(lblSendSum, "Tx:", ComSendSum);
    setNumOnLabel(lblRecvSum, "Rx:", ComRecvSum);
}
/*********************************************************************
 * 函数功能：串口发送信息的处理函数
 * 参 数：  无
 * 返回值： 无
 * 类 别：UI界面sendBt按钮
 *********************************************************************/
void mainwindow::on_sendBt_clicked()
{
    QString StrTemp,StrTimeDate;
    QByteArray ComSendBUff;
    QDateTime curDateTime=QDateTime::currentDateTime();
    StrTimeDate = curDateTime.toString("[yyyy-MM-dd hh:mm:ss.zzz] ");
    StrTemp = ui->sendEdit->toPlainText();

    if(c_serial.isOpen())//判断打开串口
    {
        if(ui->chkHexSend->checkState() != false)//16进制模式
        {
            ComSendBUff = QByteArray::fromHex(StrTemp.toUtf8()).data();
            if(ui->chkLineSend->checkState() != false)
            {
                StrTemp.append("\n");//后面添加换行
                ComSendBUff.append("\x0a");//后面添加换行
            }
            if(ui->chkTimRecv->checkState() != false)
            {
                StrTemp.prepend(StrTimeDate);//前面添加时间
            }
            ComSendSum +=  ComSendBUff.size();
            setNumOnLabel(lblSendSum, "Tx:", ComSendSum);
            c_serial.write(ComSendBUff);
            StrTemp.prepend("Tx:");
            ui->recvEdit->insertPlainText(StrTemp);
        }
        else
        {
            if(ui->chkLineSend->checkState() != false)
            {
                StrTemp.append("\n");//后面添加换行
            }
            if(ui->chkTimRecv->checkState() != false)
            {
                StrTemp.prepend(StrTimeDate);//前面添加时间
            }
            ComSendBUff = StrTemp.toLocal8Bit().data();
            ComSendSum +=  ComSendBUff.size();
            setNumOnLabel(lblSendSum, "Tx:", ComSendSum);
            c_serial.write(ComSendBUff);

            StrTemp.prepend("Tx:");//添加发送提示信息
            ui->recvEdit->insertPlainText(StrTemp);
        }
    }
    else
    {
        QMessageBox::information(this,"提示","串口尚未打开！");
    }
}
/*********************************************************************
 * 函数功能：打开数据库
 * 参 数：  无
 * 返回值： 无
 * 类 别：UI界面BT_DATABASE按钮
 *********************************************************************/

void mainwindow::on_BT_DATABASE_clicked()
{
    QSL *qsl = new QSL;
    qsl->show();
}

