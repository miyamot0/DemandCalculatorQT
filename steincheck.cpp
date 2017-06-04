#include "steincheck.h"
#include "ui_steincheck.h"

SteinCheck::SteinCheck(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SteinCheck)
{
    ui->setupUi(this);
}

SteinCheck::~SteinCheck()
{
    delete ui;
}
