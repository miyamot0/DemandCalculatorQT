#include "steincheck.h"
#include "ui_steincheck.h"

SteinCheck::SteinCheck(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SteinCheck)
{
    ui->setupUi(this);

    canProceed = false;

    ui->tableWidget->clearContents();

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("Participant"));

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("Total Pass"));

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("DeltaQ"));

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("DeltaQPass"));

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("Bounce"));

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("BouncePass"));

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("Reversals"));

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("ReversalsPass"));

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("NumPosValues"));

    ui->tableWidget->horizontalHeader()->setSectionResizeMode (QHeaderView::Fixed);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
}

void SteinCheck::appendRow(QStringList steinResults)
{
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());

    // Draw participant tag
    item = new QTableWidgetItem(steinResults[0]);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, item);

    // Color code total pass
    item = new QTableWidgetItem(steinResults[1]);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    if (item->text().contains("3", Qt::CaseInsensitive))
    {
        item->setBackgroundColor(QColor(Qt::green));
    }
    else if (item->text().contains("2", Qt::CaseInsensitive))
    {
        item->setBackgroundColor(QColor(Qt::yellow));
    }
    else if (item->text().contains("1", Qt::CaseInsensitive) || item->text().contains("0", Qt::CaseInsensitive))
    {
        item->setBackgroundColor(QColor(Qt::red));
    }
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 1, item);

    // Loop through rest
    for (int i = 2; i < steinResults.count(); i++)
    {
        item = new QTableWidgetItem(steinResults[i]);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, i, item);
    }
}

SteinCheck::~SteinCheck()
{
    delete ui;
}

void SteinCheck::on_buttonBox_accepted()
{
    canProceed = true;
}

void SteinCheck::on_buttonBox_rejected()
{
    canProceed = false;
}
