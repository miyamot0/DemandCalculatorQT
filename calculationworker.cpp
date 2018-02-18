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

#include "calculationworker.h"
#include "qstringlist.h"
#include <QtWidgets>
#include <QVector>

using namespace std;

/**
 * @brief CalculationWorker::ptrCalculationWorker
 *
 * Reference for callback method
 */
CalculationWorker* CalculationWorker::ptrCalculationWorker = nullptr;

/**
 * @brief The BruteForce struct
 *
 * Struct for searching for model starts
 */
struct BruteForce {
  double p1 = 0;
  double p2 = 0;
  double p3 = 0;

  double err = 0;

  bool operator < (const BruteForce& r1) const {
      return (err < r1.err);
  }
};

/**
 * @brief The BruteForceValues struct
 *
 * Brute force struct
 */
struct BruteForceValues {
    BruteForce largeParamStartingValueArray[1000000];
};

/**
 * @brief BruteSorter
 * @param lhs
 * @param rhs
 * @return
 *
 * Sort brute forces, get lowest error to top
 */
bool BruteSorter(BruteForce const& lhs, BruteForce const& rhs)
{
    return lhs.err < rhs.err;
}

/**
 * @brief provisionalValues
 *
 * Struct instance
 */
BruteForceValues provisionalValues;

/**
 * @brief CalculationWorker::ResetSharedCounter
 * @param amt
 * @param max
 *
 * Resets a shared, reusable counter for callback
 */
void CalculationWorker::ResetSharedCounter(int amt, int max)
{
    if (ptrCalculationWorker)
    {
        ptrCalculationWorker->currentIteration = amt;
        ptrCalculationWorker->maximumIterations = max;
    }
}

/**
 * @brief CalculationWorker::ReportFx
 * @param c
 * @param func
 *
 * Callback reporting method
 */
void CalculationWorker::ReportFx(const real_1d_array &c, double func, void * )
{
    if (ptrCalculationWorker)
    {
        /*
        qDebug() << QString("Iteration %1 of %2: Err for k(%3) = %4")
                    .arg(ptrCalculationWorker->currentIteration + 1)
                    .arg(ptrCalculationWorker->maximumIterations)
                    .arg(c[c.length() - 1])
                    .arg(func);
        */

        emit ptrCalculationWorker->statusUpdate(QString("Iteration %1 of %2: Err for k(%3) = %4")
                                           .arg(ptrCalculationWorker->currentIteration + 1)
                                           .arg(ptrCalculationWorker->maximumIterations)
                                           .arg(c[c.length() - 1])
                                           .arg(func));

        ptrCalculationWorker->currentIteration = ptrCalculationWorker->currentIteration + 1;
    }
}

/**
 * @brief CalculationWorker::CalculationWorker
 * @param mStoredValues
 * @param mCalculationSettings
 *
 * Constructor for worker
 */
CalculationWorker::CalculationWorker(QList<FittingData> mStoredValues, CalculationSettings* mCalculationSettings)
{
    mLocalStoredValues = mStoredValues;
    calculationSettings = *mCalculationSettings;
    modelType = calculationSettings.settingsModel;
    mObj = new demandmodeling();
    mObj->SetFittingAlgorithm(mCalculationSettings->settingsFitting);
    mObj->SetWeightingModel(mCalculationSettings->WeightSetting);
    mObj->SetAlternativePmaxEnabled(mCalculationSettings->settingsAlternativePmax);

    ptrCalculationWorker = this;

    killSwitch = false;
}

/**
 * @brief CalculationWorker::getPbar
 * @param yValues
 * @return
 *
 * Gets pBar for yvalues
 */
double CalculationWorker::getPbar(QList<double> &yValues)
{
    double sum = 0;

    for (int i = 0; i < yValues.count(); i++)
    {
        sum = sum + yValues.at(i);
    }

    return sum / (double) yValues.count();
}

/**
 * @brief CalculationWorker::getKMessage
 * @param call
 * @return
 *
 * Translate K methods into human-readable text
 */
