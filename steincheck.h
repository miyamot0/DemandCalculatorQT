#ifndef STEINCHECK_H
#define STEINCHECK_H

#include <QDialog>
#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QStandardItem>
#include <QCheckBox>
#include <QHBoxLayout>

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
    QWidget *pWidget;
    QCheckBox *pCheckBox;
    QHBoxLayout *pLayout;
    QList<bool> mJonhsonBickelSelections;

    bool canProceed = false;
    bool flagRaised = false;

    void appendRow(QString participant, QString criteriaOne, QString criteriaTwo);
    int getIndexOption();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::SteinCheck *ui;
};

#endif // STEINCHECK_H
