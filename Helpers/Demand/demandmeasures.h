#ifndef DEMANDMEASURES_H
#define DEMANDMEASURES_H

#include <math.h>

#include <QString>

//#include "Modeling/demandmodeling.h"

#include "Models/calculationsettings.h"
#include "Models/fittingdata.h"

#include "Utilities/tags.h"

using namespace alglib;

QString getFittingAlgorithm(CalculationSettings * calculationSettings);
QString getKMessage(BehaviorK call);
QString getCodeString(int code);
QString getPmaxEString(QList<double> &yValues, QList<double> &xValues);
QString getOmaxEString(QList<double> &yValues, QList<double> &xValues);
QString getIntensityString(QList<double> &yValues, QList<double> &xValues);
QString getBP0String(QList<double> &yValues, QList<double> &xValues);
QString getBP1String(QList<double> &yValues, QList<double> &xValues);
double getPbar(QList<double> &yValues);

/*
 * Math from Hursh
 *
 */

double CalculateHurshPmax(double q0, double alpha, double k)
{
    return 1/(q0 * alpha * pow(k, 1.5)) * (0.083 * k + 0.65);
}

double CalculateHurshOmax(double q0, double alpha, double k, double pmaxd)
{
    return (pow(10, (log10(q0) + (k * (exp(-alpha * q0 * pmaxd) - 1))))) * pmaxd;
}

double CalculateHurshEV(double alpha, double k)
{
    return 1/(alpha * pow(k, 1.5) * 100);
}

/*
QList<double> SplitWeights(QString value)
{
    QString temp = value.replace("[", "");
    QString temp2 = temp.replace("]", "");

    QStringList tempList = temp2.split(",");

    QList<double> returnList;

    double num;
    foreach (QString str, tempList) {
        num = str.toDouble();

        returnList << num;
    }

    return returnList;
}
*/

/*
 * Output for sheet
 *
 */

void FailedExponentialExponentiatedDEOutput(QStringList * mTempHolder, int i, CalculationSettings * calculationSettings, FittingData fittingData)
{
    mTempHolder->clear();

    *mTempHolder << QString::number(i + 1)
        << ((calculationSettings->settingsModel == DemandModel::Exponential) ? "Exponential" + getFittingAlgorithm(calculationSettings) :
                                                                              "Exponentiated" + getFittingAlgorithm(calculationSettings))
        << "---"
        << "---"
        << "---"
        << "---"
        << getBP1String(fittingData.ConsumptionValues, fittingData.PriceValues)
        << "---"
        << getIntensityString(fittingData.ConsumptionValues, fittingData.PriceValues)
        << "---"
        << "---"
        << "---"
        << getOmaxEString(fittingData.ConsumptionValues, fittingData.PriceValues)
        << "---"
        << getPmaxEString(fittingData.ConsumptionValues, fittingData.PriceValues)
        << "---"
        << "---"
        << "---"
        << "---"
        << "Fewer than 3 data points, skipped"
        << getKMessage(calculationSettings->settingsK)
        << fittingData.Prices
        << fittingData.Consumption;

    if (calculationSettings->settingsAlternativePmax)
    {
        *mTempHolder << "---";
        *mTempHolder << "---";
    }
}

