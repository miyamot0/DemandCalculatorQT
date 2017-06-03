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
    void SetLowerUpperBounds(const char *mUpperString, const char *mLowerString);

    real_1d_array GetParams();
    lsfitstate GetState();
    ae_int_t GetInfo();
    lsfitreport GetReport();

    void FitLinear(const char *mStarts);
    void FitExponential(const char *mStarts, QList<double> mParams);
    void FitExponentialWithK(const char *mStarts);
    void FitExponentiated(const char *mStarts, QList<double> mParams);
    void FitExponentiatedWithK(const char *mStarts);

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

    double epsx = 0.000001;
    double diffstep = 0.00001;
};

#endif // DEMANDMODELING_H
