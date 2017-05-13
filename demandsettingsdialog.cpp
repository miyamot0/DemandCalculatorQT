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
    QString mKValue = getKString();

    if (mKValue.contains("NULL", Qt::CaseInsensitive))
    {
        QMessageBox messageBox;
        messageBox.critical(this, "Error", "Your custom supplied K value is not valid.");
        messageBox.show();
        return;
    }

    SheetWidget *temp = qobject_cast <SheetWidget *>(parent());
    temp->Calculate("checkSystematic.R", getModelString(), mKValue,
                    topPrice, leftPrice, bottomPrice, rightPrice,
                    topConsumption, leftConsumption, bottomConsumption, rightConsumption,
                    ui->checkAlways->isChecked(), ui->checkFlag->isChecked(), ui->figuresEnable->isChecked());
}

QString DemandSettingsDialog::getModelString()
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

    return mModel;
}

QString DemandSettingsDialog::getKString()
{
    QString mK = "";

    if (ui->kSettingsLogIndividual->isChecked())
    {
        mK = "ind";
    }
    else if (ui->kSettingsLogGroup->isChecked())
    {
        mK = "range";
    }
    else if (ui->kSettingsFit->isChecked())
    {
        mK = "fit";
    }
    else if (ui->kSettingsFitShare->isChecked())
    {
        mK = "share";
    }
    else if (ui->kSettingsCustom->isChecked())
    {
        QString mText = ui->kSettingsCustomText->text();

        bool isTextNumeric;

        double mValue = mText.toDouble(&isTextNumeric);

        if (isTextNumeric)
        {
            mK = mText;
        }
        else
        {
            mK = "NULL";
        }
    }

    return mK;
}

void DemandSettingsDialog::on_kSettingsCustom_toggled(bool checked)
{
    ui->kSettingsCustomText->setEnabled(checked);
}
