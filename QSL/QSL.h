#ifndef QSL_H
#define QSL_H
//QT官方声明文件
#include <QMainWindow>
#include <QtWidgets>
#include <QtSql>
#include <QDataWidgetMapper>
#include <QFileDialog>
#include <QMessageBox>
//打开对话框文件
#include    "tdialogdata.h"

QT_BEGIN_NAMESPACE
namespace Ui { class QSL; }
QT_END_NAMESPACE
class QSL : public QMainWindow
{
    Q_OBJECT
private:
    QSqlDatabase  DB;   //数据库
    QSqlQueryModel  *qryModel;  //数据模型
    QItemSelectionModel *selModel;  //选择模型

    void    selectData();    //查询数据



public:
    QSL(QWidget *parent = nullptr);
    ~QSL();

    bool    openDatabase(QString aFile);
    void    updateRecord(int recNo);    //更新一条记录

private slots:

    // 与选择模型的currentRowChanged()信号关联
    //    void do_currentRowChanged(const QModelIndex &current, const QModelIndex &previous);

    void on_actOpenDB_triggered();

    void on_actRecInsert_triggered();

    void on_actRecDelete_triggered();

    void on_actRecEdit_triggered();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_actScan_triggered();

    //void on_actSQLtest_triggered();
    void on_actFields_triggered();

private:
    Ui::QSL *ui;
};
#endif // QSL_H
