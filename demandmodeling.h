#ifndef DEMANDMODELING_H
#define DEMANDMODELING_H

#include <QDebug>
#include <QMap>
#include <iostream>
#include "interpolation.h"

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "optimization.h"

using namespace alglib;

class demandmodeling
{
public:
    demandmodeling();

    void SetX(const char *mString);
    void SetY(const char *mString);
    void SetStarts(const char *mString);
    void SetBounds(const char *mUpperString, const char *mLowerString);

    real_1d_array GetParams();
    lsfitstate GetState();
    ae_int_t GetInfo();
    lsfitreport GetReport();

    void FitLinear(const char *mStarts);
    void FitExponential(const char *mStarts, QList<double> mParams);
    void FitExponentialWithK(const char *mStarts);
    void FitExponentiated(const char *mStarts, QList<double> mParams);
    void FitExponentiatedWithK(const char *mStarts);

    QStringList GetSteinTest(QStringList &x, QStringList &y);

    bool raisedFlag = false;

private:
    real_2d_array x;
    real_1d_array y;
    real_1d_array c;

    real_1d_array bndl;
    real_1d_array bndu;

    ae_int_t maxits = 1000;
    ae_int_t info;
    lsfitstate state;
    lsfitreport rep;

    double epsx = 0.000000001;
    double diffstep = 0.00000001;

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

    int passingMeasures = 0;

    double deltaq = 0.025;
    double bounce = 0.10;
    double reversals = 0;
    double ncons0 = 2;
};

#endif // DEMANDMODELING_H
