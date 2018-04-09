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

#ifndef CALCULATIONWORKER_H
#define CALCULATIONWORKER_H

#include <QObject>
#include <QVector>

#include "Modeling/demandmodeling.h"
#include "Models/fittingdata.h"

#include "Models/calculationsettings.h"

class CalculationWorker : public QObject
{
    Q_OBJECT

public:
    CalculationWorker(QList<FittingData> mStoredValues, CalculationSettings *mCalculationSettings);

    QList<FittingData> mLocalStoredValues;
    QStringList mTempHolder;

    CalculationSettings calculationSettings;

    demandmodeling *mObj;
    DemandModel modelType;

    double getPbar(QList<double> &yValues);
    double GetSharedK();

    QString getCodeString(ae_int_t code);
    QString getKMessage(BehaviorK call);
    QString getPmaxEString(QList<double> &yValues, QList<double> &xValues);
    QString getOmaxEString(QList<double> &yValues, QList<double> &xValues);
    QString getIntensityString(QList<double> &yValues, QList<double> &xValues);
    QString getBP0String(QList<double> &yValues, QList<double> &xValues);
    QString getBP1String(QList<double> &yValues, QList<double> &xValues);

    double GetMagnitude(double num);

    QList<real_1d_array> sharedHolder;

    static void ResetSharedCounter(int amt, int max);
    static void ReportFx(const real_1d_array &c, double func, void *);

    real_1d_array savedGlobalFits;

    QList<real_1d_array> arrayHolder;

    // Params
    QList<double> mParams;

    QVector<QPair<double, double>> bestParams;
    QVector<QPair<double, double>> paramHolder;

    QStringList bu,
                bl,
                starts,
                xHolder,
                yHolder,
                xReference,
                scale;

    int cntr;
    int values;
    int counter;

    double lowerK;
    double upperK;
    double kSpan;
    double tempK;

    double lowerQ;
    double upperQ;
    double qSpan;
    double tempQ;

    double lowerA;
    double upperA;
    double aSpan;

    double holdingBestK;
    double holdingBestSSR;
    double holdingTempSSR;

    double alpha;
    double alphase;

    double k;
    QString kse;
    double q0;
    double q0se;
    double pmaxd;
    double omaxd;
    double EV;

    int currentIteration;
    int maximumIterations;

    double q0Ave;
    double alphaAve;

    int location;

    // location of points
    QStringList referenceData;

    void TerminateOperations();

    const int SuccessCode = 1;
    const int CancelCode = -1;
    const int ErrorCode = -2;

private:
    static CalculationWorker* ptrCalculationWorker;

    QString getFittingAlgorithm(FittingAlgorithm value)
    {
        QString mWeightTag = calculationSettings.WeightSetting == WeightingMode::Weighted ? "-w)" : ")";

        if (value == FittingAlgorithm::Function)
        {
            return " (f" + mWeightTag;
        }
        else if (value == FittingAlgorithm::FunctionGradient)
        {
            return " (fg" + mWeightTag;
        }
        else if (value == FittingAlgorithm::FunctionGradientHessian)
        {
            return " (fgh" + mWeightTag;
        }
        else if (value == FittingAlgorithm::DifferentialEvolution)
        {
            return " (de)";
        }

        return " (---)";
    }

    bool killSwitch = false;

    unsigned int popSize = 100;

    std::vector<double> result;

signals:
    void workStarted();
    void statusUpdate(const QString &value);
    void workingResult(const QStringList &value);
    void workFinished(int code);

public slots:
    void startWork();
    void working();
};

#endif // CALCULATIONWORKER_H
