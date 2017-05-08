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

#ifndef STATUSDIALOG_H
#define STATUSDIALOG_H

#include <QDialog>
#include <QThread>
#include "rworker.h"

namespace Ui {
class StatusDialog;
}

class StatusDialog : public QDialog
{
    Q_OBJECT

public:
    /** Display status of system, for running calculations
     * @brief StatusDialog
     * @param rInstalled - Is the Rscript binary found?
     * @param commandParameter - Location of command
     * @param parent
     */
    explicit StatusDialog(bool rInstalled, bool isSVGsupported, QString commandParameter, QWidget *parent = 0);
    ~StatusDialog();

public slots:
    /** Slot for work updates, from background thread
     * @brief WorkUpdate
     * @param status - returned response from R
     */
    void WorkUpdate(QString status);
    void DiagnosticsComplete();

private:
    Ui::StatusDialog *ui;

    QThread *thread;

    Rworker *rWorker;

    int orderVar;
};

#endif // STATUSDIALOG_H
