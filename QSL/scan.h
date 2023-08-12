#ifndef SCAN_H
#define SCAN_H

#include <QWidget>
#include <QtSql>
namespace Ui {
class scan;
}

class scan : public QWidget
{
    Q_OBJECT

public:
    explicit scan(QWidget *parent = nullptr);
    ~scan();

private slots:
    void on_researchbutton_clicked();

    void on_cancelbutton_clicked();


private:
    Ui::scan *ui;
};

#endif // SCAN_H
