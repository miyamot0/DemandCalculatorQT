#include <QDebug>

#include "calculationworker.h"
#include "qstringlist.h"
#include <QtWidgets>
#include <QVector>

using namespace std;

struct BruteForce {
  double p1 = 0;
  double p2 = 0;
  double p3 = 0;

  double err = 0;

  bool operator < (const BruteForce& r1) const {
      return (err < r1.err);
  }
};

struct BruteForceValues {
    //BruteForce smallParamStartingValueArray[1000];
    //BruteForce mediumParamStartingValueArray[10000];
    BruteForce largeParamStartingValueArray[1000000];
};

BruteForceValues provisionalValues;

bool BruteSorter(BruteForce const& lhs, BruteForce const& rhs) {
    return lhs.err < rhs.err;
}

CalculationWorker::CalculationWorker(QList<FittingData> mStoredValues, CalculationSettings* mCalculationSettings)
{
    mLocalStoredValues = mStoredValues;
    calculationSettings = *mCalculationSettings;
    modelType = calculationSettings.settingsModel;
    mObj = new demandmodeling();
}

double CalculationWorker::getPbar(QList<double> &yValues)
{
    double sum = 0;

    for (int i = 0; i < yValues.count(); i++)
    {
        sum = sum + yValues.at(i);
    }

    return sum / (double) yValues.count();
}

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

