#ifndef SHEETDELEGATE_H
#define SHEETDELEGATE_H

#include <QObject>
#include <QAbstractTableModel>
#include <QItemDelegate>

class SheetDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    SheetDelegate();
    //QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    //void setEditorData(QWidget *editor, const QModelIndex &index) const;

public slots:
    void updatedEvent(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    //void commitAndClose();

};

#endif // SHEETDELEGATE_H