QString CalculationWorker::getKMessage(BehaviorK call)
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
QString CalculationWorker::getCodeString(ae_int_t code)
{
    switch ((int) code) {
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
QString CalculationWorker::getPmaxEString(QList<double> &yValues, QList<double> &xValues)
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
QString CalculationWorker::getOmaxEString(QList<double> &yValues, QList<double> &xValues)
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
QString CalculationWorker::getIntensityString(QList<double> &yValues, QList<double> &xValues)
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
QString CalculationWorker::getBP0String(QList<double> &yValues, QList<double> &xValues)
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
QString CalculationWorker::getBP1String(QList<double> &yValues, QList<double> &xValues)
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
 * @brief GetMagnitudeString
 * @param num
 * @return
 *
 * Gets string description of variable
 */
QString GetMagnitudeString(double num)
{
    int tempNum = (int) num;

    if (tempNum < 0)
    {
        return QString("1.0e%1").arg(tempNum);
    }
    else if (tempNum == 0)
    {
        return QString("1");
    }
    else if (tempNum > 0)
    {
        return QString("1.0e+%1").arg(tempNum);
    }

    return QString("1");
}

/**
 * @brief CalculationWorker::GetMagnitude
 * @param num
 * @return
 *
 * Get order of magnitude for number
 */
double CalculationWorker::GetMagnitude(double num)
{
    if (num <= 0)
    {
        return 0;
    }

    return log(num);
}

/**
 * @brief CalculationWorker::GetSharedK
 * @return
 *
 * Global fitting of K parameter
 */
double CalculationWorker::GetSharedK()
{
    int rows = 0, points = 0;

    QList<FittingData> holder;

    for (int i=0; i<mLocalStoredValues.length(); i++)
    {
        if (mLocalStoredValues[i].PriceValues.length() < 3)
        {
            continue;
        }

        rows++;

        holder.append(mLocalStoredValues[i]);

        for (int p=0; p<mLocalStoredValues[i].PriceValues.length(); p++)
        {
            points++;
        }
    }

    xReference.clear();
    xHolder.clear();
    yHolder.clear();

    referenceData.clear();
    location = 0;

    for (int i=0; i<holder.length(); i++)
    {
        for (int p=0; p<holder[i].PriceValues.length(); p++)
        {
            xReference << "[" + QString::number(cntr) + "]";

            referenceData << QString::number(location);

            xHolder << QString::number(holder[i].PriceValues[p]);
            yHolder << QString::number(holder[i].ConsumptionValues[p]);

            cntr++;
        }

        location++;
    }

    arrayHolder.clear();

    //Top is reference location, bottom is value
    arrayHolder.append(real_1d_array(QString("[%1]").arg(referenceData.join(',')).toUtf8().constData()));
    arrayHolder.append(real_1d_array(QString("[%1]").arg(xHolder.join(',')).toUtf8().constData()));

    bestParams.clear();
    bestParams.resize(holder.length());

    paramHolder.clear();
    paramHolder.resize(holder.length());

    counter = 0;

    if (calculationSettings.settingsModel == DemandModel::Exponential)
    {
        lowerK = log(0.5);
        upperK = log((log(calculationSettings.globalMaxConsumption) + 0.5) * 2);
        kSpan = upperK - lowerK;
        tempK = -1;

        lowerQ = calculationSettings.globalMinConsumption;
            lowerQ = (lowerQ > 0) ? lowerQ : 0.10;
        upperQ = calculationSettings.globalMaxConsumption * 1.5;
        qSpan = upperQ - lowerQ;
        tempQ = -1;

        lowerA = 0.99;
        upperA = 1.07;
        aSpan = upperA - lowerA;

        holdingBestK = lowerK;
        holdingBestSSR = maxrealnumber;
        holdingTempSSR = 0;

        for (int k = 0; k < 100; k++)
        {
            if (killSwitch)
            {
                return -1;
            }

            tempK = lowerK + (kSpan * (((double) k ) / 100.0));
            holdingTempSSR = 0;

            // through each series
            for (int series = 0; series < holder.length(); series++)
            {
                counter = 0;

                mObj->SetModel(DemandModel::Exponential);
                mObj->SetScalingEnabled(calculationSettings.ParameterScaling == ScalingMode::Enabled);

                mObj->SetX(holder[series].Prices.toUtf8().constData());
                mObj->SetY(holder[series].Consumption.toUtf8().constData());

                // check in for each series
                for (int i = 0; i < 100; i++)
                {
                    tempQ = lowerQ + (qSpan * (((double) i ) / 1000.0));

                    for (int j = 0; j < 100; j++)
                    {
                        provisionalValues.largeParamStartingValueArray[counter].p1 = tempQ;
                        provisionalValues.largeParamStartingValueArray[counter].p2 = log((lowerA + (aSpan * (((double) j ) / 100.0))));
                        provisionalValues.largeParamStartingValueArray[counter].p3 = exp(tempK);

                        provisionalValues.largeParamStartingValueArray[counter].err = mObj->getExponentialSSR(provisionalValues.largeParamStartingValueArray[counter].p1,
                                                                                                              provisionalValues.largeParamStartingValueArray[counter].p2,
                                                                                                              provisionalValues.largeParamStartingValueArray[counter].p3);
                        counter++;
                    }
                }

                // best of for the series is here
                std::sort(provisionalValues.largeParamStartingValueArray,
                          provisionalValues.largeParamStartingValueArray + 10000,
                          &BruteSorter);

                paramHolder[series].first = provisionalValues.largeParamStartingValueArray[0].p1;
                paramHolder[series].second = provisionalValues.largeParamStartingValueArray[0].p2;

                holdingTempSSR += provisionalValues.largeParamStartingValueArray[0].err;
            }

            if (holdingTempSSR <= holdingBestSSR)
            {
                holdingBestK = exp(tempK);

                for (int p = 0; p < paramHolder.length(); p++)
                {
                    bestParams[p].first = paramHolder[p].first;
                    bestParams[p].second = paramHolder[p].second;
                }

                holdingBestSSR = holdingTempSSR;
            }

            emit statusUpdate(QString("Scanning for starts (%1%)").arg(k + 1));
        }

        starts.clear();
        bl.clear();
        bu.clear();
        scale.clear();

        q0Ave = 0;
        alphaAve = 0;

        for (int i = 0; i < bestParams.length() / 2; i++)
        {
            q0Ave = q0Ave + bestParams[0 + i].first;
            alphaAve = alphaAve + bestParams[1 + i].second;
        }

        q0Ave /= (((double) bestParams.length()) / 2);
        alphaAve /= (((double) bestParams.length()) / 2);


        for (int i = 0; i < bestParams.length(); i++)
        {
            bl << "-inf" << "-inf";
            bu << "+inf" << "+inf";

            //scale << QString::number(GetMagnitude(q0Ave))
            //      << QString::number(GetMagnitude(alphaAve));
            scale << GetMagnitudeString(GetMagnitude(q0Ave))
                  << GetMagnitudeString(GetMagnitude(alphaAve));

            starts << QString::number(bestParams[i].first)
                   << QString::number(bestParams[i].second);
        }

        bl << "0.5";
        bu << QString::number((log(calculationSettings.globalMaxConsumption) + 0.5) * 2);

        //scale << QString::number(GetMagnitude(holdingBestK));
        scale << GetMagnitudeString(GetMagnitude(holdingBestK));

        starts << QString::number(holdingBestK);

        mObj->SetScalingEnabled(calculationSettings.ParameterScaling == ScalingMode::Enabled);
        mObj->SetX(QString("[" + xReference.join(",") + "]").toUtf8().constData());
        mObj->SetY(QString("[" + yHolder.join(",") + "]").toUtf8().constData());
        mObj->SetBounds(QString("[" + bu.join(',') + "]").toUtf8().constData(), QString("[" + bl.join(',') + "]").toUtf8().constData());
        mObj->SetScale(QString("[" + scale.join(',') + "]").toUtf8().constData());

        emit statusUpdate(QString("Fitting K parameter globally..."));

        try
        {
            mObj->FitSharedExponentialK(QString("[" + starts.join(',') + "]").toUtf8().constData(),
                                        &arrayHolder,
                                        &ReportFx,
                                        &ResetSharedCounter);
        }
        catch (alglib::ap_error e)
        {
            return -1;
        }

        savedGlobalFits = mObj->GetParams();
    }
    else if (calculationSettings.settingsModel == DemandModel::Exponentiated)
    {
        lowerK = log(0.5);
        upperK = log((log(calculationSettings.globalMaxConsumption) + 0.5) * 2);
        kSpan = upperK - lowerK;
        tempK = -1;

        lowerQ = calculationSettings.globalMinConsumption;
            lowerQ = (lowerQ > 0) ? lowerQ : 0.10;
        upperQ = calculationSettings.globalMaxConsumption * 1.5;
        qSpan = upperQ - lowerQ;
        tempQ = -1;

        lowerA = 0.99;
        upperA = 1.07;
        aSpan = upperA - lowerA;

        holdingBestK = lowerK;
        holdingBestSSR = maxrealnumber;
        holdingTempSSR = 0;

        for (int k = 0; k < 100; k++)
        {
            if (killSwitch)
            {
                return -1;
            }

            tempK = lowerK + (kSpan * (((double) k ) / 100.0));
            holdingTempSSR = 0;

            // through each series
            for (int series = 0; series < holder.length(); series++)
            {
                counter = 0;

                mObj->SetModel(DemandModel::Exponentiated);
                mObj->SetScalingEnabled(calculationSettings.ParameterScaling == ScalingMode::Enabled);

                mObj->SetX(holder[series].Prices.toUtf8().constData());
                mObj->SetY(holder[series].Consumption.toUtf8().constData());

                // check in for each series
                for (int i = 0; i < 100; i++)
                {
                    tempQ = lowerQ + (qSpan * (((double) i ) / 100.0));

                    for (int j = 0; j < 100; j++)
                    {
                        provisionalValues.largeParamStartingValueArray[counter].p1 = tempQ;
                        provisionalValues.largeParamStartingValueArray[counter].p2 = log((lowerA + (aSpan * (((double) j ) / 100.0))));
                        provisionalValues.largeParamStartingValueArray[counter].p3 = exp(tempK);

                        provisionalValues.largeParamStartingValueArray[counter].err = mObj->getExponentiatedSSR(provisionalValues.largeParamStartingValueArray[counter].p1,
                                                                                                                provisionalValues.largeParamStartingValueArray[counter].p2,
                                                                                                                provisionalValues.largeParamStartingValueArray[counter].p3);
                        counter++;
                    }
                }

                // best of for the series is here
                std::sort(provisionalValues.largeParamStartingValueArray,
                          provisionalValues.largeParamStartingValueArray + 10000,
                          &BruteSorter);

                paramHolder[series].first = provisionalValues.largeParamStartingValueArray[0].p1;
                paramHolder[series].second = provisionalValues.largeParamStartingValueArray[0].p2;

                holdingTempSSR += provisionalValues.largeParamStartingValueArray[0].err;
            }

            if (holdingTempSSR <= holdingBestSSR)
            {
                holdingBestK = exp(tempK);

                for (int p = 0; p < paramHolder.length(); p++)
                {
                    bestParams[p].first = paramHolder[p].first;
                    bestParams[p].second = paramHolder[p].second;
                }

                holdingBestSSR = holdingTempSSR;
            }

            emit statusUpdate(QString("Scanning for starts (%1%)").arg(k + 1));
        }

        starts.clear();
        bl.clear();
        bu.clear();
        scale.clear();

        q0Ave = 0;
        alphaAve = 0;

        for (int i = 0; i < bestParams.length() / 2; i++)
        {
            q0Ave = q0Ave + bestParams[0 + i].first;
            alphaAve = alphaAve + bestParams[1 + i].second;
        }

        q0Ave /= (((double) bestParams.length()) / 2);
        alphaAve /= (((double) bestParams.length()) / 2);

        for (int i = 0; i < bestParams.length(); i++)
        {
            bl << "-inf" << "-inf";
            bu << "+inf" << "+inf";

            //scale << QString::number(GetMagnitude(q0Ave))
            //      << QString::number(GetMagnitude(alphaAve));
            scale << GetMagnitudeString(GetMagnitude(q0Ave))
                  << GetMagnitudeString(GetMagnitude(alphaAve));

            starts << QString::number(bestParams[i].first)
                   << QString::number(bestParams[i].second);
        }

        bl << "0.5";
        bu << QString::number((log(calculationSettings.globalMaxConsumption) + 0.5) * 2);

        //scale << QString::number(GetMagnitude(holdingBestK));
        scale << GetMagnitudeString(GetMagnitude(holdingBestK));

        starts << QString::number(holdingBestK);

        mObj->SetScalingEnabled(calculationSettings.ParameterScaling == ScalingMode::Enabled);
        mObj->SetX(QString("[" + xReference.join(",") + "]").toUtf8().constData());
        mObj->SetY(QString("[" + yHolder.join(",") + "]").toUtf8().constData());
        mObj->SetBounds(QString("[" + bu.join(',') + "]").toUtf8().constData(), QString("[" + bl.join(',') + "]").toUtf8().constData());
        mObj->SetScale(QString("[" + scale.join(',') + "]").toUtf8().constData());

        emit statusUpdate(QString("Fitting K parameter globally..."));

        try
        {
            mObj->FitSharedExponentiatedK(QString("[" + starts.join(',') + "]").toUtf8().constData(),
                                          &arrayHolder,
                                          &ReportFx,
                                          &ResetSharedCounter);
        }
        catch (alglib::ap_error e)
        {
            return -1;
        }

        savedGlobalFits = mObj->GetParams();
    }

    return savedGlobalFits[savedGlobalFits.length() - 1];
}

/**
 * @brief CalculationWorker::TerminateOperations
 *
 * Kill switch
 */
void CalculationWorker::TerminateOperations()
{
    killSwitch = true;
}

/**
 * @brief CalculationWorker::startWork
 *
 * SLOT: start working, emit work signal
 */
void CalculationWorker::startWork()
{
    emit workStarted();
}

/**
 * @brief CalculationWorker::working
 *
 * SLOT: working, emit status signals
 */
void CalculationWorker::working()
{
    bool failed = false;

    sharedHolder.clear();

    cntr = 0;
    values = 0;

    for (int i=0; i<mLocalStoredValues.length(); i++)
    {
        for (int p=0; p<mLocalStoredValues[i].PriceValues.length(); p++)
        {
            values++;
        }
    }

    // Checking on shared k, if successful
    if (calculationSettings.settingsK == BehaviorK::Share)
    {
        if (GetSharedK() == -1)
        {
            emit workFinished(ErrorCode);

            return;
        }
    }    

    // All series
    for (int i=0; i<mLocalStoredValues.length(); i++)
    {
        if (killSwitch)
        {
            emit workFinished(CancelCode);

            return;
        }       

        if (calculationSettings.settingsModel != DemandModel::Linear && mLocalStoredValues[i].PriceValues.length() < 3)
        {
            mTempHolder.clear();
            mTempHolder << QString::number(i + 1)
                << ((calculationSettings.settingsModel == DemandModel::Exponential) ? "Exponential" + getFittingAlgorithm(calculationSettings.settingsFitting) :
                                                                                      "Exponentiated" + getFittingAlgorithm(calculationSettings.settingsFitting))
                << "---"
                << "---"
                << "---"
                << "---"
                << getBP1String(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                << "---"
                << getIntensityString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                << "---"
                << "---"
                << "---"
                << getOmaxEString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                << "---"
                << getPmaxEString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                << "---"
                << "---"
                << "---"
                << "---"
                << "Fewer than 3 data points, skipped"
                << getKMessage(calculationSettings.settingsK)
                << mLocalStoredValues[i].Prices
                << mLocalStoredValues[i].Consumption;

            if (calculationSettings.settingsAlternativePmax)
            {
                mTempHolder << "---";
                mTempHolder << "---";
            }

            emit workingResult(mTempHolder);

            continue;
        }
        else if (calculationSettings.settingsModel == DemandModel::Linear && mLocalStoredValues[i].PriceValues.length() < 3)
        {
            mTempHolder.clear();
            mTempHolder << QString::number(i + 1)
                        << "Linear"
                        << getBP0String(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                        << getBP1String(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                        << getOmaxEString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                        << getPmaxEString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                        << "---"
                        << "---"
                        << "---"
                        << "---"
                        << "---"
                        << "---"
                        << "---"
                        << "---"
                        << "---"
                        << getIntensityString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                        << "---"
                        << "---"
                        << "---"
                        << mLocalStoredValues[i].Prices
                        << mLocalStoredValues[i].Consumption;

            emit workingResult(mTempHolder);

            continue;
        }

        mObj->SetModel(calculationSettings.settingsModel);

        mObj->SetX(mLocalStoredValues[i].Prices.toUtf8().constData());
        mObj->SetY(mLocalStoredValues[i].Consumption.toUtf8().constData());

        if (calculationSettings.WeightSetting == WeightingMode::Weighted)
        {
            mObj->SetW(mLocalStoredValues[i].Weights.toUtf8().constData());
        }

        mTempHolder.clear();

        if (calculationSettings.settingsModel == DemandModel::Exponential)
        {
            if (calculationSettings.settingsK == BehaviorK::Fit)
            {
                lowerK = log(0.5);
                upperK = log((log(mLocalStoredValues[i].LocalMax) + 0.5) * 2);
                kSpan = upperK - lowerK;
                tempK = -1;

                lowerQ = mLocalStoredValues[i].LocalMin;
                    lowerQ = (lowerQ > 0) ? lowerQ : 0.10;
                upperQ = mLocalStoredValues[i].LocalMax * 1.5;
                qSpan = upperQ - lowerQ;
                tempQ = -1;

                lowerA = 0.99;
                upperA = 1.07;
                aSpan = upperA - lowerA;

                counter = 0;

                for (int k = 0; k < 100; k++)
                {
                    tempK = lowerK + (kSpan * (((double) k ) / 100.0));

                    for (int i = 0; i < 100; i++)
                    {
                        tempQ = lowerQ + (qSpan * (((double) i ) / 1000.0));

                        for (int j = 0; j < 100; j++)
                        {
                            provisionalValues.largeParamStartingValueArray[counter].p1 = tempQ;
                            provisionalValues.largeParamStartingValueArray[counter].p2 = log((lowerA + (aSpan * (((double) j ) / 100.0))));
                            provisionalValues.largeParamStartingValueArray[counter].p3 = exp(tempK);

                            provisionalValues.largeParamStartingValueArray[counter].err = mObj->getExponentialSSR(provisionalValues.largeParamStartingValueArray[counter].p1,
                                                                                                                  provisionalValues.largeParamStartingValueArray[counter].p2,
                                                                                                                  provisionalValues.largeParamStartingValueArray[counter].p3);
                            counter++;
                        }
                    }
                }

                std::sort(provisionalValues.largeParamStartingValueArray,
                          provisionalValues.largeParamStartingValueArray + 1000000,
                          &BruteSorter);

                k = (log(mLocalStoredValues[i].LocalMax) + 0.5) * 2;

                mObj->SetBounds(QString("[+inf, +inf, %1]").arg(k).toUtf8().constData(),
                                "[0.0001, -inf, 0.5]");

                scale.clear();

                scale << GetMagnitudeString(GetMagnitude(provisionalValues.largeParamStartingValueArray[0].p1));
                scale << GetMagnitudeString(GetMagnitude(provisionalValues.largeParamStartingValueArray[0].p2));
                scale << GetMagnitudeString(GetMagnitude(provisionalValues.largeParamStartingValueArray[0].p3));

                mObj->SetScalingEnabled(calculationSettings.ParameterScaling == ScalingMode::Enabled);
                mObj->SetScale(QString("[" + scale.join(',') + "]").toUtf8().constData());

                try
                {
                    mObj->FitExponentialWithK(QString("[%1, %2, %3]")
                                              .arg(provisionalValues.largeParamStartingValueArray[0].p1)
                                              .arg(provisionalValues.largeParamStartingValueArray[0].p2)
                                              .arg(provisionalValues.largeParamStartingValueArray[0].p3)
                                              .toUtf8().constData());

                    failed = false;
                }
                catch (alglib::ap_error err)
                {
                    failed = true;
                }
            }
            else
            {
                mParams.clear();

                if (calculationSettings.settingsK == BehaviorK::Individual)
                {
                    mParams << (log10(mLocalStoredValues[i].LocalMax) - log10(mLocalStoredValues[i].LocalMin)) + 0.5;
                }
                else if (calculationSettings.settingsK == BehaviorK::Range)
                {
                    mParams << (log10(calculationSettings.globalMaxConsumption) - log10(calculationSettings.globalMinConsumption)) + 0.5;
                }
                else if (calculationSettings.settingsK == BehaviorK::Share)
                {
                    mParams << savedGlobalFits[savedGlobalFits.length() - 1];
                }
                else if (calculationSettings.settingsK == BehaviorK::Custom)
                {
                    mParams << calculationSettings.customK;
                }

                lowerQ = mLocalStoredValues[i].LocalMin;
                    lowerQ = (lowerQ > 0) ? lowerQ : 0.10;

                upperQ = mLocalStoredValues[i].LocalMax * 1.5;
                qSpan = upperQ - lowerQ;

                tempQ = -1;

                lowerA = 0.99;
                upperA = 1.07;
                aSpan = upperA - lowerA;

                counter = 0;

                for (int i = 0; i < 1000; i++)
                {
                    tempQ = lowerQ + (qSpan * (((double) i ) / 1000.0));

                    for (int j = 0; j < 1000; j++)
                    {
                        provisionalValues.largeParamStartingValueArray[counter].p1 = tempQ;
                        provisionalValues.largeParamStartingValueArray[counter].p2 = log((lowerA + (aSpan * (((double) j ) / 100.0))));
                        provisionalValues.largeParamStartingValueArray[counter].p3 = mParams[0];

                        provisionalValues.largeParamStartingValueArray[counter].err = mObj->getExponentialSSR(provisionalValues.largeParamStartingValueArray[counter].p1,
                                                                                                              provisionalValues.largeParamStartingValueArray[counter].p2,
                                                                                                              provisionalValues.largeParamStartingValueArray[counter].p3);

                        counter++;
                    }
                }

                std::sort(provisionalValues.largeParamStartingValueArray,
                          provisionalValues.largeParamStartingValueArray + 1000000,
                          &BruteSorter);

                mObj->SetBounds(QString("[+inf, +inf]").toUtf8().constData(), "[0.001, -inf]");

                scale.clear();

                scale << GetMagnitudeString(GetMagnitude(provisionalValues.largeParamStartingValueArray[0].p1));
                scale << GetMagnitudeString(GetMagnitude(provisionalValues.largeParamStartingValueArray[0].p2));

                mObj->SetScalingEnabled(calculationSettings.ParameterScaling == ScalingMode::Enabled);
                mObj->SetScale(QString("[" + scale.join(',') + "]").toUtf8().constData());

                try
                {
                    mObj->FitExponential(QString("[%1, %2]")
                                        .arg(provisionalValues.largeParamStartingValueArray[0].p1)
                                        .arg(provisionalValues.largeParamStartingValueArray[0].p2)
                                        .toUtf8()
                                        .constData(), mParams);

                    failed = false;
                }
                catch (alglib::ap_error err)
                {
                    failed = true;
                }
            }

            if (failed)
            {
                mTempHolder.clear();
                mTempHolder << QString::number(i + 1);
            }
            else if ((int) mObj->GetInfo() == 2 || (int) mObj->GetInfo() == 5)
            {
                alpha = mObj->GetState().c[1];
                alphase = mObj->GetReport().errpar[1];

                k = (calculationSettings.settingsK == BehaviorK::Fit) ? mObj->GetState().c[2] : mParams.at(0);
                kse = (calculationSettings.settingsK == BehaviorK::Fit) ? QString::number(mObj->GetReport().errpar[2]) : "---";

                q0 = mObj->GetState().c[0];
                q0se = mObj->GetReport().errpar[0];

                pmaxd = 1/(q0 * alpha * pow(k, 1.5)) * (0.083 * k + 0.65);
                omaxd = (pow(10, (log10(q0) + (k * (exp(-alpha * q0 * pmaxd) - 1))))) * pmaxd;

                EV = 1/(alpha * pow(k, 1.5) * 100);

                mTempHolder.clear();
                mTempHolder << QString::number(i + 1)
                            << "Exponential" + getFittingAlgorithm(calculationSettings.settingsFitting)
                            << QString::number(alpha)
                            << QString::number(alphase)
                            << QString::number(q0)
                            << QString::number(q0se)
                            << getBP1String(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << QString::number(EV)
                            << getIntensityString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << QString::number(k)
                            << kse
                            << QString::number(omaxd)
                            << getOmaxEString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << QString::number(pmaxd)
                            << getPmaxEString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << QString::number(mObj->GetReport().rmserror)
                            << QString::number(mObj->GetReport().r2)
                            << QString::number(mObj->GetReport().avgerror)
                            << QString::number(mLocalStoredValues[i].PriceValues.count())
                            << getCodeString(mObj->GetInfo())
                            << getKMessage(calculationSettings.settingsK)
                            << mLocalStoredValues[i].Prices
                            << mLocalStoredValues[i].Consumption;

                if (calculationSettings.settingsAlternativePmax)
                {
                    mTempHolder << QString::number(mObj->GetBootStrapPmax());
                    mTempHolder << QString::number(mObj->GetBootStrapPmaxExponentialSlope(q0, alpha, k, mObj->GetBootStrapPmax()));
                }
            }
            else
            {
                mTempHolder << QString::number(i + 1)
                            << "Exponential" + getFittingAlgorithm(calculationSettings.settingsFitting)
                            << ""
                            << ""
                            << getBP1String(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << ""
                            << getIntensityString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << ""
                            << ""
                            << getOmaxEString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << ""
                            << getPmaxEString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << ""
                            << ""
                            << ""
                            << QString::number(mLocalStoredValues[i].PriceValues.count())
                            << getCodeString(mObj->GetInfo())
                            << getKMessage(calculationSettings.settingsK)
                            << mLocalStoredValues[i].Prices
                            << mLocalStoredValues[i].Consumption;

                if (calculationSettings.settingsAlternativePmax)
                {
                    mTempHolder << "---";
                    mTempHolder << "---";
                }
            }

            emit workingResult(mTempHolder);
        }
        else if (calculationSettings.settingsModel == DemandModel::Exponentiated)
        {
            if (calculationSettings.settingsK == BehaviorK::Fit)
            {
                lowerK = log(0.5);
                upperK = log((log(mLocalStoredValues[i].LocalMax) + 0.5) * 2);
                kSpan = upperK - lowerK;
                tempK = -1;

                lowerQ = mLocalStoredValues[i].LocalMin;
                    lowerQ = (lowerQ > 0) ? lowerQ : 0.10;
                upperQ = mLocalStoredValues[i].LocalMax * 1.5;
                qSpan = upperQ - lowerQ;
                tempQ = -1;

                lowerA = 0.99;
                upperA = 1.07;
                aSpan = upperA - lowerA;

                counter = 0;

                for (int k = 0; k < 100; k++)
                {
                    tempK = lowerK + (kSpan * (((double) k ) / 100.0));

                    for (int i = 0; i < 100; i++)
                    {
                        tempQ = lowerQ + (qSpan * (((double) i ) / 1000.0));

                        for (int j = 0; j < 100; j++)
                        {
                            provisionalValues.largeParamStartingValueArray[counter].p1 = tempQ;
                            provisionalValues.largeParamStartingValueArray[counter].p2 = log((lowerA + (aSpan * (((double) j ) / 100.0))));
                            provisionalValues.largeParamStartingValueArray[counter].p3 = exp(tempK);

                            provisionalValues.largeParamStartingValueArray[counter].err = mObj->getExponentiatedSSR(provisionalValues.largeParamStartingValueArray[counter].p1,
                                                                                                                    provisionalValues.largeParamStartingValueArray[counter].p2,
                                                                                                                    provisionalValues.largeParamStartingValueArray[counter].p3);
                            counter++;
                        }
                    }
                }

                std::sort(provisionalValues.largeParamStartingValueArray,
                          provisionalValues.largeParamStartingValueArray + 1000000,
                          &BruteSorter);

                k = (log(mLocalStoredValues[i].LocalMax) + 0.5) * 2;

                mObj->SetBounds(QString("[+inf, +inf, %1]").arg(k).toUtf8().constData(),
                                "[0.0001, -inf, 0.5]");

                scale.clear();

                scale << GetMagnitudeString(GetMagnitude(provisionalValues.largeParamStartingValueArray[0].p1));
                scale << GetMagnitudeString(GetMagnitude(provisionalValues.largeParamStartingValueArray[0].p2));
                scale << GetMagnitudeString(GetMagnitude(provisionalValues.largeParamStartingValueArray[0].p3));

                mObj->SetScalingEnabled(calculationSettings.ParameterScaling == ScalingMode::Enabled);
                mObj->SetScale(QString("[" + scale.join(',') + "]").toUtf8().constData());

                try
                {
                    mObj->FitExponentiatedWithK(QString("[%1, %2, %3]")
                                                .arg(provisionalValues.largeParamStartingValueArray[0].p1)
                                                .arg(provisionalValues.largeParamStartingValueArray[0].p2)
                                                .arg(provisionalValues.largeParamStartingValueArray[0].p3)
                                                .toUtf8().constData());

                    failed = false;
                }
                catch (alglib::ap_error err)
                {
                    failed = true;
                }
            }
            else
            {
                mParams.clear();

                if (calculationSettings.settingsK == BehaviorK::Individual)
                {
                    mParams << (log10(mLocalStoredValues[i].LocalMax) - log10(mLocalStoredValues[i].LocalMin)) + 0.5;
                }
                else if (calculationSettings.settingsK == BehaviorK::Range)
                {
                    mParams << (log10(calculationSettings.globalMaxConsumption) - log10(calculationSettings.globalMinConsumption)) + 0.5;
                }
                else if (calculationSettings.settingsK == BehaviorK::Share)
                {
                    mParams << savedGlobalFits[savedGlobalFits.length() - 1];
                }
                else if (calculationSettings.settingsK == BehaviorK::Custom)
                {
                    mParams << calculationSettings.customK;
                }

                lowerQ = mLocalStoredValues[i].LocalMin;
                    lowerQ = (lowerQ > 0) ? lowerQ : 0.10;

                upperQ = mLocalStoredValues[i].LocalMax * 1.5;
                qSpan = upperQ - lowerQ;

                tempQ = -1;

                lowerA = 0.99;
                upperA = 1.07;
                aSpan = upperA - lowerA;

                counter = 0;

                for (int i = 0; i < 1000; i++)
                {
                    tempQ = lowerQ + (qSpan * (((double) i ) / 1000.0));

                    for (int j = 0; j < 1000; j++)
                    {
                        provisionalValues.largeParamStartingValueArray[counter].p1 = tempQ;
                        provisionalValues.largeParamStartingValueArray[counter].p2 = log((lowerA + (aSpan * (((double) j ) / 1000.0))));
                        provisionalValues.largeParamStartingValueArray[counter].p3 = mParams[0];

                        provisionalValues.largeParamStartingValueArray[counter].err = mObj->getExponentiatedSSR(provisionalValues.largeParamStartingValueArray[counter].p1,
                                                                                                                provisionalValues.largeParamStartingValueArray[counter].p2,
                                                                                                                provisionalValues.largeParamStartingValueArray[counter].p3);

                        counter++;
                    }
                }

                std::sort(provisionalValues.largeParamStartingValueArray,
                          provisionalValues.largeParamStartingValueArray + 1000000,
                          &BruteSorter);

                mObj->SetBounds(QString("[+inf, +inf]").toUtf8().constData(), "[0.001, -inf]");

                scale.clear();

                scale << GetMagnitudeString(GetMagnitude(provisionalValues.largeParamStartingValueArray[0].p1));
                scale << GetMagnitudeString(GetMagnitude(provisionalValues.largeParamStartingValueArray[0].p2));

                mObj->SetScalingEnabled(calculationSettings.ParameterScaling == ScalingMode::Enabled);
                mObj->SetScale(QString("[" + scale.join(',') + "]").toUtf8().constData());

                try
                {
                    mObj->FitExponentiated(QString("[%1, %2]")
                                          .arg(provisionalValues.largeParamStartingValueArray[0].p1)
                                          .arg(provisionalValues.largeParamStartingValueArray[0].p2)
                                          .toUtf8()
                                          .constData(), mParams);

                    failed = false;
                }
                catch (alglib::ap_error err)
                {
                    failed = true;
                }
            }

            if ((int) mObj->GetInfo() == 2 || (int) mObj->GetInfo() == 5)
            {
                alpha = mObj->GetState().c[1];
                alphase = mObj->GetReport().errpar[1];
                k = (calculationSettings.settingsK == BehaviorK::Fit) ? mObj->GetState().c[2] : mParams.at(0);
                kse = (calculationSettings.settingsK == BehaviorK::Fit) ? QString::number(mObj->GetReport().errpar[2]) : "---";

                q0 = mObj->GetState().c[0];
                q0se = mObj->GetReport().errpar[0];
                pmaxd = 1/(q0 * alpha * pow(k, 1.5)) * (0.083 * k + 0.65);
                omaxd = (q0 * (pow(10,(k * (exp(-alpha * q0 * pmaxd) - 1))))) * pmaxd;

                EV = 1/(alpha * pow(k, 1.5) * 100);

                mTempHolder.clear();
                mTempHolder << QString::number(i + 1)
                            << "Exponentiated" + getFittingAlgorithm(calculationSettings.settingsFitting)
                            << QString::number(alpha)
                            << QString::number(alphase)
                            << QString::number(q0)
                            << QString::number(q0se)
                            << getBP0String(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << getBP1String(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << QString::number(EV)
                            << getIntensityString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << QString::number(k)
                            << kse
                            << QString::number(omaxd)
                            << getOmaxEString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << QString::number(pmaxd)
                            << getPmaxEString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << QString::number(mObj->GetReport().rmserror)
                            << QString::number(mObj->GetReport().r2)
                            << QString::number(mObj->GetReport().avgerror)
                            << QString::number(mLocalStoredValues[i].PriceValues.count())
                            << getCodeString(mObj->GetInfo())
                            << getKMessage(calculationSettings.settingsK)
                            << mLocalStoredValues[i].Prices
                            << mLocalStoredValues[i].Consumption;

                if (calculationSettings.settingsAlternativePmax)
                {
                    mTempHolder << QString::number(mObj->GetBootStrapPmax());
                    mTempHolder << QString::number(mObj->GetBootStrapPmaxExponentiatedSlope(q0, alpha, k, mObj->GetBootStrapPmax()));
                }
            }
            else
            {
                mTempHolder.clear();
                mTempHolder << QString::number(i + 1)
                            << "Exponentiated" + getFittingAlgorithm(calculationSettings.settingsFitting)
                            << ""
                            << ""
                            << ""
                            << ""
                            << getBP0String(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << getBP1String(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << ""
                            << getIntensityString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << ""
                            << ""
                            << ""
                            << getOmaxEString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << ""
                            << getPmaxEString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << ""
                            << ""
                            << ""
                            << QString::number(mLocalStoredValues[i].PriceValues.count())
                            << getCodeString(mObj->GetInfo())
                            << getKMessage(calculationSettings.settingsK)
                            << mLocalStoredValues[i].Prices
                            << mLocalStoredValues[i].Consumption;

                if (calculationSettings.settingsAlternativePmax)
                {
                    mTempHolder << "---";
                    mTempHolder << "---";
                }
            }

            emit workingResult(mTempHolder);
        }
        else if (calculationSettings.settingsModel == DemandModel::Linear)
        {
            mObj->SetBounds("[+inf, +inf, +inf]", "[-inf, -inf, -inf]");
            mObj->FitLinear("[1, 1, 1]");
            if ((int) mObj->GetInfo() == 2 || (int) mObj->GetInfo() == 5)
            {
                double a = mObj->GetState().c[0];
                double b = mObj->GetState().c[1];
                double L = mObj->GetState().c[2];
                double pmaxd = (1 + b)/a;
                double omaxd = (L * pow(pmaxd, b)) / exp(a * pmaxd) * pmaxd;

                double pbar = getPbar(mLocalStoredValues[i].PriceValues);

                mTempHolder.clear();
                mTempHolder << QString::number(i + 1)
                            << "Linear"
                            << getBP0String(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << getBP1String(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << getOmaxEString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << getPmaxEString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << QString::number(L)
                            << QString::number(mObj->GetReport().errpar[2])
                            << QString::number(b)
                            << QString::number(mObj->GetReport().errpar[1])
                            << QString::number(a)
                            << QString::number(mObj->GetReport().errpar[0])
                            << QString::number(mObj->GetReport().r2)
                            << "---"
                            << QString::number(b - (a * pbar))
                            << getIntensityString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << QString::number(omaxd)
                            << QString::number(pmaxd)
                            << getCodeString(mObj->GetInfo())
                            << mLocalStoredValues[i].Prices
                            << mLocalStoredValues[i].Consumption;
            }
            else
            {
                mTempHolder.clear();
                mTempHolder << QString::number(i + 1)
                            << "Linear"
                            << getBP0String(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << getBP1String(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << getOmaxEString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << getPmaxEString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << ""
                            << ""
                            << ""
                            << ""
                            << "TO DO"
                            << ""
                            << getIntensityString(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
                            << ""
                            << ""
                            << getCodeString(mObj->GetInfo())
                            << mLocalStoredValues[i].Prices
                            << mLocalStoredValues[i].Consumption;
            }

            emit workingResult(mTempHolder);
        }
    }

    emit workFinished(SuccessCode);
}