void SuccessfulExponentialExponentiatedDEOutput(QStringList * mTempHolder, int i, CalculationSettings * calculationSettings,
                                              FittingData fittingData, double q0, double alpha, double k, double avgerr)
{

    double pmaxd = CalculateHurshPmax(q0, alpha, k);
    double omaxd = CalculateHurshOmax(q0, alpha, k, pmaxd);
    double EV = CalculateHurshEV(alpha, k);

    QString modelString = calculationSettings->settingsModel == DemandModel::Exponential ? "Exponential" : "Exponentiated";

    mTempHolder->clear();
    *mTempHolder << QString::number(i + 1)
                 << modelString + getFittingAlgorithm(calculationSettings)
                 << QString::number(alpha)
                 << "---"
                 << QString::number(q0)
                 << "---"
                 << getBP1String(fittingData.ConsumptionValues, fittingData.PriceValues)
                 << QString::number(EV)
                 << getIntensityString(fittingData.ConsumptionValues, fittingData.PriceValues)
                 << QString::number(k)
                 << "---"
                 << QString::number(omaxd)
                 << getOmaxEString(fittingData.ConsumptionValues, fittingData.PriceValues)
                 << QString::number(pmaxd)
                 << getPmaxEString(fittingData.ConsumptionValues, fittingData.PriceValues)
                 << QString::number(avgerr)
                 << "---"
                 << QString::number(sqrt(avgerr))
                 << QString::number(fittingData.PriceValues.count())
                 << "---"
                 << getKMessage(calculationSettings->settingsK)
                 << fittingData.Prices
                 << fittingData.Consumption;

    if (calculationSettings->settingsAlternativePmax)
    {
        // TODO

        //mTempHolder << QString::number(mObj->GetBootStrapPmax());
        //mTempHolder << QString::number(mObj->GetBootStrapPmaxExponentialSlope(q0, alpha, k, mObj->GetBootStrapPmax()));

        *mTempHolder << "---";
        *mTempHolder << "---";
    }
}

void FailedExponentialExponentiatedLMOutput(QStringList * mTempHolder, int i, CalculationSettings * calculationSettings, FittingData fittingData)
{
    mTempHolder->clear();

    *mTempHolder << QString::number(i + 1)
        << ((calculationSettings->settingsModel == DemandModel::Exponential) ? "Exponential" + getFittingAlgorithm(calculationSettings) :
                                                                              "Exponentiated" + getFittingAlgorithm(calculationSettings))
        << "---"
        << "---"
        << "---"
        << "---"
        << getBP1String(fittingData.ConsumptionValues, fittingData.PriceValues)
        << "---"
        << getIntensityString(fittingData.ConsumptionValues, fittingData.PriceValues)
        << "---"
        << "---"
        << "---"
        << getOmaxEString(fittingData.ConsumptionValues, fittingData.PriceValues)
        << "---"
        << getPmaxEString(fittingData.ConsumptionValues, fittingData.PriceValues)
        << "---"
        << "---"
        << "---"
        << "---"
        << "Failure"
        << getKMessage(calculationSettings->settingsK)
        << fittingData.Prices
        << fittingData.Consumption;

    if (calculationSettings->settingsAlternativePmax)
    {
        *mTempHolder << "---";
        *mTempHolder << "---";
    }
}

void SuccessfulExponentialExponentiatedLMOutput(QStringList * mTempHolder, int i, CalculationSettings * calculationSettings,
                                                FittingData fittingData, double q0, double q0se,
                                                double alpha, double alphase,
                                                double k, double kse,
                                                double avgerr, double r2)
{
    double pmaxd = CalculateHurshPmax(q0, alpha, k);
    double omaxd = CalculateHurshOmax(q0, alpha, k, pmaxd);
    double EV = CalculateHurshEV(alpha, k);

    QString kseString = kse == -1 ? "---" : QString::number(kse);

    QString modelString = calculationSettings->settingsModel == DemandModel::Exponential ? "Exponential" : "Exponentiated";

    mTempHolder->clear();
    *mTempHolder << QString::number(i + 1)
                 << modelString + getFittingAlgorithm(calculationSettings)
                 << QString::number(alpha)
                 << QString::number(alphase)
                 << QString::number(q0)
                 << QString::number(q0se)
                 << getBP1String(fittingData.ConsumptionValues, fittingData.PriceValues)
                 << getBP0String(fittingData.ConsumptionValues, fittingData.PriceValues)
                 << QString::number(EV)
                 << getIntensityString(fittingData.ConsumptionValues, fittingData.PriceValues)
                 << QString::number(k)
                 << kseString
                 << QString::number(omaxd)
                 << getOmaxEString(fittingData.ConsumptionValues, fittingData.PriceValues)
                 << QString::number(pmaxd)
                 << getPmaxEString(fittingData.ConsumptionValues, fittingData.PriceValues)
                 << QString::number(avgerr)
                 << QString::number(r2)
                 << QString::number(sqrt(avgerr))
                 << QString::number(fittingData.PriceValues.count())
                 << "---"
                 << getKMessage(calculationSettings->settingsK)
                 << fittingData.Prices
                 << fittingData.Consumption;

    if (calculationSettings->settingsAlternativePmax)
    {
        // TODO

        //mTempHolder << QString::number(mObj->GetBootStrapPmax());
        //mTempHolder << QString::number(mObj->GetBootStrapPmaxExponentialSlope(q0, alpha, k, mObj->GetBootStrapPmax()));

        *mTempHolder << "---";
        *mTempHolder << "---";
    }
}

