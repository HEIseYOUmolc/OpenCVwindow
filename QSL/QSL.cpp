#include "QSL.h"
#include "scan.h"
#include "ui_QSL.h"
extern int numRows;
bool get_numRows_flag=false;
int numRows=0;
/*********************************************************************
 * 函数功能：选择数据库
 * 参 数：空
 * 返回值： 空
 * 类 别：QSL
 *********************************************************************/
void QSL::selectData()
{
    qryModel=new QSqlQueryModel(this);
    selModel=new QItemSelectionModel(qryModel,this);

    ui->tableView->setModel(qryModel);
    ui->tableView->setSelectionModel(selModel);

    qryModel->setQuery("SELECT CropNo,CropName,CropEName,CropOName,DisPro,PreMeth, GallLink,"
                       "Memo FROM CropDieases order by CropNo"
                       );
    if (qryModel->lastError().isValid())
    {
        QMessageBox::information(this, "错误", "数据表查询错误,错误信息\n"
                                 +qryModel->lastError().text());
        return;
    }

    QSqlRecord rec=qryModel->record();  //获取空记录，用于获取字段序号

    //    connect(theSelection,&QItemSelectionModel::currentRowChanged,
    //            this, &MainWindow::do_currentRowChanged);

    //设置字段显示标题
    //根据农业部令
    //第二十条　农业部将品种登记信息进行公告，公告内容包括：登记编号、作物种类、品种名称、申请者、育种者、品种来源、特征特性、品质、抗性、产量、栽培技术要点、适宜种植区域及季节等。
    //登记编号格式为：GPD+作物种类+（年号）+2位数字的省份代号+4位数字顺序号。
    qryModel->setHeaderData(rec.indexOf("CropNo"),       Qt::Horizontal, "病虫害编号");
    qryModel->setHeaderData(rec.indexOf("CropName"),     Qt::Horizontal, "病虫害名称");
    qryModel->setHeaderData(rec.indexOf("CropEName"),    Qt::Horizontal, "英文名称");
    qryModel->setHeaderData(rec.indexOf("CropOName"),    Qt::Horizontal, "其他名称");
    qryModel->setHeaderData(rec.indexOf("DisPro"),       Qt::Horizontal, "病虫害简介");
    qryModel->setHeaderData(rec.indexOf("PreMeth"),      Qt::Horizontal, "防止方法");
    qryModel->setHeaderData(rec.indexOf("GallLink"),     Qt::Horizontal, "图库链接");
    qryModel->setHeaderData(rec.indexOf("Memo"),         Qt::Horizontal, "备注");   


    ui->actOpenDB->setEnabled(false);
    ui->actRecInsert->setEnabled(true);
    ui->actRecDelete->setEnabled(true);
    ui->actRecEdit->setEnabled(true);
    ui->actScan->setEnabled(true);
    ui->actFields->setEnabled(true);
}
/*********************************************************************
 * 函数功能：更新/修改一条记录
 * 参 数：int recNo
 * 返回值： 空
 * 类 别：QSL
 *********************************************************************/
void QSL::updateRecord(int recNo)
{

    QSqlRecord  curRec=qryModel->record(recNo); //获取数据模型的一条记录

    int CropNo=curRec.value("CropNo").toInt();    //获取CropNo

    QSqlQuery query;
    query.prepare("select * from CropDieases where CropNo = :ID");
    query.bindValue(":ID",CropNo);
    query.exec();
    query.first();
    if (!query.isValid())   //无有效记录
        return;

    curRec=query.record();  //获取当前记录
    TDialogData    *dataDialog=new TDialogData(this); //创建对话框
    Qt::WindowFlags    flags=dataDialog->windowFlags();
    dataDialog->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint); //对话框固定大小

    dataDialog->setUpdateRecord(curRec);    //更新对话框的数据和界面
    int ret=dataDialog->exec();
    if (ret==QDialog::Accepted)
    {
        QSqlRecord  recData=dataDialog->getRecordData();    //获得对话框返回的记录

        query.prepare("update CropDieases set CropName=:CropName, CropEName=:CropEName,"
                      " CropOName=:CropOName,  DisPro=:DisPro,"
                      " PreMeth=:PreMeth,GallLink=:GallLink ,"
                      " Memo=:Memo,Photo=:Photo"
                      " where CropNo = :ID");

        query.bindValue(":CropName",recData.value("CropName"));
        query.bindValue(":CropEName",  recData.value("CropEName"));
        query.bindValue(":CropOName",recData.value("CropOName"));
        query.bindValue(":DisPro",recData.value("DisPro"));
        query.bindValue(":PreMeth",  recData.value("PreMeth"));
        query.bindValue(":GallLink",recData.value("GallLink"));
        query.bindValue(":Memo",   recData.value("Memo"));
        query.bindValue(":Photo",   recData.value("Photo"));

        query.bindValue(":ID",      CropNo);

        if (!query.exec())
            QMessageBox::critical(this, "错误", "记录更新错误\n"+query.lastError().text());
        else
        {
            QString sqlStr=qryModel->query().executedQuery();   //执行过的SELECT语句
            qryModel->setQuery(sqlStr);//数据模型重新查询数据，更新tableView显示
        }
    }

    delete dataDialog;      //删除对话框
}
/*********************************************************************
 * 函数功能：UI界面初始化
 * 参 数：QWidget *parent
 * 返回值： 空
 * 类 别：UI界面
 *********************************************************************/
