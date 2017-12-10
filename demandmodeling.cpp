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

#include "demandmodeling.h"
//#include <QDebug>

void demandmodeling::SetModel(DemandModel model)
{
    modelMode = model;
}

void demandmodeling::SetX(const char *mString)
{
    x = mString;
}

void demandmodeling::SetY(const char *mString)
{
    y = mString;
    yStored = mString;

    if (modelMode == DemandModel::Exponential)
    {
        for (int i=0; i<y.length(); i++)
        {
            y[i] = log10(y[i]);
        }
    }
    else if (modelMode == DemandModel::Linear)
    {
        for (int i=0; i<y.length(); i++)
        {
            y[i] = log(y[i]);
        }
    }
}

void demandmodeling::SetStarts(const char *mString)
{
    c = mString;
}

void demandmodeling::SetScale(const char *mString)
{
    s = mString;
}

void demandmodeling::SetBounds(const char *mUpperString, const char *mLowerString)
{
    bndu = mUpperString;
    bndl = mLowerString;
}

void demandmodeling::SetScalingEnabled(bool value)
{
    scalingParameters = value;
}

real_1d_array demandmodeling::GetParams()
{
    return c;
}

lsfitstate demandmodeling::GetState()
{
    return state;
}

ae_int_t demandmodeling::GetInfo()
{
    return info;
}

lsfitreport demandmodeling::GetReport()
{
    return rep;
}

double demandmodeling::getMinimumConsumption()
{
    double minNonZeroConsumption = maxrealnumber;

    for (int i = 0; i < yStored.length(); i++)
    {
        if (yStored[i] > 0 && yStored[i] < minNonZeroConsumption)
        {
            minNonZeroConsumption = yStored[i];
        }
    }

    return minNonZeroConsumption;
}

double demandmodeling::getMaximumConsumption()
{
    double maxNonZeroConsumption = minrealnumber;

    for (int i = 0; i < yStored.length(); i++)
    {
        if (yStored[i] > 0 && yStored[i] > maxNonZeroConsumption)
        {
            maxNonZeroConsumption = yStored[i];
        }
    }

    return maxNonZeroConsumption;
}

void exponential_demand(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    func = log10(c[0]) + k * (exp(-c[1] * c[0] * x[0]) - 1);
}

void exponential_demand_grad(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);

    func = log10(c[0]) + k * (exp(-c[1] * c[0] * x[0]) - 1);

    grad[0] = 1/(c[0] * log(10)) - k * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0]));
    grad[1] = -(k * (exp(-c[1] * c[0] * x[0]) * (c[0] * x[0])));
}

void exponential_demand_with_k(const real_1d_array &c, const real_1d_array &x, double &func, void *)
{
    func = log10(c[0]) + c[2] * (exp(-c[1] * c[0] * x[0]) - 1);
}

void exponential_demand_with_k_grad(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, void *)
{
    func = log10(c[0]) + c[2] * (exp(-c[1] * c[0] * x[0]) - 1);

    grad[0] = 1/(c[0] * log(10)) - c[2] * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0]));
    grad[1] = -(c[2] * (exp(-c[1] * c[0] * x[0]) * (c[0] * x[0])));
    grad[2] = (exp(-c[1] * c[0] * x[0]) - 1);
}

void exponential_demand_with_k_shared(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    QList<real_1d_array> *params = (QList<real_1d_array> *) ptr;

    // This species parameters of interest (beginning at 0)
    int reference = params->at(0)[(int)x[0]];

    func =  log10(c[reference * 2]) + c[c.length() - 1] * (exp(-c[(reference * 2) + 1] * c[reference * 2] * params->at(params->length() - 1)[(int)x[0]]) - 1);
}

double demandmodeling::getExponentialSSR(double Q0, double alpha, double k)
{
    if (isnan(Q0) || isnan(alpha) || isnan(k))
    {
        return 999999999999;
    }

    double returner = 0.0;

    for (int i=0; i<x.rows(); i++)
    {
        returner += pow((y[i] - (log10(Q0) + k * (exp(-alpha * Q0 * x[i][0]) - 1))), 2);
    }

    return returner;
}

void exponentiated_demand(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    func = c[0] * pow(10, (k * (exp(-c[1] * c[0] * x[0]) - 1)));
}

void exponentiated_demand_grad(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);

    func = c[0] * pow(10, (k * (exp(-c[1] * c[0] * x[0]) - 1)));

    grad[0] = pow(10,(k * (exp(-c[1] * c[0] * x[0]) - 1))) -
            c[0] * (pow(10,(k * (exp(-c[1] * c[0] * x[0]) - 1))) *
                  (log(10) * (k * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0])))));

    grad[1] = -(c[0] * (pow(10,(k * (exp(-c[1] * c[0] * x[0]) - 1))) * (log(10) * (k * (exp(-c[1] * c[0] * x[0]) * (c[0] * x[0]))))));

}

