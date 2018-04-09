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

#ifndef CALCULATIONSETTINGS_H
#define CALCULATIONSETTINGS_H

#include <QObject>
#include "Utilities/tags.h"

class CalculationSettings
{
public:
    CalculationSettings();

    QString scriptName;

    int topPrice,
        leftPrice,
        bottomPrice,
        rightPrice;

    int topConsumption,
        leftConsumption,
        bottomConsumption,
        rightConsumption;

    int topWeight,
        leftWeight,
        bottomWeight,
        rightWeight;

    double globalMaxConsumption,
           globalMinConsumption;

    DemandModel settingsModel;

    BehaviorK settingsK;
    double customK = -1;

    Behavior settingsZeroConsumption;
    double customZeroConsumptionReplacement = -1;

    Behavior settingsQ0;
    double customQ0replacement = -1;

    ScalingMode ParameterScaling;

    WeightingMode WeightSetting;

    SystematicCheck settingsCheck;

    ChartingOptions settingsChart;

    FittingAlgorithm settingsFitting;

    bool settingsAlternativePmax;
};

#endif // CALCULATIONSETTINGS_H
