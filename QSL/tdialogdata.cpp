#include "tdialogdata.h"
#include "ui_tdialogdata.h"

#include <QFileDialog>

TDialogData::TDialogData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TDialogData)
{
    ui->setupUi(this);
}

TDialogData::~TDialogData()
{
    delete ui;
}

//编辑记录，更新记录数据到界面
void TDialogData::setUpdateRecord(QSqlRecord &recData)
{
    m_record=recData;    //记录存入私有变量
    ui->spinCropNo->setEnabled(false);   //员工编号不允许编辑
    setWindowTitle("更新记录");

    //根据recData的数据更新界面显示
    ui->spinCropNo->setValue(recData.value("CropNo").toInt());
    ui->editName->setText(recData.value("CropName").toString());
    ui->editEName->setText(recData.value("CropEName").toString());
    ui->editOName->setText(recData.value("CropOName").toString());
    ui->editDisPro->setText(recData.value("DisPro").toString());
    ui->editPreMeth->setText(recData.value("PreMeth").toString());
    QString browser_url=recData.value("GallLink").toString();
    //第一个browser_url为打开的网址，第二个browser_url为显示的网址
    ui->editGallLink->setText(browser_url);
    ui->editGallLink_go->setText("<a style='color: green;'href=\""+browser_url+"\">go");
    ui->editMemo->setPlainText(recData.value("Memo").toString());

    QVariant    va=recData.value("Photo");
    if (!va.isValid())  //图片字段内容为空
        ui->LabPhoto->clear();
    else    //显示图片
    {
        QByteArray data=va.toByteArray();
        QPixmap pic;
        pic.loadFromData(data);
        ui->LabPhoto->setPixmap(pic.scaledToWidth(ui->LabPhoto->size().width()));
    }
}


//插入记录，无需更新界面显示，但是要存储recData的字段结构
void TDialogData::setInsertRecord(QSqlRecord &recData)
{
    m_record=recData;    //保存recData到私有变量
    ui->spinCropNo->setEnabled(true);    //插入的记录，员工编号允许编辑
    setWindowTitle("插入新记录");
    ui->spinCropNo->setValue(recData.value("CropNo").toInt());
}

//点击"确定"按钮后，界面数据保存到记录变量mRecord
QSqlRecord TDialogData::getRecordData()
{
    m_record.setValue("CropNo",      ui->spinCropNo->value());
    m_record.setValue("CropName",    ui->editName->text());
    m_record.setValue("CropEName",   ui->editEName->text());
    m_record.setValue("CropOName",   ui->editOName->text());

    m_record.setValue("DisPro",      ui->editDisPro->toPlainText());
    m_record.setValue("PreMeth",     ui->editPreMeth->toPlainText());

    m_record.setValue("GallLink",    ui->editGallLink->toPlainText());

    m_record.setValue("Memo",        ui->editMemo->toPlainText());
    //照片编辑时已经修改了mRecord的photo字段的值

    return  m_record; //以记录作为返回值
}

void TDialogData::on_btnClearPhoto_clicked()
{ //清除照片
    ui->LabPhoto->clear();
    m_record.setNull("Photo");   //Photo字段清空
}

void TDialogData::on_btnSetPhoto_clicked()
{//设置照片
    QString aFile=QFileDialog::getOpenFileName(this,"选择图片文件","", "照片(*.jpg)");
    if (aFile.isEmpty())
        return;

    QByteArray data;
    QFile* file=new QFile(aFile);
    file->open(QIODevice::ReadOnly);
    data = file->readAll();
    file->close();

    m_record.setValue("Photo",data); //图片保存到Photo字段

    QPixmap pic;
    pic.loadFromData(data);
    ui->LabPhoto->setPixmap(pic.scaledToWidth(ui->LabPhoto->size().width()));
}