QString CalculationWorker::getCodeString(ae_int_t code)
{
    switch ((int) code) {
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

void CalculationWorker::startWork()
{
    emit workStarted();
}

void CalculationWorker::working()
{
    real_1d_array savedGlobalFits;

    // Params
    QList<double> mParams;

    QStringList bu,
                bl,
                starts,
                xHolder,
                yHolder,
                xReference;

    int cntr = 0;

    sharedHolder.clear();

    QList<QStringList> tempHolder;

    int values = 0;

    for (int i=0; i<mLocalStoredValues.length(); i++)
    {
        for (int p=0; p<mLocalStoredValues[i].PriceValues.length(); p++)
        {
            values++;
        }
    }

    QVector<QVector<QString>> tempMatrix;
    tempMatrix.reserve(mLocalStoredValues.length());

    if (calculationSettings.settingsK == BehaviorK::Share)
    {
        for (int i=0; i<mLocalStoredValues.length(); i++)
        {
            QVector<QString> temp;
            temp.resize(values);
            temp.fill("0");

            tempMatrix.append(temp);

            for (int p=0; p<mLocalStoredValues[i].PriceValues.length(); p++)
            {
                tempMatrix[i][cntr] = "1";

                xReference << "[" + QString::number(cntr) + "]";

                xHolder << QString::number(mLocalStoredValues[i].PriceValues[p]);
                yHolder << QString::number(mLocalStoredValues[i].ConsumptionValues[p]);

                cntr++;
            }
        }

        QList<real_1d_array> arrayHolder;
        for (int v = 0; v < tempMatrix.length(); v++)
        {
            arrayHolder.append(real_1d_array(QString("[%1]").arg(tempMatrix[v].toList().join(",")).toUtf8().constData()));
        }

        arrayHolder.append(real_1d_array(QString("[%1]").arg(xHolder.join(',')).toUtf8().constData()));



        QVector<QPair<double, double>> bestParams;
        bestParams.resize(mLocalStoredValues.length());

        QVector<QPair<double, double>> paramHolder;
        paramHolder.resize(mLocalStoredValues.length());

        int counter = 0;

        if (calculationSettings.settingsModel == DemandModel::Exponential)
        {
            // Exponential only
            double lowerK = 0.5;
            double upperK = log10(calculationSettings.globalMaxConsumption) * 2;
            double kSpan = upperK - lowerK;
            double tempK = -1;

            double lowerQ = calculationSettings.globalMinConsumption;
                lowerQ = (lowerQ > 0) ? lowerQ : 0.10;
            double upperQ = calculationSettings.globalMaxConsumption * 1.5;
            double qSpan = upperQ - lowerQ;
            double tempQ = -1;

            double lowerA = 0.99;
            double upperA = 1.07;
            double aSpan = upperA - lowerA;

            double holdingBestK = lowerK,
                   holdingBestSSR = maxrealnumber,
                   holdingTempSSR = 0;

            for (int k = 0; k < 100; k++)
            {
                tempK = lowerK + (kSpan * (((double) k ) / 100.0));
                holdingTempSSR = 0;

                // through each series
                for (int series = 0; series < mLocalStoredValues.length(); series++)
                {
                    counter = 0;
                    mObj->SetModel(DemandModel::Exponential);

                    mObj->SetX(mLocalStoredValues[series].Prices.toUtf8().constData());
                    mObj->SetY(mLocalStoredValues[series].Consumption.toUtf8().constData());

                    // check in for each series
                    for (int i = 0; i < 100; i++)
                    {
                        tempQ = lowerQ + (qSpan * (((double) i ) / 1000.0));

                        for (int j = 0; j < 100; j++)
                        {
                            provisionalValues.largeParamStartingValueArray[counter].p1 = tempQ;
                            provisionalValues.largeParamStartingValueArray[counter].p2 = log((lowerA + (aSpan * (((double) j ) / 100.0))));
                            provisionalValues.largeParamStartingValueArray[counter].p3 = pow(10, tempK);

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
                    holdingBestK = tempK;

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

            for (int i = 0; i < bestParams.length(); i++)
            {
                bl << "-inf" << "-inf";
                bu << "+inf" << "+inf";

                starts << QString::number(bestParams[i].first)
                       << QString::number(bestParams[i].second);
            }

            bl << "0.5";
            bu << "+inf";

            starts << QString::number(holdingBestK);

            QString tempStarts  = "[" + starts.join(',') + "]";
            //qDebug() << "Temp Starts: " << tempStarts;

            mObj->SetX(QString("[" + xReference.join(",") + "]").toUtf8().constData());
            //qDebug() << QString("[" + xReference.join(",") + "]");

            mObj->SetY(QString("[" + yHolder.join(",") + "]").toUtf8().constData());
            //qDebug() << QString("[" + yHolder.join(",") + "]");

            QString tempUpper = QString("[" + bu.join(',') + "]");
            //qDebug() << "Temp Upper: "<< tempUpper.toUtf8().constData();

            QString tempLower = QString("[" + bl.join(',') + "]");
            //qDebug() << "Temp Lower: "<< tempLower.toUtf8().constData();

            mObj->SetBounds(tempUpper.toUtf8().constData(), tempLower.toUtf8().constData());

            mObj->FitSharedExponentialK(tempStarts.toUtf8().constData(), &arrayHolder);

            savedGlobalFits = mObj->GetParams();
        }
        else if (calculationSettings.settingsModel == DemandModel::Exponentiated)
        {
            // Exponential only
            double lowerK = 0.5;
            double upperK = log10(calculationSettings.globalMaxConsumption) * 2;
            double kSpan = upperK - lowerK;
            double tempK = -1;

            double lowerQ = calculationSettings.globalMinConsumption;
                lowerQ = (lowerQ > 0) ? lowerQ : 0.10;

            double upperQ = calculationSettings.globalMaxConsumption * 1.5;

            double qSpan = upperQ - lowerQ;

            double tempQ = -1;

            double lowerA = 0.99;
            double upperA = 1.07;
            double aSpan = upperA - lowerA;

            double holdingBestK = lowerK,
                   holdingBestSSR = maxrealnumber,
                   holdingTempSSR = 0;

            for (int k = 0; k < 100; k++)
            {
                tempK = lowerK + (kSpan * (((double) k ) / 100.0));
                holdingTempSSR = 0;

                // through each series
                for (int series = 0; series < mLocalStoredValues.length(); series++)
                {
                    counter = 0;
                    mObj->SetModel(DemandModel::Exponentiated);

                    mObj->SetX(mLocalStoredValues[series].Prices.toUtf8().constData());
                    mObj->SetY(mLocalStoredValues[series].Consumption.toUtf8().constData());

                    // check in for each series
                    for (int i = 0; i < 100; i++)
                    {
                        tempQ = lowerQ + (qSpan * (((double) i ) / 100.0));

                        for (int j = 0; j < 100; j++)
                        {
                            provisionalValues.largeParamStartingValueArray[counter].p1 = tempQ;
                            provisionalValues.largeParamStartingValueArray[counter].p2 = log((lowerA + (aSpan * (((double) j ) / 100.0))));
                            provisionalValues.largeParamStartingValueArray[counter].p3 = pow(10, tempK);

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
                    holdingBestK = tempK;

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

            for (int i = 0; i < bestParams.length(); i++)
            {
                bl << "-inf" << "-inf";
                bu << "+inf" << "+inf";

                starts << QString::number(bestParams[i].first)
                       << QString::number(bestParams[i].second);
            }

            bl << "0.5";
            bu << "+inf";

            starts << QString::number(holdingBestK);

            QString tempStarts  = "[" + starts.join(',') + "]";
            qDebug() << "Temp Starts: " << tempStarts;

            mObj->SetX(QString("[" + xReference.join(",") + "]").toUtf8().constData());
            qDebug() << QString("[" + xReference.join(",") + "]");

            mObj->SetY(QString("[" + yHolder.join(",") + "]").toUtf8().constData());
            qDebug() << QString("[" + yHolder.join(",") + "]");

            QString tempUpper = QString("[" + bu.join(',') + "]");
            qDebug() << "Temp Upper: "<< tempUpper.toUtf8().constData();

            QString tempLower = QString("[" + bl.join(',') + "]");
            qDebug() << "Temp Lower: "<< tempLower.toUtf8().constData();

            mObj->SetBounds(tempUpper.toUtf8().constData(), tempLower.toUtf8().constData());

            mObj->FitSharedExponentiatedK(tempStarts.toUtf8().constData(), &arrayHolder);

            savedGlobalFits = mObj->GetParams();
        }
    }

    // All series
    for (int i=0; i<mLocalStoredValues.length(); i++)
    {
        mObj->likelyQ0 = mLocalStoredValues[i].LocalMax;
        mObj->SetModel(calculationSettings.settingsModel);
        mObj->SetX(mLocalStoredValues[i].Prices.toUtf8().constData());
        mObj->SetY(mLocalStoredValues[i].Consumption.toUtf8().constData());

        mTempHolder.clear();

        if (calculationSettings.settingsModel == DemandModel::Exponential)
        {
            if (calculationSettings.settingsK == BehaviorK::Fit)
            {
                double lowerK = 0.5;
                double upperK = log10(mLocalStoredValues[i].LocalMax) * 2;
                double kSpan = upperK - lowerK;
                double tempK = -1;

                double lowerQ = mLocalStoredValues[i].LocalMin;
                    lowerQ = (lowerQ > 0) ? lowerQ : 0.10;
                double upperQ = mLocalStoredValues[i].LocalMax * 1.5;
                double qSpan = upperQ - lowerQ;
                double tempQ = -1;

                double lowerA = 0.99;
                double upperA = 1.07;
                double aSpan = upperA - lowerA;

                int counter = 0;

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
                            provisionalValues.largeParamStartingValueArray[counter].p3 = pow(10, tempK);

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

                double k = (log10(mLocalStoredValues[i].LocalMax) - log10(mLocalStoredValues[i].LocalMin)) + 0.5;

                mObj->SetBounds(QString("[+inf, +inf, %1]").arg(k).toUtf8().constData(), "[0.0001, -inf, 0.5]");

                mObj->FitExponentialWithK(QString("[%1, %2, %3]")
                                          .arg(provisionalValues.largeParamStartingValueArray[0].p1)
                                          .arg(provisionalValues.largeParamStartingValueArray[0].p2)
                                          .arg(provisionalValues.largeParamStartingValueArray[0].p3)
                                          .toUtf8().constData());
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
                    //mParams << calculationSettings.globalFitK;
                }
                else if (calculationSettings.settingsK == BehaviorK::Custom)
                {
                    mParams << calculationSettings.customK;
                }

                double lowerQ = mLocalStoredValues[i].LocalMin;
                    lowerQ = (lowerQ > 0) ? lowerQ : 0.10;

                double upperQ = mLocalStoredValues[i].LocalMax * 1.5;
                double qSpan = upperQ - lowerQ;

                double tempQ = -1;

                double lowerA = 0.99;
                double upperA = 1.07;
                double aSpan = upperA - lowerA;

                int counter = 0;

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
                mObj->FitExponential(QString("[%1, %2]")
                                    .arg(provisionalValues.largeParamStartingValueArray[0].p1)
                                    .arg(provisionalValues.largeParamStartingValueArray[0].p2)
                                    .toUtf8()
                                    .constData(), mParams);
            }

            if ((int) mObj->GetInfo() == 2 || (int) mObj->GetInfo() == 5)
            {
                double alpha = mObj->GetState().c[1];
                double alphase = mObj->GetReport().errpar[1];

                double k = (calculationSettings.settingsK == BehaviorK::Fit) ? mObj->GetState().c[2] : mParams.at(0);
                QString kse = (calculationSettings.settingsK == BehaviorK::Fit) ? QString::number(mObj->GetReport().errpar[2]) : "---";

                double q0 = mObj->GetState().c[0];
                double q0se = mObj->GetReport().errpar[0];

                double pmaxd = 1/(q0 * alpha * pow(k, 1.5)) * (0.083 * k + 0.65);
                double omaxd = (pow(10, (log10(q0) + (k * (exp(-alpha * q0 * pmaxd) - 1))))) * pmaxd;

                double EV = 1/(alpha * pow(k, 1.5) * 100);

                mTempHolder.clear();
                mTempHolder << QString::number(i + 1)
                            << "Exponential"
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
            }
            else
            {
                mTempHolder << QString::number(i + 1)
                            << "Exponential"
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
            }

            emit workingResult(mTempHolder);
        }
        else if (calculationSettings.settingsModel == DemandModel::Exponentiated)
        {
            if (calculationSettings.settingsK == BehaviorK::Fit)
            {
                double lowerK = 0.5;
                double upperK = log10(mLocalStoredValues[i].LocalMax) * 2;
                double kSpan = upperK - lowerK;
                double tempK = -1;

                double lowerQ = mLocalStoredValues[i].LocalMin;
                    lowerQ = (lowerQ > 0) ? lowerQ : 0.10;
                double upperQ = mLocalStoredValues[i].LocalMax * 1.5;
                double qSpan = upperQ - lowerQ;
                double tempQ = -1;

                double lowerA = 0.99;
                double upperA = 1.07;
                double aSpan = upperA - lowerA;

                int counter = 0;

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
                            provisionalValues.largeParamStartingValueArray[counter].p3 = pow(10, tempK);

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

                provisionalValues.largeParamStartingValueArray[0].p2 = (provisionalValues.largeParamStartingValueArray[0].p2 == 0) ? 0.000000001 :
                                                                                                                                     provisionalValues.largeParamStartingValueArray[0].p2;

                double k = (log10(mLocalStoredValues[i].LocalMax) - log10(mLocalStoredValues[i].LocalMin)) + 0.5;

                mObj->SetBounds(QString("[+inf, +inf, %1]").arg(k).toUtf8().constData(),
                                "[0.0001, -inf, 0.5]");

                mObj->FitExponentiatedWithK(QString("[%1, %2, %3]")
                                            .arg(provisionalValues.largeParamStartingValueArray[0].p1)
                                            .arg(provisionalValues.largeParamStartingValueArray[0].p2)
                                            .arg(provisionalValues.largeParamStartingValueArray[0].p3)
                                            .toUtf8().constData());
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
                    //mParams << calculationSettings.globalFitK;
                }
                else if (calculationSettings.settingsK == BehaviorK::Custom)
                {
                    mParams << calculationSettings.customK;
                }

                double lowerQ = mLocalStoredValues[i].LocalMin;
                    lowerQ = (lowerQ > 0) ? lowerQ : 0.10;

                double upperQ = mLocalStoredValues[i].LocalMax * 1.5;
                double qSpan = upperQ - lowerQ;

                double tempQ = -1;

                double lowerA = 0.99;
                double upperA = 1.07;
                double aSpan = upperA - lowerA;

                int counter = 0;

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

                provisionalValues.largeParamStartingValueArray[0].p2 = (provisionalValues.largeParamStartingValueArray[0].p2 == 0) ? 0.000000001 :
                                                                                                                                     provisionalValues.largeParamStartingValueArray[0].p2;

                mObj->SetBounds(QString("[+inf, +inf]").toUtf8().constData(), "[0.001, -inf]");
                mObj->FitExponentiated(QString("[%1, %2]")
                                      .arg(provisionalValues.largeParamStartingValueArray[0].p1)
                                      .arg(provisionalValues.largeParamStartingValueArray[0].p2)
                                      .toUtf8()
                                      .constData(), mParams);
            }

            if ((int) mObj->GetInfo() == 2 || (int) mObj->GetInfo() == 5)
            {
                double alpha = mObj->GetState().c[1];
                double alphase = mObj->GetReport().errpar[1];
                double k = (calculationSettings.settingsK == BehaviorK::Fit) ? mObj->GetState().c[2] : mParams.at(0);
                QString kse = (calculationSettings.settingsK == BehaviorK::Fit) ? QString::number(mObj->GetReport().errpar[2]) : "---";

                double q0 = mObj->GetState().c[0];
                double q0se = mObj->GetReport().errpar[0];
                double pmaxd = 1/(q0 * alpha * pow(k, 1.5)) * (0.083 * k + 0.65);
                double omaxd = (q0 * (pow(10,(k * (exp(-alpha * q0 * pmaxd) - 1))))) * pmaxd;

                double EV = 1/(alpha * pow(k, 1.5) * 100);

                mTempHolder.clear();
                mTempHolder << QString::number(i + 1)
                            << "Exponentiated"
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
            }
            else
            {
                mTempHolder.clear();
                mTempHolder << QString::number(i + 1)
                            << "Exponentiated"
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

    emit workFinished();
}