QSL::QSL(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QSL)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->tableView);

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);      //不能编辑
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);     //行选择
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);    //单行选择
    ui->tableView->setAlternatingRowColors(true);
}

QSL::~QSL()
{
    delete ui;
}
/*********************************************************************
 * 函数功能：添加数据库的驱动
 * 参 数：int recNo
 * 返回值： 空
 * 类 别：QSL
 *********************************************************************/
bool QSL::openDatabase(QString aFile)
{
    DB=QSqlDatabase::addDatabase("QSQLITE"); //添加 SQLITE数据库驱动
    DB.setDatabaseName(aFile);  //设置数据库文件
    if (DB.open())
    {
        selectData();
        return true;
    }
    else
        return false;
}

/*void MainWindow::do_currentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    if (!current.isValid())
    {
        ui->statusBar->clearMessage();
        return;
    }

    int curRecNo=theSelection->currentIndex().row();
    QSqlRecord  curRec=qryModel->record(curRecNo);  //获取当前记录
    int empNo=curRec.value("EmpNo").toInt();
    QString name=curRec.value("Name").toString();

    ui->statusBar->showMessage(QString("当前记录，工号=%1 , 姓名=%2").arg(empNo).arg(name));
}*/
/*********************************************************************
 * 函数功能：从文件中添加数据库数据库
 * 参 数：空
 * 返回值： 空
 * 类 别：UI界面OpenDB
 *********************************************************************/
void QSL::on_actOpenDB_triggered()
{
    QString aFile=QFileDialog::getOpenFileName(this,"选择文件","","SQLite数据库(*.db3)");
    if (aFile.isEmpty())
        return;

    DB=QSqlDatabase::addDatabase("QSQLITE"); //添加 SQLITE数据库驱动
    DB.setDatabaseName(aFile);  //设置数据库文件
    if (DB.open())
        selectData();
    else
        QMessageBox::warning(this, "错误", "打开数据库失败");
}
/*********************************************************************
 * 函数功能：从数据库中添加记录
 * 参 数：空
 * 返回值： 空
 * 类 别：UI界面
 *********************************************************************/
void QSL::on_actRecInsert_triggered()
{
    QSqlQuery query;
    query.exec("select * from CropDieases where CropNo =-1");   //实际查不出记录，只查询字段信息

    QSqlRecord curRec=query.record();   //获取当前记录,实际为空记录
    curRec.setValue("CropNo",qryModel->rowCount()+2000);

    TDialogData    *dataDialog=new TDialogData(this);
    Qt::WindowFlags    flags=dataDialog->windowFlags();
    dataDialog->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint); //对话框固定大小
    dataDialog->setInsertRecord(curRec); //插入记录

    int ret=dataDialog->exec();
    if (ret==QDialog::Accepted)
    {
        QSqlRecord  recData=dataDialog->getRecordData();
        query.prepare("INSERT INTO CropDieases (CropNo,CropName,CropEName,CropOName,DisPro,"
                      " PreMeth,GallLink,Memo,Photo) "
                      " VALUES(:CropNo,:CropName, :CropEName,:CropOName,:DisPro,"
                      " :PreMeth,:GallLink,:Memo,:Photo)");

        query.bindValue(":CropNo",recData.value("CropNo"));
        query.bindValue(":CropName",recData.value("CropName"));
        query.bindValue(":CropEName",recData.value("CropEName"));
        query.bindValue(":CropOName",recData.value("CropOName"));
        query.bindValue(":DisPro",recData.value("DisPro"));
        query.bindValue(":PreMeth",recData.value("PreMeth"));
        query.bindValue(":GallLink",recData.value("GallLink"));
        query.bindValue(":Memo",recData.value("Memo"));
        query.bindValue(":Photo",recData.value("Photo"));

        if (!query.exec())
            QMessageBox::critical(this, "错误", "插入记录错误\n"+query.lastError().text());
        else //插入，删除记录后需要重新设置SQL语句查询
        {
            QString sqlStr=qryModel->query().executedQuery();   //执行过的SELECT语句
            qryModel->setQuery(sqlStr);         //重新查询数据
        }
    }

    delete dataDialog;
}

/*********************************************************************
 * 函数功能：从数据库中删除数据
 * 参 数：空
 * 返回值： 空
 * 类 别：UI界面actRecDelete
 *********************************************************************/
