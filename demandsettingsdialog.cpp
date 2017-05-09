#include "demandsettingsdialog.h"
#include "ui_demandsettingsdialog.h"

DemandSettingsDialog::DemandSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DemandSettingsDialog)
{
    ui->setupUi(this);
}

DemandSettingsDialog::~DemandSettingsDialog()
{
    delete ui;
}
