#include "steincheckdialog.h"
#include "ui_steincheckdialog.h"

#include <QDebug>

SteinCheckDialog::SteinCheckDialog(QWidget *parent, QString jsonString) :
    QDialog(parent),
    ui(new Ui::SteinCheckDialog)
{
    ui->setupUi(this);

    canProceed = false;
    flagRaised = false;

    ui->tableWidget->clearContents();

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("Participant"));

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("Bounce"));

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("BouncePass"));

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("DeltaQ"));

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("DeltaQPass"));

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("Reversals"));

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("ReversalsPass"));

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("NumPosValues"));

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("TotalPass"));

    jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &err);
    jsonArr = jsonDoc.array();

    for (int i=0; i < jsonArr.count(); i++)
    {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());

        jsonVal = jsonArr.at(i);
        jsonObj = jsonVal.toObject();

        QTableWidgetItem *item = new QTableWidgetItem(QString::number(jsonObj["Participant"].toInt()));
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, item);

        item = new QTableWidgetItem(QString::number(jsonObj["Bounce"].toInt()));
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 1, item);

        item = new QTableWidgetItem(jsonObj["BouncePass"].toString());
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        if (!item->text().contains("Pass", Qt::CaseInsensitive))
        {
            flagRaised = true;
        }
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 2, item);

        item = new QTableWidgetItem(QString::number(jsonObj["DeltaQ"].toInt()));
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 3, item);

        item = new QTableWidgetItem(jsonObj["DeltaQPass"].toString());
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        if (!item->text().contains("Pass", Qt::CaseInsensitive))
        {
            flagRaised = true;
        }
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 4, item);

        item = new QTableWidgetItem(QString::number(jsonObj["Reversals"].toInt()));
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 5, item);

        item = new QTableWidgetItem(jsonObj["ReversalsPass"].toString());
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        if (!item->text().contains("Pass", Qt::CaseInsensitive))
        {
            flagRaised = true;
        }
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 6, item);

        item = new QTableWidgetItem(QString::number(jsonObj["NumPosValues"].toInt()));
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 7, item);

        item = new QTableWidgetItem(QString::number(jsonObj["TotalPass"].toInt()));
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 8, item);
    }
}

SteinCheckDialog::~SteinCheckDialog()
{
    delete ui;
}

void SteinCheckDialog::on_proceedButton_clicked()
{
    canProceed = true;
    close();
}

void SteinCheckDialog::on_cancelButton_clicked()
{
    canProceed = false;
    close();
}
