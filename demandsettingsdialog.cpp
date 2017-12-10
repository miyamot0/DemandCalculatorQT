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

    isRunningAnalyses = false;
}

DemandSettingsDialog::~DemandSettingsDialog()
{
    delete ui;
}

void DemandSettingsDialog::ToggleButton(bool status)
{
    if (status)
    {
        ui->pushButton->setText("Calculate Demand");

        isRunningAnalyses = false;
    }
    else
    {
        ui->pushButton->setText("Cancel Analyses");

        isRunningAnalyses = true;
    }
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
        ui->q0KeepValue->setChecked(true);
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
    SheetWidget *temp = qobject_cast <SheetWidget *>(parent());

    if (isRunningAnalyses)
    {
        temp->KillThread();

        return;
    }

    temp->calculationSettings = new CalculationSettings();

    temp->calculationSettings->settingsK = getBehaviorK();

    if (temp->calculationSettings->settingsK == BehaviorK::Custom)
    {
        QString mText = ui->kSettingsCustomText->text();

        bool isTextNumeric;

        mText.toDouble(&isTextNumeric);

        if (isTextNumeric)
        {
            temp->calculationSettings->customK = mText.toDouble(&isTextNumeric);
        }
        else
        {
            QMessageBox messageBox;
            messageBox.critical(this, "Error", "Your custom supplied K value is not valid.");
            messageBox.show();
            return;
        }
    }

    temp->calculationSettings->settingsZeroConsumption = getBehaviorZeroConsumption();

    if (temp->calculationSettings->settingsZeroConsumption == Behavior::Modify)
    {
        QString replnum = getReplaceZeroConsumptionString();
        bool isValidNumber = false;

        replnum.toDouble(&isValidNumber);

        if (isValidNumber)
        {
            temp->calculationSettings->customZeroConsumptionReplacement = replnum.toDouble(&isValidNumber);
        }
        else
        {
            QMessageBox messageBox;
            messageBox.critical(this, "Error", "Your custom supplied Y replacement is not valid.");
            messageBox.show();

            return;
        }
    }

    temp->calculationSettings->settingsQ0 = getBehaviorQ0();

    if (temp->calculationSettings->settingsQ0 == Behavior::Modify)
    {
        if (ui->breakpointModifyValueHundredth->isChecked())
        {
            temp->calculationSettings->customQ0replacement = 0.01;
        }
        else if (ui->breakpointModifyValueTenth->isChecked())
        {
            temp->calculationSettings->customQ0replacement = 0.1;
        }
        else if (ui->breakpointModifyValueCustom->isChecked())
        {
            QString replnum = getReplaceZeroConsumptionString();

            bool isValidNumber = false;

            replnum.toDouble(&isValidNumber);

            if (isValidNumber)
            {
                temp->calculationSettings->customQ0replacement = replnum.toDouble(&isValidNumber);
            }
            else
            {
                QMessageBox messageBox;
                messageBox.critical(this, "Error", "Your custom supplied Y replacement is not valid.");
                messageBox.show();

                return;
            }
        }
    }

    // Model Choice
    temp->calculationSettings->settingsModel = getModel();

    // Pricing Values
    temp->calculationSettings->topPrice = topPrice;
    temp->calculationSettings->leftPrice = leftPrice;
    temp->calculationSettings->bottomPrice = bottomPrice;
    temp->calculationSettings->rightPrice = rightPrice;

    // Consumption Values
    temp->calculationSettings->topConsumption = topConsumption;
    temp->calculationSettings->leftConsumption = leftConsumption;
    temp->calculationSettings->bottomConsumption = bottomConsumption;
    temp->calculationSettings->rightConsumption = rightConsumption;

    // Checking Behavior
    temp->calculationSettings->settingsCheck = getSystematicCheck();

    // Charting Behavior
    temp->calculationSettings->settingsChart = getCharting();

    // Scaling Mode
    temp->calculationSettings->ParameterScaling = getScalingMode();

    temp->Calculate();
}

DemandModel DemandSettingsDialog::getModel()
{
    if (ui->modelingLinear->isChecked())
    {
        return DemandModel::Linear;
    }
    else if (ui->modelingExponential->isChecked())
    {
        return DemandModel::Exponential;
    }
    else
    {
        return DemandModel::Exponentiated;
    }
}

ScalingMode DemandSettingsDialog::getScalingMode()
{
    if (ui->checkBoxScaling->isChecked())
    {
        return ScalingMode::Enabled;
    }
    else
    {
        return ScalingMode::Disabled;
    }
}

BehaviorK DemandSettingsDialog::getBehaviorK()
{
    if (ui->kSettingsLogIndividual->isChecked())
    {
        return BehaviorK::Individual;
    }
    else if (ui->kSettingsLogGroup->isChecked())
    {
        return BehaviorK::Range;
    }
    else if (ui->kSettingsFit->isChecked())
    {
        return BehaviorK::Fit;
    }
    else if (ui->kSettingsFitShare->isChecked())
    {
        return BehaviorK::Share;
    }
    else
    {
        return BehaviorK::Custom;
    }
}

Behavior DemandSettingsDialog::getBehaviorZeroConsumption()
{
    if (ui->breakpointDropValue->isChecked())
    {
        return Behavior::Drop;
    }
    else if (ui->breakpointKeepValue->isChecked())
    {
        return Behavior::Keep;
    }
    else
    {
        return Behavior::Modify;
    }
}

Behavior DemandSettingsDialog::getBehaviorQ0()
{
    if (ui->q0DropValue->isChecked())
    {
        return Behavior::Drop;
    }
    else if (ui->q0KeepValue->isChecked())
    {
        return Behavior::Keep;
    }
    else
    {
        return Behavior::Modify;
    }
}

SystematicCheck DemandSettingsDialog::getSystematicCheck()
{
    if (ui->checkAlways->isChecked())
    {
        return SystematicCheck::Always;
    }
    else if (ui->checkFlag->isChecked())
    {
        return SystematicCheck::Flag;
    }
    else
    {
        return SystematicCheck::Never;
    }
}

ChartingOptions DemandSettingsDialog::getCharting()
{
    if (ui->figuresEnable->isChecked())
    {
        return ChartingOptions::Native;
    }
    else if (ui->figuresEnableStandard->isChecked())
    {
        return ChartingOptions::Standardized;
    }
    else
    {
        return ChartingOptions::None;
    }
}

// String getters

QString DemandSettingsDialog::getReplaceQ0String()
{
    return ui->q0CustomValueText->text();
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

// Event handles

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
