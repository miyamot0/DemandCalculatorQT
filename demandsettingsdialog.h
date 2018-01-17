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

#ifndef DEMANDSETTINGSDIALOG_H
#define DEMANDSETTINGSDIALOG_H

#include <QDialog>
#include "calculationsettings.h"

namespace Ui {
class DemandSettingsDialog;
}

class DemandSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DemandSettingsDialog(QWidget *parent = 0);
    ~DemandSettingsDialog();

    DemandModel getModel();

    BehaviorK getBehaviorK();

    Behavior getBehaviorQ0();
    Behavior getBehaviorZeroConsumption();
    ScalingMode getScalingMode();
    FittingAlgorithm getFittingAlgorithm();

    SystematicCheck getSystematicCheck();
    ChartingOptions getCharting();

public slots:
    void UpdatePrices(QString label, int top, int left, int bottom, int right);
    void UpdateConsumption(QString label, int top, int left, int bottom, int right);
    void ToggleButton(bool status);
    void WindowStateActive(bool status);

private slots:
    QString getReplaceQ0String();
    QString getReplaceZeroConsumptionString();

    void on_modelingLinear_toggled(bool checked);
    void on_modelingExponential_toggled(bool checked);
    void on_modelingExponentiated_toggled(bool checked);
    void on_kSettingsCustom_toggled(bool checked);
    void on_pushButton_clicked();

    void on_breakpointModifyValueCustom_toggled(bool checked);

    void on_q0ModifyValue_toggled(bool checked);

private:
    Ui::DemandSettingsDialog *ui;

    int topPrice;
    int leftPrice;
    int bottomPrice;
    int rightPrice;

    int topConsumption;
    int leftConsumption;
    int bottomConsumption;
    int rightConsumption;

    bool isRunningAnalyses;
};

#endif // DEMANDSETTINGSDIALOG_H
