#ifndef CALCULATIONSETTINGS_H
#define CALCULATIONSETTINGS_H

#include <QObject>

enum class DemandModel
{
    Linear,
    Exponential,
    Exponentiated
};

enum class ChartingOptions
{
    None,
    Native,
    Standardized
};

enum class BehaviorK
{
    Individual,
    Range,
    Fit,
    Share,
    Custom
};

enum class Behavior
{
    Keep,
    Drop,
    Modify
};

enum class SystematicCheck
{
    Always,
    Flag,
    Never
};

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

    double globalMaxConsumption,
           globalMinConsumption,
           globalFitK;

    DemandModel settingsModel;

    BehaviorK settingsK;
    double customK = -1;

    Behavior settingsZeroConsumption;
    double customZeroConsumptionReplacement = -1;

    Behavior settingsQ0;
    double customQ0replacement = -1;

    SystematicCheck settingsCheck;

    ChartingOptions settingsChart;
};

#endif // CALCULATIONSETTINGS_H