void FailedLinearDEOutput(QStringList * mTempHolder, int i, FittingData fittingData)
{
    mTempHolder->clear();

    *mTempHolder << QString::number(i + 1)
                << "Linear"
                << getBP0String(fittingData.ConsumptionValues, fittingData.PriceValues)
                << getBP1String(fittingData.ConsumptionValues, fittingData.PriceValues)
                << getOmaxEString(fittingData.ConsumptionValues, fittingData.PriceValues)
                << getPmaxEString(fittingData.ConsumptionValues, fittingData.PriceValues)
                << "---"
                << "---"
                << "---"
                << "---"
                << "---"
                << "---"
                << "---"
                << "---"
                << "---"
                << getIntensityString(fittingData.ConsumptionValues, fittingData.PriceValues)
                << "---"
                << "---"
                << "Fewer than 3 data points, skipped"
                << fittingData.Prices
                << fittingData.Consumption;
}

void SuccessfulLinearDEOutput(QStringList * mTempHolder, int i,
                              FittingData fittingData, double L, double a, double b)
{
    double pmaxd = (1 + b)/a;
    double omaxd = (L * pow(pmaxd, b)) / exp(a * pmaxd) * pmaxd;

    double pbar = getPbar(fittingData.PriceValues);

    mTempHolder->clear();
    *mTempHolder << QString::number(i + 1)
                << "Linear"
                << getBP0String(fittingData.ConsumptionValues, fittingData.PriceValues)
                << getBP1String(fittingData.ConsumptionValues, fittingData.PriceValues)
                << getOmaxEString(fittingData.ConsumptionValues, fittingData.PriceValues)
                << getPmaxEString(fittingData.ConsumptionValues, fittingData.PriceValues)
                << QString::number(L)
                << "---"
                << QString::number(b)
                << "---"
                << QString::number(a)
                << "---"
                << "---"
                << "---"
                << QString::number(b - (a * pbar))
                << getIntensityString(fittingData.ConsumptionValues, fittingData.PriceValues)
                << QString::number(omaxd)
                << QString::number(pmaxd)
                << "---"
                << fittingData.Prices
                << fittingData.Consumption;

}

void SuccessfulLinearLMOutput(QStringList * mTempHolder, int i, CalculationSettings * calculationSettings,
                              FittingData fittingData, double L, double Lse,
                              double a, double ase,
                              double b, double bse,
                              double avgerr, double r2,
                              int code)
{
    double pmaxd = (1 + b)/a;
    double omaxd = (L * pow(pmaxd, b)) / exp(a * pmaxd) * pmaxd;

    double pbar = getPbar(fittingData.PriceValues);

    mTempHolder->clear();
    *mTempHolder << QString::number(i + 1)
                << "Linear"
                << getBP0String(fittingData.ConsumptionValues, fittingData.PriceValues)
                << getBP1String(fittingData.ConsumptionValues, fittingData.PriceValues)
                << getOmaxEString(fittingData.ConsumptionValues, fittingData.PriceValues)
                << getPmaxEString(fittingData.ConsumptionValues, fittingData.PriceValues)
                << QString::number(L)
                << QString::number(Lse)
                << QString::number(b)
                << QString::number(bse)
                << QString::number(a)
                << QString::number(ase)
                << QString::number(r2)
                << "---"
                << QString::number(b - (a * pbar))
                << getIntensityString(fittingData.ConsumptionValues, fittingData.PriceValues)
                << QString::number(omaxd)
                << QString::number(pmaxd)
                << getCodeString(code)
                << fittingData.Prices
                << fittingData.Consumption;

}

