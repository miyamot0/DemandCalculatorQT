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

ResultsDialog::ResultsDialog(QWidget *parent) :
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
}

void ResultsDialog::ImportDataAndShow(bool cbBIC, bool cbAIC, bool cbRMSE, bool cbBF, bool tripLogNormal, QString metric)
{
    SheetWidget *temp = qobject_cast <SheetWidget *>(parent());

    QList<int> skipList;

    skipList << 2 << 9 << 37;

    if (!cbRMSE)
    {
        skipList << 3 << 10 << 17 << 24 << 31 << 38;
    }

    if (!cbBIC)
    {
        skipList << 4 << 11 << 18 << 25 << 32 << 39;
    }

    if (!cbAIC)
    {
        skipList << 5 << 12 << 19 << 26 << 33 << 40;
    }

    if (!cbBF)
    {
        skipList << 6 << 13 << 20 << 27 << 34 << 41;
    }

    QStringList columnList;

    columnList << "Series";

    if (tripLogNormal)
    {
        columnList << "Mazur.k";
    }
    else
    {
        columnList << "Mazur.lnk";
    }

    columnList << "";
    columnList << "Mazur.RMSE";
    columnList << "Mazur.BIC";
    columnList << "Mazur.AIC";
    columnList << "Mazur.BF";
    columnList << "Mazur.prob";

    if (tripLogNormal)
    {
        columnList << "exp.k";
    }
    else
    {
        columnList << "exp.lnk";
    }

    columnList << "";
    columnList << "exp.RMSE";
    columnList << "exp.BIC";
    columnList << "exp.AIC";
    columnList << "exp.BF";
    columnList << "exp.prob";

    columnList << "BD.beta";
    columnList << "BD.delta";
    columnList << "BD.RMSE";
    columnList << "BD.BIC";
    columnList << "BD.AIC";
    columnList << "BD.BF";
    columnList << "BD.prob";

    if (tripLogNormal)
    {
        columnList << "MG.k";
    }
    else
    {
        columnList << "MG.lnk";
    }

    columnList << "MG.s";
    columnList << "MG.RMSE";
    columnList << "MG.BIC";
    columnList << "MG.AIC";
    columnList << "MG.BF";
    columnList << "MG.prob";

    if (tripLogNormal)
    {
        columnList << "Rachlin.k";
    }
    else
    {
        columnList << "Rachlin.lnk";
    }

    columnList << "Rachlin.s";
    columnList << "Rachlin.RMSE";
    columnList << "Rachlin.BIC";
    columnList << "Rachlin.AIC";
    columnList << "Rachlin.BF";
    columnList << "Rachlin.prob";

    columnList << "noise.mean";
    columnList << "";
    columnList << "noise.RMSE";
    columnList << "noise.BIC";
    columnList << "noise.AIC";
    columnList << "noise.BF";
    columnList << "noise.prob";

    columnList << "probmodel";
    columnList << metric;

    // Create columns
    for(int i=0; i<columnList.count();i++)
    {
        if (!skipList.contains(i))
        {
            ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
            ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem(columnList.at(i)));
        }
    }

    int spacer;

    for(int i=0; i<temp->allResults.count(); i++)
    {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        QStringList mList = temp->allResults.at(i);

        spacer = 0;

        for(int j=0; j<mList.count(); j++)
        {
            if (!skipList.contains(j))
            {
                QTableWidgetItem *item = new QTableWidgetItem(mList.at(j));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, j - spacer, item);
            }
            else
            {
                spacer++;
            }
        }
    }

    ui->tableWidget->viewport()->update();

    show();
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

void ResultsDialog::convertExcelColumn(QString &mString, int column)
{
    int dividend = column + 1;
    QString columnName = "";

    int modulo;

    while (dividend > 0)
    {
        modulo = (dividend - 1) % 26;
        columnName = new QString(65 + modulo) + columnName;
        dividend = (int)((dividend - modulo) / 26);
    }

    mString = columnName;
}
