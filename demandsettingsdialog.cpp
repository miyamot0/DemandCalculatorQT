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

#include "demandsettingsdialog.h"
#include "ui_demandsettingsdialog.h"

#include "sheetwidget.h"

DemandSettingsDialog::DemandSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DemandSettingsDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Tool);
}

DemandSettingsDialog::~DemandSettingsDialog()
{
    delete ui;
}

void DemandSettingsDialog::ToggleButton(bool status)
{
    ui->pushButton->setEnabled(status);
}

void DemandSettingsDialog::WindowStateActive(bool status)
{
    ui->priceRangeText->setEnabled(status);
    ui->consumptionRangeText->setEnabled(status);
    ui->groupBoxModels->setEnabled(status);
    ui->groupBoxStein->setEnabled(status);
    ui->groupBoxY->setEnabled(status);
    ui->groupBoxX->setEnabled(status);
    ui->groupBoxK->setEnabled(status);
    ui->groupBoxFigure->setEnabled(status);

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
    if (checked)
    {
        ui->q0DropValue->setChecked(true);
        ui->breakpointDropValue->setChecked(true);
    }

    ui->groupBoxK->setEnabled(!checked);
}

void DemandSettingsDialog::on_modelingExponential_toggled(bool checked)
{
    if (checked)
    {
        ui->q0DropValue->setChecked(true);
        ui->breakpointDropValue->setChecked(true);
    }
}

void DemandSettingsDialog::on_modelingExponentiated_toggled(bool checked)
{
    if (checked)
    {
        ui->q0KeepValue->setChecked(true);
        ui->breakpointKeepValue->setChecked(true);
    }
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

    QString rem0 = getRemZeroString();
    QString replnum = "0.01";

    if (rem0.contains("MODIFY"))
    {
        replnum = getReplaceZeroConsumptionString();
        bool isValidNumber = false;

        replnum.toDouble(&isValidNumber);

        if (!isValidNumber)
        {
            QMessageBox messageBox;
            messageBox.critical(this, "Error", "Your custom supplied Y replacement is not valid.");
            messageBox.show();

            return;
        }
    }

    QString remQ0 = getRemQZeroString();
    QString replQ0 = "0.01";

    if (remQ0.contains("MODIFY"))
    {
        replQ0 = getReplaceQ0String();
        bool isValidNumber = false;

        replQ0.toDouble(&isValidNumber);

        if (!isValidNumber)
        {
            QMessageBox messageBox;
            messageBox.critical(this, "Error", "Your custom supplied X replacement is not valid.");
            messageBox.show();

            return;
        }
    }

    SheetWidget *temp = qobject_cast <SheetWidget *>(parent());
    temp->Calculate("checkSystematic.R", getModelString(), mKValue,
                    topPrice, leftPrice, bottomPrice, rightPrice,
                    topConsumption, leftConsumption, bottomConsumption, rightConsumption,
                    ui->checkAlways->isChecked(), ui->checkFlag->isChecked(),
                    rem0, replnum, remQ0, replQ0,
                    ui->figuresEnable->isChecked(),
                    ui->figuresEnableStandard->isChecked());
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

        mText.toDouble(&isTextNumeric);

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

QString DemandSettingsDialog::getRemQZeroString()
{
    QString mReturnValue = "KEEP";

    if (ui->q0DropValue->isChecked())
    {
        mReturnValue = "DROP";

    } else if (ui->q0KeepValue->isChecked())
    {
        mReturnValue = "KEEP";

    } else if (ui->q0ModifyValue->isChecked())
    {
        mReturnValue = "MODIFY";
    }

    return mReturnValue;
}

QString DemandSettingsDialog::getReplaceQ0String()
{
    return ui->q0CustomValueText->text();
}

QString DemandSettingsDialog::getRemZeroString()
{
    QString mReturnValue = "KEEP";

    if (ui->breakpointDropValue->isChecked())
    {
        mReturnValue = "DROP";
    }
    else if (ui->breakpointKeepValue->isChecked())
    {
        mReturnValue = "KEEP";
    }
    else if (ui->breakpointModifyValueTenth->isChecked() ||
             ui->breakpointModifyValueHundredth->isChecked() ||
             ui->breakpointModifyValueCustom->isChecked())
    {
        mReturnValue = "MODIFY";
    }

    return mReturnValue;
}

QString DemandSettingsDialog::getReplaceZeroConsumptionString()
{
    QString mReturnValue = "0.01";

    if (ui->breakpointModifyValueTenth->isChecked())
    {
        mReturnValue = "0.1";
    }
    else if (ui->breakpointModifyValueHundredth->isChecked())
    {
        mReturnValue = "0.01";
    }
    else if (ui->breakpointModifyValueCustom->isChecked())
    {
        mReturnValue = ui->breakpointModifyValueCustomText->text();
    }

    return mReturnValue;
}

void DemandSettingsDialog::on_kSettingsCustom_toggled(bool checked)
{
    ui->kSettingsCustomText->setEnabled(checked);
}

void DemandSettingsDialog::on_breakpointModifyValueCustom_toggled(bool checked)
{
    ui->breakpointModifyValueCustomText->setEnabled(checked);
}

void DemandSettingsDialog::on_q0ModifyValue_toggled(bool checked)
{
    ui->q0CustomValueText->setEnabled(checked);
}
