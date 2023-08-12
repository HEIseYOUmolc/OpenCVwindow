#include "scan.h"
#include "ui_scan.h"
#include "QSL.h"
#include  "tdialogdata.h"

extern int numRows;
scan::scan(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::scan)
{
    ui->setupUi(this);
    // 创建一个查询模型，并执行查询
    QSqlQueryModel *model = new QSqlQueryModel(this);
    model->setQuery("SELECT CropNo FROM CropDieases ORDER BY CropNo");

    ui->cropComboBox->setModel(model);
    ui->cropComboBox->setModelColumn(0);
}

scan::~scan()
{
    delete ui;
}
/*********************************************************************
 * 函数功能：设置要查询的numRows
 * 参 数：空
 * 返回值： 空
 * 类 别：QSL
 *********************************************************************/
void scan::on_researchbutton_clicked()
{
    QSL qsl;
    int targetCropNo = ui->cropComboBox->currentText().toInt();
    QSqlQuery qrySelcet;
    qrySelcet.prepare("SELECT rowid FROM CropDieases WHERE CropNo = :targetCropNo");
    qrySelcet.bindValue(":targetCropNo", targetCropNo);
    qrySelcet.exec();
    qrySelcet.next();
    if( targetCropNo == 1001)
    {
        numRows=0;
    }
    else{
        numRows=qrySelcet.value(0).toInt()-1;
    }
    //获取数值，显示信息
    this->close();
    //qsl.updateRecord(numRows);
}

void scan::on_cancelbutton_clicked()
{
    this->close();
}

