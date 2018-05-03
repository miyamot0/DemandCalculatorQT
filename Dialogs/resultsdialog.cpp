/**
   Copyright 2017 Shawn Gilroy

   This file is part of Demand Curve Analyzer, Qt port.

   Demand Curve Analyzer is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 3.

   Demand Curve Analyzer is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Demand Curve Analyzer.  If not, see http://www.gnu.org/licenses/.

   The Demand Curve Analyzer is a tool to assist researchers in behavior economics.

   Email: shawn(dot)gilroy(at)temple.edu

  */

#include <QtXlsx>
#include <QDir>
#include <QFileDialog>
#include <QDesktopWidget>

#include "resultsdialog.h"
#include "ui_resultsdialog.h"

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

/**
 * @brief ResultsDialog::setOptions
 * @param showPmax
 */
void ResultsDialog::setOptions(bool showPmax)
{
    showAlternativePmax = showPmax;
}

/**
 * @brief ResultsDialog::setResults
 * @param mData
 */
void ResultsDialog::setResults(QList<QStringList> mData)
{
    auto mTags = getExponentiatedKeys();

    for(int i=0; i<mData.count(); i++)
    {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());

        QStringList mList = mData.at(i);

        for(int j=0; j<mList.count(); j++)
        {
            QTableWidgetItem *item = new QTableWidgetItem(mList.at(j));
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, j, item);
        }
    }

    ui->tableWidget->viewport()->update();
}

/**
 * @brief ResultsDialog::getLinearKeys
 * @return
 */
QStringList ResultsDialog::getLinearKeys()
{
    QStringList mReturnKeys;
    mReturnKeys << "ID"
                << "Equation"
                << "BP0"
                << "BP1"
                << "Omaxe"
                << "Pmaxe"
                << "L"
                << "Lse"
                << "b"
                << "bse"
                << "a"
                << "ase"
                << "R2"
                << "Elasticity"
                << "MeanElasticity"
                << "Intensity"
                << "Omaxd"
                << "Pmaxd"
                << "Notes"
                << "Prices"
                << "Consumption";

    return mReturnKeys;
}

/**
 * @brief ResultsDialog::getExponentialKeys
 * @return
 */
QStringList ResultsDialog::getExponentialKeys()
{
    QStringList mReturnKeys;
    mReturnKeys << "ID"
                << "Equation"
                << "Alpha"
                << "Alphase"
                << "Q0d"
                << "Q0dse"
                << "BP0"
                << "BP1"
                << "EV"
                << "Intensity"
                << "K"
                << "Kse"
                << "Omaxd"
                << "Omaxe"
                << "Pmaxd"
                << "Pmaxe"
                << "RMSError"
                << "R2"
                << "avgerror"
                << "N"
                << "Notes"
                << "K Method"
                << "Prices"
                << "Consumption";

    if (showAlternativePmax)
    {
        mReturnKeys << "Exact Pmax"
                    << "Exact Pmax Slope";
    }

    return mReturnKeys;
}

/**
 * @brief ResultsDialog::getExponentiatedKeys
 * @return
 */
QStringList ResultsDialog::getExponentiatedKeys()
{
    QStringList mReturnKeys;
    mReturnKeys << "ID"
                << "Equation"
                << "Alpha"
                << "Alphase"
                << "Q0d"
                << "Q0dse"
                << "BP0"
                << "BP1"
                << "EV"
                << "Intensity"
                << "K"
                << "Kse"
                << "Omaxd"
                << "Omaxe"
                << "Pmaxd"
                << "Pmaxe"
                << "RMSError"
                << "R2"
                << "SdRes"
                << "N"
                << "Notes"
                << "K Method"
                << "Prices"
                << "Consumption";

    if (showAlternativePmax)
    {
        mReturnKeys << "Exact Pmax"
                    << "Exact Pmax Slope";
    }

    return mReturnKeys;
}

/**
 * @brief ResultsDialog::setResultsType
 * @param mModel
 */
void ResultsDialog::setResultsType(DemandModel mModel)
{
    QStringList mColumns;

    if (mModel == DemandModel::Exponential)
    {
        mColumns = getExponentialKeys();
    }
    else if (mModel == DemandModel::Exponentiated)
    {
        mColumns = getExponentiatedKeys();
    }
    else if (mModel == DemandModel::Linear)
    {
        mColumns = getLinearKeys();
    }

    ui->tableWidget->clear();

    for (int i = 0; i < mColumns.count(); i++)
    {
        ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
        ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem(mColumns.at(i)));
    }
}

/**
 * @brief ResultsDialog::~ResultsDialog
 */
ResultsDialog::~ResultsDialog()
{
    delete ui;
}

/**
 * @brief ResultsDialog::copy
 */
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

/**
 * @brief ResultsDialog::on_pushButton_2_clicked
 */
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
