#ifndef STEINCHECK_H
#define STEINCHECK_H

#include <QDialog>
#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>

namespace Ui {
class SteinCheck;
}

class SteinCheck : public QDialog
{
    Q_OBJECT

public:
    explicit SteinCheck(QWidget *parent = 0);
    ~SteinCheck();

    QTableWidgetItem *item;

    bool canProceed = false;
    bool flagRaised = false;

    void appendRow(QStringList steinResults);

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::SteinCheck *ui;
};

#endif // STEINCHECK_H
