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

#include <QtWidgets>

#include "commanding.h"
#include "sheetwidget.h"

UpdateCommand::UpdateCommand(const QModelIndex *mIndex, const QString &oldContent, QString &newContent, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    mItemModelRef = mIndex->model();
    mItemIndex = mIndex;
    mNewContent = newContent;
    mOldContent = oldContent;

    mColumn = mIndex->column();
    mRow = mIndex->row();
}

void UpdateCommand::undo()
{    
    SheetWidget *temp = qobject_cast <SheetWidget *>(mItemModelRef->parent()->parent());
    temp->table->item(mRow, mColumn)->setText(mOldContent);
}

void UpdateCommand::redo()
{
    SheetWidget *temp = qobject_cast <SheetWidget *>(mItemModelRef->parent()->parent());

    if (temp->table->item(mRow, mColumn) != NULL)
    {
        temp->table->item(mRow, mColumn)->setText(mNewContent);
    }
    else
    {
        temp->table->setItem(mRow, mColumn, new QTableWidgetItem(mNewContent));
    }
}

UpdateCommandBlock::UpdateCommandBlock(const QModelIndex *mIndex, const QStringList &oldContent, QStringList &newContent, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    mTopLeftModelRef = mIndex->model();
    mTopLeftIndex = mIndex;
    mNewContent = newContent;
    mOldContent = oldContent;

    mLeftColumn = mIndex->column();
    mLeftRow = mIndex->row();
}

void UpdateCommandBlock::undo()
{
    SheetWidget *temp = qobject_cast <SheetWidget *>(mTopLeftModelRef->parent()->parent());

    for (int row = 0; row < mOldContent.count(); row++)
    {
        QStringList columns = mOldContent[row].split('\t');

        for (int col = 0; col < columns.count(); col++)
        {
            temp->table->item(mLeftRow + row, mLeftColumn + col)->setText(columns[col]);
        }
    }
}

void UpdateCommandBlock::redo()
{
    SheetWidget *temp = qobject_cast <SheetWidget *>(mTopLeftModelRef->parent()->parent());

    for (int row = 0; row < mNewContent.count(); row++)
    {
        QStringList columns = mNewContent[row].split('\t');

        for (int col = 0; col < columns.count(); col++)
        {
            if (temp->table->item(mLeftRow + row, mLeftColumn + col) != NULL)
            {
                temp->table->item(mLeftRow + row, mLeftColumn + col)->setText(columns[col]);
            }
            else
            {
                temp->table->setItem(mLeftRow + row, mLeftColumn + col, new QTableWidgetItem(columns[col]));
            }
        }
    }
}
