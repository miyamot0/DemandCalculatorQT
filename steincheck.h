#ifndef STEINCHECK_H
#define STEINCHECK_H

#include <QDialog>

namespace Ui {
class SteinCheck;
}

class SteinCheck : public QDialog
{
    Q_OBJECT

public:
    explicit SteinCheck(QWidget *parent = 0);
    ~SteinCheck();

private:
    Ui::SteinCheck *ui;
};

#endif // STEINCHECK_H