void QSL::on_actRecDelete_triggered()
{
    int curRecNo=selModel->currentIndex().row();
    QSqlRecord  curRec=qryModel->record(curRecNo); //获取当前记录
    if (curRec.isEmpty()) //当前为空记录
        return;

    int CropNo=curRec.value("CropNo").toInt();    //获取员工编号
    QSqlQuery query;
    query.prepare("delete  from CropDieases where CropNo = :ID");
    query.bindValue(":ID",CropNo);

    if (!query.exec())
        QMessageBox::critical(this, "错误", "删除记录出现错误\n"+query.lastError().text());
    else //插入，删除记录后需要重新设置SQL语句查询
    {
        QString sqlStr=qryModel->query().executedQuery();//  执行过的SELECT语句
        qryModel->setQuery(sqlStr);         //重新查询数据
    }
}

/*********************************************************************
 * 函数功能：从数据库中修改数据
 * 参 数：空
 * 返回值： 空
 * 类 别：UI界面actRecEdit
 *********************************************************************/
void QSL::on_actRecEdit_triggered()
{//编辑当前记录
    int curRecNo=selModel->currentIndex().row();
    updateRecord(curRecNo);
}
/*********************************************************************
 * 函数功能：更新框框的信息
 * 参 数：空
 * 返回值： 空
 * 类 别：UI界面actRecEdit
 *********************************************************************/
void QSL::on_tableView_doubleClicked(const QModelIndex &index)
{ //tableView上双击,编辑当前记录
    int curRecNo=index.row();
    updateRecord(curRecNo);
}

/*********************************************************************
 * 函数功能：从数据库中遍历数据,找出目标记录
 * 参 数：空
 * 返回值： 空
 * 类 别：UI界面actScan
 *********************************************************************/
void QSL::on_actScan_triggered()
{
    if(get_numRows_flag==true)
    {
        updateRecord(numRows);
        get_numRows_flag=false;
    }
    else
    {
        scan *SCAN = new scan;
        SCAN->show();
        get_numRows_flag=true;
    }
}

/*
void QSL::on_actSQLtest_triggered()
{
    ////3. 使用addBindValue()
    //    QSqlQuery query;
    //    query.prepare("UPDATE employee SET Department=?, Salary=? WHERE EmpNo =?");
    //    query.addBindValue("技术部");
    //    query.addBindValue(6000);
    //    query.addBindValue(1007);
    //    if (query.exec())
    //    {
    //        //        QString str=query.lastQuery();
    //        QString str=query.executedQuery();
    //                ui->statusBar->showMessage("SQL执行成功："+str);
    //    }
    //    else
    //        ui->statusBar->showMessage("SQL执行失败: "+query.lastError().text());


    /////2. 使用问号占位符
    //    QSqlQuery query;
    //    query.prepare("UPDATE employee SET Department=?, Salary=? WHERE EmpNo =?");
    //    query.bindValue(0,  "技术部");
    //    query.bindValue(1,  5000);
    //    query.bindValue(2,  2006);
    //    if (query.exec())
    //    {
    //        //        QString str=query.lastQuery();
    //        QString str=query.executedQuery();
    //                ui->statusBar->showMessage("SQL执行成功："+str);
    //    }
    //    else
    //        ui->statusBar->showMessage("SQL执行失败: "+query.lastError().text());


    //////1. 使用参数名占位符
    //    QSqlQuery query;
    //    query.prepare("SELECT empNo, Name, Gender, Salary FROM employee "
    //                  " WHERE Gender =:sex AND Salary >=:salary");
    //    query.bindValue(":sex",  "男");
    //    query.bindValue(":salary",  5000);
    //    if (query.exec())
    //    {
    ////        QString str=query.lastQuery();
    //        QString str=query.executedQuery();
    ////        ui->statusBar->showMessage("SQL执行成功："+str);
    //        ui->statusBar->showMessage(QString("SQL执行成功,记录条数=%1").arg(query.size()));
    //    }
    //    else
    //        ui->statusBar->showMessage("SQL执行失败: "+query.lastError().text());


    ////1. 使用参数名占位符,
    //    QSqlQuery query;
    //    query.prepare("SELECT empNo, Name, Gender, Salary FROM employee "
    //                  " WHERE Gender =:sex AND Salary >=:salary");
    //    query.bindValue(0,  "男");
    //    query.bindValue(1,  5000);
    //    if (query.exec())
    //    {
    //        //        QString str=query.lastQuery();
    //        QString str=query.executedQuery();
    //                ui->statusBar->showMessage("SQL执行成功："+str);
    ////        ui->statusBar->showMessage(QString("SQL执行成功,记录条数=%1").arg(query.size()));
    //    }
    //    else
    //        ui->statusBar->showMessage("SQL执行失败: "+query.lastError().text());


}*/

void QSL::on_actFields_triggered()
{
    //获取字段列表
    QSqlRecord  emptyRec=qryModel->record();//获取空记录，只有字段名
    QString  str;
    for (int i=0;i<emptyRec.count();i++)
        str=str+emptyRec.fieldName(i)+'\n';
    QMessageBox::information(this, "所有字段名", str);
}