void FailedLinearLMOutput(QStringList * mTempHolder, int i, FittingData fittingData, int code)
{
    mTempHolder->clear();
    *mTempHolder << QString::number(i + 1)
                << "Linear"
                << getBP0String(fittingData.ConsumptionValues, fittingData.PriceValues)
                << getBP1String(fittingData.ConsumptionValues, fittingData.PriceValues)
                << getOmaxEString(fittingData.ConsumptionValues, fittingData.PriceValues)
                << getPmaxEString(fittingData.ConsumptionValues, fittingData.PriceValues)
                << "---"
                << "---"
                << "---"
                << "---"
                << "---"
                << "---"
                << getIntensityString(fittingData.ConsumptionValues, fittingData.PriceValues)
                << "---"
                << "---"
                << getCodeString(code)
                << fittingData.Prices
                << fittingData.Consumption;
}

double GetDerivedK(CalculationSettings * calculationSettings, FittingData fittingData, real_1d_array savedGlobalFits)
{
    if (calculationSettings->settingsK == BehaviorK::Individual)
    {
        return (log10(fittingData.LocalMax) - log10(fittingData.LocalMin)) + 0.5;
    }
    else if (calculationSettings->settingsK == BehaviorK::Range)
    {
        return (log10(calculationSettings->globalMaxConsumption) - log10(calculationSettings->globalMinConsumption)) + 0.5;
    }
    else if (calculationSettings->settingsK == BehaviorK::Share)
    {
        return savedGlobalFits[savedGlobalFits.length() - 1];
    }
    else if (calculationSettings->settingsK == BehaviorK::Custom)
    {
        return calculationSettings->customK;
    }
    else
    {
        return -1;
    }
}

QString getFittingAlgorithm(CalculationSettings * calculationSettings)
{
    QString mWeightTag = calculationSettings->WeightSetting == WeightingMode::Weighted ? "-w)" : ")";

    if (calculationSettings->settingsFitting == FittingAlgorithm::Function)
    {
        return " (f" + mWeightTag;
    }
    else if (calculationSettings->settingsFitting == FittingAlgorithm::FunctionGradient)
    {
        return " (fg" + mWeightTag;
    }
    else if (calculationSettings->settingsFitting == FittingAlgorithm::FunctionGradientHessian)
    {
        return " (fgh" + mWeightTag;
    }
    else if (calculationSettings->settingsFitting == FittingAlgorithm::DifferentialEvolution)
    {
        return " (de)";
    }

    return " (---)";
}

/**
 * @brief CalculationWorker::getKMessage
 * @param call
 * @return
 *
 * Translate K methods into human-readable text
 */
QString getKMessage(BehaviorK call)
{
    if (call == BehaviorK::Individual)
    {
        return QString("Individual Log Range");
    }
    else if (call == BehaviorK::Range)
    {
        return QString("Overall Group Log Range");
    }
    else if (call == BehaviorK::Fit)
    {
        return QString("Individually Fitted");
    }
    else if (call == BehaviorK::Share)
    {
        return QString("Overall Group Fitted");
    }
    else
    {
        return QString("NA");
    }
}

/**
 * @brief CalculationWorker::getCodeString
 * @param code
 * @return
 *
 * Translate error codes for ALGLIB into human readable text
 */
