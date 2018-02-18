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

   ====================================================================================

   ALGLIB 3.11.0 (source code generated 2017-05-11)
   Copyright (c) Sergey Bochkanov (ALGLIB project).

   >>> SOURCE LICENSE >>>
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation (www.fsf.org); either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   A copy of the GNU General Public License is available at
   http://www.fsf.org/licensing/licenses
   >>> END OF LICENSE >>>

  */

#ifndef DEMANDMODELING_H
#define DEMANDMODELING_H

#include <QMap>
#include <iostream>
#include "interpolation.h"

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "optimization.h"

#include "calculationsettings.h"
#include "lambertresult.h"

using namespace alglib;

#define M_E 2.71828182845904523536
#define GSL_DBL_EPSILON 2.2204460492503131e-16

class demandmodeling
{
public:
    demandmodeling();

    void SetModel(DemandModel model);
    void SetX(const char *mString);
    void SetY(const char *mString);
    void SetW(const char *mString);
    void SetStarts(const char *mString);
    void SetScale(const char *mString);
    void SetBounds(const char *mUpperString,
                   const char *mLowerString);
    void SetScalingEnabled(bool value);
    void SetAlternativePmaxEnabled(bool value);
    void SetFittingAlgorithm(FittingAlgorithm value);
    void SetWeightingModel(WeightingMode value);

    double getExponentialSSR(double Q0, double alpha, double k);
    double getExponentiatedSSR(double Q0, double alpha, double k);

    double series_eval(double r);
    LambertResult halley_iteration(double x, double w_initial, int max_iters);
    LambertResult gsl_sf_lambert_W0(double x);

    void TestMixedModel();

    real_1d_array GetParams();
    lsfitstate GetState();
    ae_int_t GetInfo();
    lsfitreport GetReport();

    double getMinimumConsumption();
    double getMaximumConsumption();

    void FitLinear(const char *mStarts);

    void FitExponential(const char *mStarts, QList<double> mParams);
    void FitExponentialWithK(const char *mStarts);

    void FitSharedExponentialK(const char *mStarts,
                               QList<real_1d_array> *arrayHolder,
                               void (*caller)(const real_1d_array &, double, void *),
                               void (*reset)(int, int));
    void FitSharedExponentiatedK(const char *mStarts,
                                 QList<real_1d_array> *arrayHolder,
                                 void (*caller)(const real_1d_array &, double, void *),
                                 void (*reset)(int, int));

    void FitExponentiated(const char *mStarts, QList<double> mParams);
    void FitExponentiatedWithK(const char *mStarts);

    //void BootstrapPmaxExponential(double Q0, double A, double K);
    //void BootstrapPmaxExponentiated(double Q0, double A, double K);

    double ExactSolutionPmax(double Q0, double A, double K);

    double GetBootStrapPmax();
    double GetBootStrapPmaxExponentialSlope(double Q, double A, double K, double pMax);
    double GetBootStrapPmaxExponentiatedSlope(double Q0, double A, double K, double pMax);

    QStringList GetSteinTest(QStringList &x, QStringList &y);

    bool raisedFlag = false;

    int sharedIterationMax = 1000;

private:
    real_2d_array x;
    real_1d_array y;
    real_1d_array w;

    real_1d_array yStored;
    real_1d_array c;

    real_1d_array s;

    real_1d_array bndl;
    real_1d_array bndu;

    real_1d_array trueX;

    bool scalingParameters = false;

    bool alternativePmax = false;

    ae_int_t maxits = 5000;
    ae_int_t info;
    lsfitstate state;
    lsfitreport rep;

    double epsx = 0.0000001;
    double diffstep = 0.0000001;

    // Work stuff

    minlbfgsstate workOutputState;
    minlbfgsreport workOutputRep;

    real_1d_array workOutputX;

    // Stein criteria

    QStringList mSteinReturn;

    QList<QPair<double, double>> mModPoints;

    bool containsZeroes = false;
    bool checkX, checkY;
    double tempX, tempY;
    int numPosValues = 0;
    double deltaQ;
    double bounceThreshold;
    int bounceCount = 0;
    double bounceScore;
    int reversalCount = 0;

    QString deltaQPass;
    QString bouncePass;
    QString reversalpass;

    DemandModel modelMode;

    FittingAlgorithm fittingAlgorithm;

    WeightingMode weightingMode;

    int passingMeasures = 0;

    double deltaq = 0.025;
    double bounce = 0.10;
    double reversals = 0;
    double ncons0 = 2;

    double exactSolutionPmax;
};

#endif // DEMANDMODELING_H
