#include <QtWidgets>
#include <QTableWidgetItem>

#include "sheetwidget.h"
#include "sheetdelegate.h"
#include "commanding.h"

SheetDelegate::SheetDelegate()
{

}

/*
QWidget *SheetDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const
{
    QLineEdit *editor = new QLineEdit(parent);

    qDebug() << "create editor";

    //connect(editor, &QLineEdit::textChanged, this, &SheetDelegate::commitAndClose);



    return editor;
}
*/

void SheetDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    if (edit) {

        qDebug() << "add to stack";

        SheetWidget *temp = qobject_cast <SheetWidget *>(model->parent()->parent());
        temp->undoStack->push(new UpdateCommand(&index, model->data(index, Qt::EditRole).toString(), edit->text()));
        model->setData(index, edit->text(), Qt::EditRole);



        //connect(model, &QAbstractItemModel::dataChanged, this, &SheetDelegate::updatedEvent);

        //connect(edit,
        //        SIGNAL(textChanged(QString)),
        //        this,
        //        SLOT(updatedEvent()));
        //connect(edit, SIGNAL(textChanged(QString)), this, SLOT(updatedEvent(QString)));

        return;
    }
}

/*
void SheetDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    qDebug() << "set editor data";
}
*/

void SheetDelegate::updatedEvent(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    //SheetWidget *temp = qobject_cast <SheetWidget *>(topLeft.model()->parent()->parent());
}