QString getCodeString(int code)
{
    switch (code) {
        case -8:
            return QString("Error: Model error");
            break;

        case -7:
            return QString("Error: gradient verification failed");
            break;

        case 2:
            return QString("Success: relative step is no more than EpsX");
            break;

        case 5:
            return QString("Note: MaxIts steps was taken");
            break;

        case 7:
            return QString("Error: stopping conditions are too stringent, further improvement is impossible");
            break;

        default:
            return QString("Code: %1").arg((int) code);
            break;
    }
}

/**
 * @brief CalculationWorker::getPmaxEString
 * @param yValues
 * @param xValues
 * @return
 *
 * Gets empirical (data) pmax
 */
QString getPmaxEString(QList<double> &yValues, QList<double> &xValues)
{
    double maxExpendNumber = minrealnumber;
    double maxPrice = 0.0;

    for (int i = 0; i < yValues.length(); i++)
    {
        if ((xValues[i] * yValues[i]) >= maxExpendNumber)
        {
            maxExpendNumber = (xValues[i] * yValues[i]);

            maxPrice = xValues[i];
        }
    }

    return QString::number(maxPrice);
}

/**
 * @brief CalculationWorker::getOmaxEString
 * @param yValues
 * @param xValues
 * @return
 *
 * Get empirical (data) omax
 */
QString getOmaxEString(QList<double> &yValues, QList<double> &xValues)
{
    double maxExpendNumber = minrealnumber;

    for (int i = 0; i < yValues.length(); i++)
    {
        if ((xValues[i] * yValues[i]) >= maxExpendNumber)
        {
            maxExpendNumber = (xValues[i] * yValues[i]);
        }
    }

    return QString::number(maxExpendNumber);
}

/**
 * @brief CalculationWorker::getIntensityString
 * @param yValues
 * @param xValues
 * @return
 *
 * Get empirical (data) Q0
 */
QString getIntensityString(QList<double> &yValues, QList<double> &xValues)
{
    double minNonZeroPrice = maxrealnumber;

    QString consString = "NA";

    for (int i = 0; i < yValues.length(); i++)
    {
        if (xValues[i] < minNonZeroPrice)
        {
            minNonZeroPrice = xValues[i];
            consString = QString::number(yValues[i]);
        }
    }

    return consString;
}

/**
 * @brief CalculationWorker::getBP0String
 * @param yValues
 * @param xValues
 * @return
 *
 * Gets breakpoint (0)
 */
QString getBP0String(QList<double> &yValues, QList<double> &xValues)
{
    double maxNonZeroPrice = minrealnumber;

    QString priceString = "NA";

    for (int i = 0; i < yValues.length(); i++)
    {
        if (yValues[i] <= 0 && xValues[i] > maxNonZeroPrice)
        {
            maxNonZeroPrice = xValues[i];

            priceString = QString::number(maxNonZeroPrice);
        }
    }

    return priceString;
}

/**
 * @brief CalculationWorker::getBP1String
 * @param yValues
 * @param xValues
 * @return
 *
 * Gets breakpoint (1)
 */
QString getBP1String(QList<double> &yValues, QList<double> &xValues)
{
    double maxNonZeroPrice = minrealnumber;

    QString priceString = "NA";

    for (int i = 0; i < yValues.length(); i++)
    {
        if (yValues[i] > 0 && xValues[i] > maxNonZeroPrice)
        {
            maxNonZeroPrice = xValues[i];

            priceString = QString::number(maxNonZeroPrice);
        }
    }

    return priceString;
}

/**
 * @brief CalculationWorker::getPbar
 * @param yValues
 * @return
 *
 * Gets pBar for yvalues
 */
double getPbar(QList<double> &yValues)
{
    double sum = 0;

    for (int i = 0; i < yValues.count(); i++)
    {
        sum = sum + yValues.at(i);
    }

    return sum / (double) yValues.count();
}

#endif // DEMANDMEASURES_H
