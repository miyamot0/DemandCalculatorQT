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

#ifndef SHEETSELECTDIALOG_H
#define SHEETSELECTDIALOG_H

#include <QDialog>

namespace Ui {
class SheetSelectDialog;
}

class SheetSelectDialog : public QDialog
{
    Q_OBJECT

public:
    /** Display current sheets, selecting one for import
     * @brief SheetSelectDialog
     * @param parent
     */
    explicit SheetSelectDialog(QWidget *parent = 0);

    /** Updates UI, with selection-based options
     * @brief UpdateUI
     * @param list
     */
    void UpdateUI(QStringList list);

    /** Pull selected item from UI
     * @brief GetSelected
     * @return
     */
    QString GetSelected();

    ~SheetSelectDialog();

private:
    Ui::SheetSelectDialog *ui;
};

#endif // SHEETSELECTDIALOG_H
