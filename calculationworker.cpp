//#include <QDebug>

#include "calculationworker.h"
#include "qstringlist.h"
#include <QtWidgets>

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

double CalculationWorker::getPbar(QStringList &xValues)
{
    QSet<QString> mPrices = QSet<QString>::fromList(xValues);

    double sum = 0;

    for (int i = 0; i < mPrices.count(); i++)
    {
        sum = sum + mPrices.values().at(i).toDouble();
    }

    return sum / (double) mPrices.count();
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
    // Params
    QList<double> mParams;

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
                    mParams << calculationSettings.globalFitK;
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
                    mParams << calculationSettings.globalFitK;
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
    }

    emit workFinished();
}
