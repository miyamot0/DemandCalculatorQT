/**
   Copyright 2017 Shawn Gilroy

   This file is part of Discounting Model Selector, Qt port.

   Discounting Model Selector is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 3.

   Discounting Model Selector is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Discounting Model Selector.  If not, see http://www.gnu.org/licenses/.

   The Discounting Model Selector is a tool to assist researchers in behavior economics.

   Email: shawn(dot)gilroy(at)temple.edu

  */

#include <QtXlsx>
#include <QDir>
#include <QFileDialog>
#include <QDesktopWidget>

#include "resultsdialog.h"
#include "ui_resultsdialog.h"
#include "sheetwidget.h"

ResultsDialog::ResultsDialog(QWidget *parent, QString jsonString) :
    QDialog(parent),
    ui(new Ui::ResultsDialog)
{
    ui->setupUi(this);

    this->window()->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            window()->size(),
            qApp->desktop()->availableGeometry()
        )
    );

    copyAction = new QAction("Copy", this);
    copyAction->setShortcut(QKeySequence("Ctrl+C"));
    copyAction->setIcon(QIcon(":/images/edit-copy.png"));
    connect(copyAction, &QAction::triggered, this, &ResultsDialog::copy);

    addAction(copyAction);    

    jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &err);
    jsonArr = jsonDoc.array();

    QStringList keyList;

    for (int i=0; i < jsonArr.count(); i++)
    {
        jsonVal = jsonArr.at(i);
        jsonObj = jsonVal.toObject();

        if (i == 0)
        {
            if (jsonObj["Equation"].toString().contains("hs"))
            {
                keyList = getExponentialKeys();
            }
            else if (jsonObj["Equation"].toString().contains("koff"))
            {
                keyList = getExponentiatedKeys();
            }
            else
            {
                keyList = getLinearKeys();
            }
        }

        if (i == 0)
        {
            ui->tableWidget->clearContents();

            for (int j=0; j<keyList.length(); j++)
            {
                ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
                ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem(keyList.at(j)));
            }
        }

        ui->tableWidget->insertRow(ui->tableWidget->rowCount());

        for (int j=0; j<keyList.length(); j++)
        {
            QString label = "";

            QJsonValueRef mObj = jsonObj[keyList.at(j)];

            if (mObj.isDouble())
            {
                label = QString::number(mObj.toDouble());
            }
            else if (mObj.isString())
            {
                label = mObj.toString();
            }

            QTableWidgetItem *item = new QTableWidgetItem(label);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, j, item);
        }
    }
}

QStringList ResultsDialog::getLinearKeys()
{
    QStringList mReturnKeys;
    mReturnKeys << "ID" << "Equation"
                << "BP1" << "Intensity"
                << "Omaxe" << "Pmaxe";

    return mReturnKeys;
}

QStringList ResultsDialog::getExponentialKeys()
{
    QStringList mReturnKeys;
    mReturnKeys << "ID" << "Equation"
                << "Alpha" << "AlphaHigh" << "AlphaLow" << "Alphase"
                << "Q0d" << "Q0High" << "Q0Low" << "Q0se"
                << "BP1" << "EV" << "Intensity" << "K"
                << "Omaxd" << "Omaxe" << "Pmaxd" << "Pmaxe"
                << "AbsSS"  << "R2" << "SdRes" << "N" << "Notes" ;

    return mReturnKeys;
}

QStringList ResultsDialog::getExponentiatedKeys()
{
    QStringList mReturnKeys;
    mReturnKeys << "ID" << "Equation"
                << "Alpha" << "AlphaHigh" << "AlphaLow" << "Alphase"
                << "Q0d" << "Q0High" << "Q0Low" << "Q0se"
                << "BP0" << "BP1" << "EV" << "Intensity" << "K"
                << "Omaxd" << "Omaxe" << "Pmaxd" << "Pmaxe"
                << "AbsSS"  << "R2" << "SdRes" << "N" << "Notes" ;

    return mReturnKeys;
}

ResultsDialog::~ResultsDialog()
{
    delete ui;
}

void ResultsDialog::copy()
{
    QList<QTableWidgetSelectionRange> range = ui->tableWidget->selectedRanges();
    QTableWidgetSelectionRange mRange = range.first();

    QString str;

    for (int i = 0; i < mRange.rowCount(); ++i) {
        if (i > 0)
        {
            str += "\n";
        }

        for (int j = 0; j < mRange.columnCount(); ++j) {
            if (j > 0)
            {
                str += "\t";
            }

            if (ui->tableWidget->item(mRange.topRow() + i, mRange.leftColumn() + j) == NULL)
            {
                str += "";
            }
            else
            {
                str += ui->tableWidget->item(mRange.topRow() + i, mRange.leftColumn() + j)->data(Qt::DisplayRole).toString();
            }
        }
    }

    QApplication::clipboard()->setText(str);
}

void ResultsDialog::on_pushButton_2_clicked()
{
    QString selFilter="Spreadsheet (*.xlsx)";
    QString file_name = QFileDialog::getSaveFileName(this, "Save file", QDir::homePath(), "Spreadsheet (*.xlsx)", &selFilter);

    if(!file_name.trimmed().isEmpty())
    {
        QXlsx::Document xlsx;

        int rows = ui->tableWidget->rowCount();
        int cols = ui->tableWidget->columnCount();

        QString temp;
        QTableWidgetItem *col;

        for (int i=0; i<rows; i++)
        {
            for (int j=0; j<cols; j++)
            {
                if (i == 0)
                {
                    col = ui->tableWidget->horizontalHeaderItem(j);
                    temp = col->text();
                    xlsx.write(1, j + 1, temp);
                }

                QTableWidgetItem *item = ui->tableWidget->item(i, j);

                if (item != NULL && !item->text().isEmpty())
                {
                    temp = ui->tableWidget->item(i, j)->data(Qt::DisplayRole).toString();
                    xlsx.write(i + 2, j + 1, temp);
                }

            }
        }

        xlsx.saveAs(file_name);
    }
}
