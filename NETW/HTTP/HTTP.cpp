#include "HTTP.h"
#include "ui_HTTP.h"
#include    <QDir>
#include    <QMessageBox>
#include    <QDesktopServices>

/*********************************************************************
 * 函数功能：完成ui界面设置
 * 参 数：  无
 * 返回值： 无
 * 类 别：HTTP
 *********************************************************************/
HTTP::HTTP(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HTTP)
{
    ui->setupUi(this);
    ui->editURL->setClearButtonEnabled(true);
}

HTTP::~HTTP()
{
    delete ui;
}
/*********************************************************************
 * 函数功能：网络响应结束
 * 参 数：  无
 * 返回值： 无
 * 类 别：HTTP
 *********************************************************************/
void HTTP::do_finished()
{
    QFileInfo fileInfo(downloadedFile->fileName());   //为了获取下载后的文件名

    downloadedFile->close();
    delete downloadedFile;   //删除临时文件对象
    reply->deleteLater(); //由主事件循环删除此对象

    if (ui->chkBoxOpen->isChecked())    //打开下载的文件
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
    ui->btnDownload->setEnabled(true);
}
/*********************************************************************
 * 函数功能：读取下载的数据
 * 参 数：  无
 * 返回值： 无
 * 类 别：HTTP
 *********************************************************************/
void HTTP::do_readyRead()
{
    downloadedFile->write(reply->readAll());
}
/*********************************************************************
 * 函数功能：下载进度
 * 参 数：  无
 * 返回值： 无
 * 类 别：HTTP
 *********************************************************************/
void HTTP::do_downloadProgress(qint64 bytesRead, qint64 totalBytes)
{
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(bytesRead);
}
/*********************************************************************
 * 函数功能："选择路径"  按钮
 * 参 数：  无
 * 返回值： 无
 * 类 别：HTTP
 *********************************************************************/
void HTTP::on_btnChosePath_clicked()
{
    QString selectedPath = QFileDialog::getExistingDirectory(nullptr, "选择路径", QDir::currentPath());
    QDir    dir(selectedPath);
    QString  sub="resources";
    dir.mkdir(sub);
    ui->editPath->setText(selectedPath+"/"+sub+"/");
}

/*********************************************************************
 * 函数功能："默认路径"  按钮
 * 参 数：  无
 * 返回值： 无
 * 类 别：HTTP
 *********************************************************************/
void HTTP::on_btnDefaultPath_clicked()
{
    QString  curPath=QDir::currentPath();
    QDir    dir(curPath);
    QString  sub="resources";
    dir.mkdir(sub);
    ui->editPath->setText(curPath+"/"+sub+"/");
}
/*********************************************************************
 * 函数功能："下载"按钮，开始下载
 * 参 数：  无
 * 返回值： 无
 * 类 别：HTTP
 *********************************************************************/
void HTTP::on_btnDownload_clicked()
{
    QString urlSpec = ui->editURL->text().trimmed();
    if (urlSpec.isEmpty())
    {
        QMessageBox::information(this, "错误","请指定需要下载的URL");
        return;
    }
    QUrl newUrl = QUrl::fromUserInput(urlSpec);  //URL地址
    if (!newUrl.isValid())
    {
        QString info="无效URL:"+urlSpec+"\n错误信息:"+newUrl.errorString();
        QMessageBox::information(this, "错误",info);
        return;
    }
    QString tempDir =ui->editPath->text().trimmed();    //临时目录
    if (tempDir.isEmpty())
    {
        QMessageBox::information(this, "错误", "请指定保存下载文件的目录");
        return;
    }

    QString fullFileName =tempDir+newUrl.fileName(); //文件名
    if (QFile::exists(fullFileName))
        QFile::remove(fullFileName);

    downloadedFile =new QFile(fullFileName);        //创建临时文件
    if (!downloadedFile->open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this, "错误","临时文件打开错误");
        return;
    }

    ui->btnDownload->setEnabled(false);
    //发送网络请求，创建网络响应
    reply = networkManager.get(QNetworkRequest(newUrl));
    connect(reply, SIGNAL(readyRead()), this, SLOT(do_readyRead()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(do_downloadProgress(qint64,qint64)));
    connect(reply, SIGNAL(finished()), this, SLOT(do_finished()));
}
/*********************************************************************
 * 函数功能："编辑URL"按钮
 * 参 数：  无
 * 返回值： 无
 * 类 别：HTTP
 *********************************************************************/
void HTTP::on_editURL_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);
}




