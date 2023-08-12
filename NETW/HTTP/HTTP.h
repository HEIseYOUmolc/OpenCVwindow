#ifndef HTTP_H
#define HTTP_H

#include    <QMainWindow>
#include    <QNetworkAccessManager>
#include    <QNetworkReply>
#include    <QFile>
#include    <QUrl>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class HTTP; }
QT_END_NAMESPACE

class HTTP : public QMainWindow
{
    Q_OBJECT
private:
    QNetworkAccessManager networkManager;   //网络管理
    QNetworkReply *reply;   //网络响应
    QFile *downloadedFile;  //下载保存的临时文件
public:
    HTTP(QWidget *parent = nullptr);
    ~HTTP();

private slots:
    //自定义槽函数
    void do_finished();
    void do_readyRead();
    void do_downloadProgress(qint64 bytesRead, qint64 totalBytes);


    void on_btnDefaultPath_clicked();

    void on_btnDownload_clicked();

    void on_editURL_textChanged(const QString &arg1);

    void on_btnChosePath_clicked();

private:
    Ui::HTTP *ui;
};

#endif // HTTP_H
