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

#ifndef STEINCHECKDIALOG_H
#define STEINCHECKDIALOG_H

#include <QDialog>
#include <QtWidgets>

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
    bool flagRaised;

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