void exponentiated_demand_with_k(const real_1d_array &c, const real_1d_array &x, double &func, void *)
{
    func = c[0] * pow(10, (c[2] * (exp(-c[1] * c[0] * x[0]) - 1)));
}

void exponentiated_demand_with_k_grad(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, void *)
{
    func = c[0] * pow(10, (c[2] * (exp(-c[1] * c[0] * x[0]) - 1)));

    grad[0] = pow(10,(c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) -
            c[0] * (pow(10,(c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
                  (log(10) * (c[2] * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0])))));

    grad[1] = -(c[0] * (pow(10,(c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) * (log(10) * (c[2] * (exp(-c[1] * c[0] * x[0]) * (c[0] * x[0]))))));

    grad[2] = c[0] * (pow(10,(c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) * (log(10) * (exp(-c[1] * c[0] * x[0]) - 1)));

}

void exponentiated_demand_with_k_shared(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    QList<real_1d_array> *params = (QList<real_1d_array> *) ptr;

    // This species parameters of interest (beginning at 0)
    int reference = params->at(0)[(int)x[0]];

    func =  c[reference * 2] * pow(10, (c[c.length() - 1] * (exp(-c[(reference * 2) + 1] * c[reference * 2] * params->at(params->count() - 1)[(int)x[0]]) - 1)));
}

double demandmodeling::getExponentiatedSSR(double Q0, double alpha, double k)
{
    if (isnan(Q0) || isnan(alpha) || isnan(k))
    {
        return 999999999999;
    }

    double returner = 0.0;

    for (int i=0; i<x.rows(); i++)
    {
        returner += pow((y[i] - (Q0 * pow(10, (k * (exp(-alpha * Q0 * x[i][0]) - 1))))), 2);
    }

    return returner;
}

void linear_demand(const real_1d_array &c, const real_1d_array &x, double &func, void *)
{
    func = log(c[2]) + (c[1] * log(x[0])) - c[0] * x[0];
}

void demandmodeling::FitLinear(const char *mStarts)
{
    SetStarts(mStarts);

    lsfitcreatef(x, y, c, diffstep, state);
    lsfitsetbc(state, bndl, bndu);
    lsfitsetcond(state, epsx, maxits);

    alglib::lsfitfit(state, linear_demand);

    lsfitresults(state, info, c, rep);
}

void demandmodeling::FitExponential(const char *mStarts, QList<double> mParams)
{
    SetStarts(mStarts);

    lsfitcreatefg(x,
                  y,
                  c,
                  true,
                  state);

    lsfitsetcond(state,
                 epsx,
                 maxits);

    lsfitsetbc(state,
               bndl,
               bndu);

    if (scalingParameters)
    {
        lsfitsetscale(state, s);
    }

    alglib::lsfitfit(state,
                     exponential_demand,
                     exponential_demand_grad,
                     NULL,
                     &mParams);

    lsfitresults(state, info, c, rep);
}

void demandmodeling::FitExponentialWithK(const char *mStarts)
{
    SetStarts(mStarts);

    lsfitcreatefg(x,
                  y,
                  c,
                  true,
                  state);

    lsfitsetcond(state,
                 epsx,
                 maxits);

    lsfitsetbc(state,
               bndl,
               bndu);

    if (scalingParameters)
    {
        lsfitsetscale(state, s);
    }

    alglib::lsfitfit(state, exponential_demand_with_k, exponential_demand_with_k_grad);

    lsfitresults(state, info, c, rep);
}

void demandmodeling::FitSharedExponentialK(const char *mStarts,
                                           QList<real_1d_array> *arrayHolder,
                                           void (*caller)(const real_1d_array &c, double func, void *ptr),
                                           void (*reset)(int amt, int max))
{
    SetStarts(mStarts);

    // Note: 0.001 as hard constant for just shared methods
    lsfitcreatef(x,
                 y,
                 c,
                 0.001,
                 state);

    // Note: 0.001 as hard constant for just shared methods
    lsfitsetcond(state,
                 0.001,
                 sharedIterationMax - 2);

    lsfitsetbc(state,
               bndl,
               bndu);

    if (scalingParameters)
    {
        lsfitsetscale(state, s);
    }

    reset(0, sharedIterationMax);

    lsfitsetxrep(state, true);

    alglib::lsfitfit(state,
                     exponential_demand_with_k_shared,
                     caller,
                     arrayHolder);

    lsfitresults(state, info, c, rep);
}

void demandmodeling::FitExponentiated(const char *mStarts, QList<double> mParams)
{
    SetStarts(mStarts);

    lsfitcreatefg(x,
                  y,
                  c,
                  true,
                  state);

    lsfitsetcond(state,
                 epsx,
                 maxits);

    lsfitsetbc(state,
               bndl,
               bndu);

    if (scalingParameters)
    {
        lsfitsetscale(state, s);
    }

    alglib::lsfitfit(state,
                     exponentiated_demand,
                     exponentiated_demand_grad,
                     NULL,
                     &mParams);

    lsfitresults(state, info, c, rep);
}

void demandmodeling::FitExponentiatedWithK(const char *mStarts)
{
    SetStarts(mStarts);

    lsfitcreatefg(x,
                  y,
                  c,
                  true,
                  state);

    lsfitsetcond(state,
                 epsx,
                 maxits);

    lsfitsetbc(state,
               bndl,
               bndu);

    if (scalingParameters)
    {
        lsfitsetscale(state, s);
    }

    alglib::lsfitfit(state, exponentiated_demand_with_k, exponentiated_demand_with_k_grad);

    lsfitresults(state, info, c, rep);
}

void demandmodeling::FitSharedExponentiatedK(const char *mStarts,
                                             QList<real_1d_array> *arrayHolder,
                                             void (*caller)(const real_1d_array &c, double func, void *ptr),
                                             void (*reset)(int amt, int max))
{
    SetStarts(mStarts);

    // Note: 0.001 as hard constant for just shared methods
    lsfitcreatef(x,
                 y,
                 c,
                 0.001,
                 state);

    // Note: 0.001 as hard constant for just shared methods
    lsfitsetcond(state,
                 (0.001 * arrayHolder->at(0)[arrayHolder->at(0).length() - 0]),
                 sharedIterationMax - 2);

    lsfitsetbc(state,
               bndl,
               bndu);

    if (scalingParameters)
    {
        lsfitsetscale(state, s);
    }

    reset(0, sharedIterationMax);

    lsfitsetxrep(state, true);

    alglib::lsfitfit(state,
                     exponentiated_demand_with_k_shared,
                     caller,
                     arrayHolder);

    lsfitresults(state, info, c, rep);
}

// Deprecated test method
void demandmodeling::TestMixedModel()
{

}

struct QPairFirstComparer
{
    template<typename T1, typename T2>
    bool operator()(const QPair<T1,T2> &one, const QPair<T1,T2> &two) const
    {
        return one.first < two.first;
    }
};

QStringList demandmodeling::GetSteinTest(QStringList &x, QStringList &y)
{
    containsZeroes = false;

    mModPoints.clear();

    numPosValues = 0;

    for (int i = 0; i < x.length() && i < x.length(); i++)
    {
        tempX = x[i].toDouble(&checkX);
        tempY = y[i].toDouble(&checkY);

        if (checkX && checkY)
        {
            if (tempY > 0)
            {
                numPosValues++;
            }
            else if (tempY == 0)
            {
                containsZeroes = true;
            }

            mModPoints.append(QPair<double, double>(tempX, tempY));
        }
    }

    // Sort into increasing prices
    //TODO: update to std::sort
    std::sort(mModPoints.begin(), mModPoints.end(), QPairFirstComparer());

    // Calculate DeltaQ
    deltaQ = (log10(mModPoints.first().second + 0.01) - log10(mModPoints.last().second + 0.01)) /
            (log10(mModPoints.last().first + 0.01) - log10(mModPoints.first().first + 0.01));

    // Calculate bounce
    bounceThreshold = mModPoints.first().second * 0.25;
    bounceCount = 0;

    for (int i = 1; i < mModPoints.count(); i++)
    {
        if ((mModPoints[i].second - mModPoints[i - 1].second) > bounceThreshold)
        {
            bounceCount++;
        }
    }

    bounceScore = (double) bounceCount / ((double) (mModPoints.count() - 1));

    // Calculate reversals
    reversalCount = 0;

    if (containsZeroes)
    {
        for (int i = 0; i < mModPoints.count() - ncons0; i++)
        {
            if (mModPoints[i].second == 0 && mModPoints[i + 1].second == 0 && mModPoints[i + 2].second > 0)
            {
                reversalCount++;
            }
        }
    }

    deltaQPass = (deltaQ >= deltaq) ? "Pass" : "Fail";
    bouncePass = (bounceScore <= bounce) ? "Pass" : "Fail";
    reversalpass = (reversalCount <= reversals) ? "Pass" : "Fail";

    passingMeasures = 0;

    if (deltaQPass.contains("Pass", Qt::CaseInsensitive))
    {
        passingMeasures++;
    }

    if (bouncePass.contains("Pass", Qt::CaseInsensitive))
    {
        passingMeasures++;
    }

    if (reversalpass.contains("Pass", Qt::CaseInsensitive))
    {
        passingMeasures++;
    }

    raisedFlag = (passingMeasures < 3) ? true : false;

    mSteinReturn.clear();
    mSteinReturn << "" // Participant id, filled in later
                 << QString::number(passingMeasures)
                 << QString::number(deltaQ)
                 << deltaQPass
                 << QString::number(bounceScore)
                 << bouncePass
                 << QString::number(reversalCount)
                 << reversalpass
                 << QString::number(numPosValues);

    return mSteinReturn;
}

demandmodeling::demandmodeling()
{

}
