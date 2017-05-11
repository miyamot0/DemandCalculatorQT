#ifndef STEINCHECKDIALOG_H
#define STEINCHECKDIALOG_H

#include <QDialog>
#include <QtWidgets>
#include <QProcess>

namespace Ui {
class SteinCheckDialog;
}

class SteinCheckDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SteinCheckDialog(QWidget *parent = 0, QString jsonString = "");
    ~SteinCheckDialog();
    bool canProceed;

private slots:
    void on_proceedButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::SteinCheckDialog *ui;
    QJsonParseError err;
    QJsonDocument jsonDoc;
    QJsonArray jsonArr;
    QJsonValue jsonVal;
    QJsonObject jsonObj;
};

#endif // STEINCHECKDIALOG_H
