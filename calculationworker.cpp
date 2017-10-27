#include <QDebug>

#include "calculationworker.h"
#include "qstringlist.h"
#include <QtWidgets>
#include <limits>

using namespace std;

struct QPairSecondComparer
{
    template<typename T1, typename T2>
    bool operator()(const QPair<T1,T2> &one, const QPair<T1,T2> &two) const
    {
        return one.second > two.second;
    }
};

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
    BruteForce mediumParamStartingValueArray[10000];
    BruteForce largeParamStartingValueArray[100000];
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
            return QString("NA");
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
    fitResult = new FitResult(modelType);
    QStringList tempList;

    QString strPrice;
    QString strConsumption;

    // Holder
    QString temp;

    // Params
    QList<double> mParams;

    // All series
    for (int i=0; i<mLocalStoredValues.length(); i++)
    {
        mObj->likelyQ0 = mLocalStoredValues[i].LocalMax;
        mObj->SetModel(calculationSettings.settingsModel);
        mObj->SetX(mLocalStoredValues[i].Prices.toUtf8().constData());
        mObj->SetY(mLocalStoredValues[i].Consumption.toUtf8().constData());

        qDebug() << mLocalStoredValues[i].Prices << " : " << mLocalStoredValues[i].Consumption << " : " << " min " << " max";

        mTempHolder.clear();

        if (calculationSettings.settingsModel == DemandModel::Exponential)
        {
            if (calculationSettings.settingsK == BehaviorK::Fit)
            {
                double lowerK = log10(0.5);
                double upperK = log10(log10(mLocalStoredValues[i].LocalMax) + 1);
                double kSpan = upperK - lowerK;
                double tempK = -1;

                double lowerQ = mLocalStoredValues[i].LocalMin;
                    lowerQ = (lowerQ > 0) ? lowerQ : 0.10;
                double upperQ = mLocalStoredValues[i].LocalMax;
                double tempQ = -1;

                double lowerA = 0.96;
                double upperA = 1.04;
                double aSpan = upperA - lowerA;

                int counter = 0;

                for (int k = 0; k < 10; k++)
                {
                    tempK = lowerK + (kSpan * (((double) i ) / 100.0));

                    for (int i = 0; i < 100; i++)
                    {
                        tempQ = lowerQ + (upperQ * (((double) i ) / 100.0));

                        for (int j = 0; j < 100; j++)
                        {
                            provisionalValues.largeParamStartingValueArray[counter].p1 = tempQ;
                            provisionalValues.largeParamStartingValueArray[counter].p2 = log((lowerA + (aSpan * (((double) j ) / 100.0))));
                            provisionalValues.largeParamStartingValueArray[counter].p3 = pow(10, tempK);

                            provisionalValues.mediumParamStartingValueArray[counter].err = mObj->getExponentialSSR(provisionalValues.mediumParamStartingValueArray[counter].p1,
                                                                                                                   provisionalValues.mediumParamStartingValueArray[counter].p2,
                                                                                                                   provisionalValues.mediumParamStartingValueArray[counter].p3);
                            counter++;
                        }
                    }
                }

                std::sort(provisionalValues.mediumParamStartingValueArray,
                          provisionalValues.mediumParamStartingValueArray + 100000,
                          &BruteSorter);

                double k = (log10(mLocalStoredValues[i].LocalMax) - log10(mLocalStoredValues[i].LocalMin)) + 0.5;

                mObj->SetBounds(QString("[+inf, +inf, %1]").arg(k).toUtf8().constData(), "[0.0001, -inf, 0.5]");

                mObj->FitExponentialWithK(QString("[%1, %2, %3]")
                                          .arg(provisionalValues.mediumParamStartingValueArray[0].p1)
                                          .arg(provisionalValues.mediumParamStartingValueArray[0].p2)
                                          .arg(provisionalValues.mediumParamStartingValueArray[0].p3)
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

                double upperQ = mLocalStoredValues[i].LocalMax;

                double tempQ = -1;

                double lowerA = 0.96;
                double upperA = 1.04;
                double aSpan = upperA - lowerA;

                int counter = 0;

                for (int i = 0; i < 100; i++)
                {
                    tempQ = lowerQ + (upperQ * (((double) i ) / 100.0));

                    for (int j = 0; j < 100; j++)
                    {
                        provisionalValues.mediumParamStartingValueArray[counter].p1 = tempQ;
                        provisionalValues.mediumParamStartingValueArray[counter].p2 = log((lowerA + (aSpan * (((double) j ) / 100.0))));
                        provisionalValues.mediumParamStartingValueArray[counter].p3 = mParams[0];

                        provisionalValues.mediumParamStartingValueArray[counter].err = mObj->getExponentialSSR(provisionalValues.mediumParamStartingValueArray[counter].p1,
                                                                                                               provisionalValues.mediumParamStartingValueArray[counter].p2,
                                                                                                               provisionalValues.mediumParamStartingValueArray[counter].p3);

                        counter++;
                    }
                }

                std::sort(provisionalValues.mediumParamStartingValueArray,
                          provisionalValues.mediumParamStartingValueArray + 10000,
                          &BruteSorter);

                mObj->SetBounds(QString("[+inf, +inf]").toUtf8().constData(), "[0.001, -inf]");
                mObj->FitExponential(QString("[%1, %2]")
                                    .arg(provisionalValues.mediumParamStartingValueArray[0].p1)
                                    .arg(provisionalValues.mediumParamStartingValueArray[0].p2)
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

                double upperQ = mLocalStoredValues[i].LocalMax;

                double tempQ = -1;

                double lowerA = 0.96;
                double upperA = 1.04;
                double aSpan = upperA - lowerA;

                int counter = 0;

                for (int i = 0; i < 100; i++)
                {
                    tempQ = lowerQ + (upperQ * (((double) i ) / 100.0));

                    for (int j = 0; j < 100; j++)
                    {
                        provisionalValues.mediumParamStartingValueArray[counter].p1 = tempQ;
                        provisionalValues.mediumParamStartingValueArray[counter].p2 = log((lowerA + (aSpan * (((double) j ) / 100.0))));
                        provisionalValues.mediumParamStartingValueArray[counter].p3 = mParams[0];

                        provisionalValues.mediumParamStartingValueArray[counter].err = mObj->getExponentiatedSSR(provisionalValues.mediumParamStartingValueArray[counter].p1,
                                                                                                                 provisionalValues.mediumParamStartingValueArray[counter].p2,
                                                                                                                 provisionalValues.mediumParamStartingValueArray[counter].p3);

                        counter++;
                    }
                }

                std::sort(provisionalValues.mediumParamStartingValueArray,
                          provisionalValues.mediumParamStartingValueArray + 10000,
                          &BruteSorter);

                mObj->SetBounds(QString("[+inf, +inf]").toUtf8().constData(), "[0.001, -inf]");
                mObj->FitExponentiated(QString("[%1, %2]")
                                      .arg(provisionalValues.mediumParamStartingValueArray[0].p1)
                                      .arg(provisionalValues.mediumParamStartingValueArray[0].p2)
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
                            << getBP0String(mLocalStoredValues[i].ConsumptionValues, mLocalStoredValues[i].PriceValues)
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
    }

    //emit workingResult(*fitResult);

//    for (int i=0; i<mLocalStoredValues.length(); i++)
//    {

/*
        mObj->likelyQ0 = localMax;

        mXString.append("]");
        mYString.append("]");

        mObj->SetModel(model);
        mObj->SetX(mXString.toUtf8().constData());
        mObj->SetY(mYString.toUtf8().constData());

        if (model == "linear")
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

                double pbar = getPbar(pricePointsTemp);

                mTempHolder.clear();
                mTempHolder << QString::number(i + 1)
                            << "Linear"
                            << getBP0String(valuePoints, pricePointsTemp)
                            << getBP1String(valuePoints, pricePointsTemp)
                            << getOmaxEString(valuePoints, pricePointsTemp)
                            << getPmaxEString(valuePoints, pricePointsTemp)
                            << QString::number(L)
                            << QString::number(mObj->GetReport().errpar[2])
                            << QString::number(b)
                            << QString::number(mObj->GetReport().errpar[1])
                            << QString::number(a)
                            << QString::number(mObj->GetReport().errpar[0])
                            << QString::number(mObj->GetReport().r2)
                            << "---"
                            << QString::number(b - (a * pbar))
                            << getIntensityString(valuePoints, pricePointsTemp)
                            << QString::number(omaxd)
                            << QString::number(pmaxd)
                            << getCodeString(mObj->GetInfo())
                            << mXString
                            << mYString;
                            //<< mYLogString;
            }
            else
            {
                mTempHolder.clear();
                mTempHolder << QString::number(i + 1)
                            << "Linear"
                            << getBP0String(valuePoints, pricePointsTemp)
                            << getBP1String(valuePoints, pricePointsTemp)
                            << getOmaxEString(valuePoints, pricePointsTemp)
                            << getPmaxEString(valuePoints, pricePointsTemp)
                            << ""
                            << ""
                            << ""
                            << ""
                            << "TO DO"
                            << ""
                            << getIntensityString(valuePoints, pricePointsTemp)
                            << ""
                            << ""
                            << getCodeString(mObj->GetInfo())
                            << mXString
                            << mYString;
                            //<< mYLogString;
            }

            allResults.append(mTempHolder);
        }
        else if (model == "hs")
        {
            kString.toDouble(&mKcheck);

            if (kString == "fit")
            {
                // K, Q0, Alpha
                mObj->SetBounds(QString("[%1, %2, +inf]").arg((log10(localMax) - log10(localMin)) + 0.5).arg(localMax * 2).toUtf8().constData(),
                                QString("[0.5, 0.001, -inf]").toUtf8().constData());

                mObj->FitExponentialWithK(QString("[%1, %2, 0.0001]").arg(((log10(localMax) - log10(localMin)) + 0.5)).arg(localMax).toUtf8().constData());
            }
            else
            {
                mObj->SetBounds(QString("[%1, +inf]").arg(localMax * 2).toUtf8().constData(), "[0.0001, -inf]");

                mParams.clear();

                if (kString == "ind")
                {
                    mParams << (log10(localMax) - log10(localMin)) + 0.5;
                }
                else if (kString == "range")
                {
                    mParams << (log10(globalMax) - log10(globalMin)) + 0.5;
                }
                else if (kString == "share")
                {
                    mParams << globalFitK;
                }
                else if (mKcheck)
                {
                    mParams << kString.toDouble();
                }
                else
                {
                    // Sanity check
                    return;
                }

                mObj->FitExponential(QString("[%1, 0.0001]").arg(localMax).toUtf8().constData(), mParams);
            }

            if ((int) mObj->GetInfo() == 2 || (int) mObj->GetInfo() == 5)
            {
                double alpha = (kString == "fit") ? mObj->GetState().c[2] : mObj->GetState().c[1];
                double alphase = (kString == "fit") ? mObj->GetReport().errpar[2] : mObj->GetReport().errpar[1];

                double k = (kString == "fit") ? mObj->GetState().c[0] : mParams.at(0);
                QString kse = (kString == "fit") ? QString::number(mObj->GetReport().errpar[0]) : "---";

                double q0 = (kString == "fit") ? mObj->GetState().c[1] : mObj->GetState().c[0];
                double q0se = (kString == "fit") ? mObj->GetReport().errpar[1] : mObj->GetReport().errpar[0];

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
                            << getBP1String(valuePoints, pricePointsTemp)
                            << QString::number(EV)
                            << getIntensityString(valuePoints, pricePointsTemp)
                            << QString::number(k)
                            << kse
                            << QString::number(omaxd)
                            << getOmaxEString(valuePoints, pricePointsTemp)
                            << QString::number(pmaxd)
                            << getPmaxEString(valuePoints, pricePointsTemp)
                            << QString::number(mObj->GetReport().rmserror)
                            << QString::number(mObj->GetReport().r2)
                            << QString::number(mObj->GetReport().avgerror)
                            << QString::number(pricePointsTemp.count())
                            << getCodeString(mObj->GetInfo())
                            << getKMessage(kString)
                            << mXString
                            << mYString;
            }
            else
            {
                mTempHolder.clear();
                mTempHolder << QString::number(i + 1)
                            << "Exponential"
                            << ""
                            << ""
                            << getBP1String(valuePoints, pricePointsTemp)
                            << ""
                            << getIntensityString(valuePoints, pricePointsTemp)
                            << ""
                            << ""
                            << getOmaxEString(valuePoints, pricePointsTemp)
                            << ""
                            << getPmaxEString(valuePoints, pricePointsTemp)
                            << ""
                            << ""
                            << ""
                            << QString::number(pricePointsTemp.count())
                            << getCodeString(mObj->GetInfo())
                            << getKMessage(kString)
                            << mXString
                            << mYString;
            }

            allResults.append(mTempHolder);
        }
        else if (model == "koff")
        {
            if (kString == "fit")
            {
                // K, Q0, Alpha
                mObj->SetBounds(QString("[%1, %2, +inf]").arg((log10(localMax) - log10(localMin)) + 0.5).arg(localMax * 2).toUtf8().constData(),
                                QString("[0.5, 0.001, -inf]").toUtf8().constData());

                mObj->FitExponentiatedWithK(QString("[%1, %2, 0.0001]").arg(((log10(localMax) - log10(localMin)) + 0.5)).arg(localMax / 2).toUtf8().constData());
            }
            else
            {
                kString.toDouble(&mKcheck);

                mObj->SetBounds(QString("[%1, +inf]").arg(localMax * 2).toUtf8().constData(), "[0.0001, -inf]");

                mParams.clear();

                if (kString == "ind")
                {
                    mParams << (log10(localMax) - log10(localMin)) + 0.5;
                }
                else if (kString == "range")
                {
                    mParams << (log10(globalMax) - log10(globalMin)) + 0.5;
                }
                else if (kString == "share")
                {
                    mParams << globalFitK;
                }
                else if (mKcheck)
                {
                    mParams << kString.toDouble();
                }
                else
                {
                    // Sanity check
                    return;
                }

                mObj->FitExponentiated(QString("[%1, 0.0001]").arg(localMax / 2).toUtf8().constData(), mParams);
            }

            if ((int) mObj->GetInfo() == 2 || (int) mObj->GetInfo() == 5)
            {
                double alpha = (kString == "fit") ? mObj->GetState().c[2] : mObj->GetState().c[1];
                double alphase = (kString == "fit") ? mObj->GetReport().errpar[2] : mObj->GetReport().errpar[1];
                double k = (kString == "fit") ? mObj->GetState().c[0] : mParams.at(0);
                QString kse = (kString == "fit") ? QString::number(mObj->GetReport().errpar[0]) : "---";

                double q0 = (kString == "fit") ? mObj->GetState().c[1] : mObj->GetState().c[0];
                double q0se = (kString == "fit") ? mObj->GetReport().errpar[1] : mObj->GetReport().errpar[0];
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
                            << getBP0String(valuePoints, pricePointsTemp)
                            << getBP1String(valuePoints, pricePointsTemp)
                            << QString::number(EV)
                            << getIntensityString(valuePoints, pricePointsTemp)
                            << QString::number(k)
                            << kse
                            << QString::number(omaxd)
                            << getOmaxEString(valuePoints, pricePointsTemp)
                            << QString::number(pmaxd)
                            << getPmaxEString(valuePoints, pricePointsTemp)
                            << QString::number(mObj->GetReport().rmserror)
                            << QString::number(mObj->GetReport().r2)
                            << QString::number(mObj->GetReport().avgerror)
                            << QString::number(pricePointsTemp.count())
                            << getCodeString(mObj->GetInfo())
                            << getKMessage(kString)
                            << mXString
                            << mYString;
            }
            else
            {
                mTempHolder.clear();
                mTempHolder << QString::number(i + 1)
                            << "Exponentiated"
                            << ""
                            << ""
                            << getBP0String(valuePoints, pricePointsTemp)
                            << getBP1String(valuePoints, pricePointsTemp)
                            << ""
                            << getIntensityString(valuePoints, pricePointsTemp)
                            << ""
                            << ""
                            << getOmaxEString(valuePoints, pricePointsTemp)
                            << ""
                            << getPmaxEString(valuePoints, pricePointsTemp)
                            << ""
                            << ""
                            << ""
                            << QString::number(pricePointsTemp.count())
                            << getCodeString(mObj->GetInfo())
                            << getKMessage(kString)
                            << mXString
                            << mYString;
            }

            allResults.append(mTempHolder);
        }


        emit workingResult(*fitResult);
    }
*/

    emit workFinished();

    // work
}
