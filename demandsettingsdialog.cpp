#include "demandsettingsdialog.h"
#include "ui_demandsettingsdialog.h"

#include "sheetwidget.h"

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

void DemandSettingsDialog::ToggleButton(bool status)
{
    ui->pushButton->setEnabled(status);
}

void DemandSettingsDialog::UpdatePrices(QString label, int top, int left, int bottom, int right)
{
    ui->priceRangeText->setText(label);

    topPrice = top;
    leftPrice = left;
    bottomPrice = bottom;
    rightPrice = right;
}

void DemandSettingsDialog::UpdateConsumption(QString label, int top, int left, int bottom, int right)
{
    ui->consumptionRangeText->setText(label);

    topConsumption = top;
    leftConsumption = left;
    bottomConsumption = bottom;
    rightConsumption = right;
}

void DemandSettingsDialog::on_modelingLinear_toggled(bool checked)
{
    ui->q0DropValue->setChecked(true);
    ui->breakpointDropValue->setChecked(true);
}

void DemandSettingsDialog::on_modelingExponential_toggled(bool checked)
{
    ui->q0DropValue->setChecked(true);
    ui->breakpointDropValue->setChecked(true);
}

void DemandSettingsDialog::on_modelingExponentiated_toggled(bool checked)
{
    ui->q0KeepValue->setChecked(true);
    ui->breakpointKeepValue->setChecked(true);
}

void DemandSettingsDialog::on_pushButton_clicked()
{
    QString mModel = "";

    if (ui->modelingLinear->isChecked())
    {
        mModel = "linear";
    }
    else if (ui->modelingExponential->isChecked())
    {
        mModel = "hs";
    }
    else if (ui->modelingExponentiated->isChecked())
    {
        mModel = "koff";
    }

    // add "check" and "notify" based on entries

    SheetWidget *temp = qobject_cast <SheetWidget *>(parent());
    temp->Calculate("calculateDemand.R", mModel,
                    topPrice, leftPrice, bottomPrice, rightPrice,
                    topConsumption, leftConsumption, bottomConsumption, rightConsumption,
                    true, true, ui->figuresEnable->isChecked());
}
