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
