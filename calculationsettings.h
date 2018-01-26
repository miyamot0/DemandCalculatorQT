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

enum class ScalingMode
{
    Enabled,
    Disabled
};

enum class SystematicCheck
{
    Always,
    Flag,
    Never
};

enum class FittingAlgorithm
{
    Function,
    FunctionGradient,
    FunctionGradientHessian
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
           globalMinConsumption;

    DemandModel settingsModel;

    BehaviorK settingsK;
    double customK = -1;

    Behavior settingsZeroConsumption;
    double customZeroConsumptionReplacement = -1;

    Behavior settingsQ0;
    double customQ0replacement = -1;

    ScalingMode ParameterScaling;

    SystematicCheck settingsCheck;

    ChartingOptions settingsChart;

    FittingAlgorithm settingsFitting;

    bool settingsAlternativePmax;
};

#endif // CALCULATIONSETTINGS_H
