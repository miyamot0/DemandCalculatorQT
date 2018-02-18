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

void demandmodeling::SetW(const char *mString)
{
    w = mString;
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

void demandmodeling::SetAlternativePmaxEnabled(bool value)
{
    alternativePmax = value;
}

void demandmodeling::SetFittingAlgorithm(FittingAlgorithm value)
{
    fittingAlgorithm = value;
}

void demandmodeling::SetWeightingModel(WeightingMode value)
{
    weightingMode = value;
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

void exponential_demand_hessian(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, real_2d_array &hess, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);

    func = log10(c[0]) + k * (exp(-c[1] * c[0] * x[0]) - 1);

    grad[0] = 1/(c[0] * log(10)) - k * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0]));
    grad[1] = -(k * (exp(-c[1] * c[0] * x[0]) * (c[0] * x[0])));

    hess[0][0] = -(1/pow(c[0],(2/log(10))) - k * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0]) * (c[1] * x[0])));
    hess[0][1] = -(k * (exp(-c[1] * c[0] * x[0]) * x[0] - exp(-c[1] * c[0] * x[0]) * (c[0] * x[0]) * (c[1] * x[0])));

    hess[1][0] = -(k * (exp(-c[1] * c[0] * x[0]) * x[0] - exp(-c[1] * c[0] * x[0]) * (c[1] * x[0]) * (c[0] * x[0])));
    hess[1][1] = k * (exp(-c[1] * c[0] * x[0]) * (c[0] * x[0]) * (c[0] * x[0]));
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

void exponential_demand_with_k_hessian(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, real_2d_array &hess, void *)
{
    func = log10(c[0]) + c[2] * (exp(-c[1] * c[0] * x[0]) - 1);

    grad[0] = 1/(c[0] * log(10)) - c[2] * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0]));
    grad[1] = -(c[2] * (exp(-c[1] * c[0] * x[0]) * (c[0] * x[0])));
    grad[2] = (exp(-c[1] * c[0] * x[0]) - 1);

    hess[0][0] = -(1/pow(c[0],(2/log(10))) - c[2] * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0]) * (c[1] * x[0])));
    hess[0][1] = -(c[2] * (exp(-c[1] * c[0] * x[0]) * x[0] - exp(-c[1] * c[0] * x[0]) * (c[0] * x[0]) * (c[1] * x[0])));
    hess[0][2] = -(exp(-c[1] * c[0] * x[0]) * (c[1] * x[0]));

    hess[1][0] = -(c[2] * (exp(-c[1] * c[0] * x[0]) * x[0] - exp(-c[1] * c[0] * x[0]) * (c[1] * x[0]) * (c[0] * x[0])));
    hess[1][1] = c[2] * (exp(-c[1] * c[0] * x[0]) * (c[0] * x[0]) * (c[0] * x[0]));
    hess[1][2] = -(exp(-c[1] * c[0] * x[0]) * (c[0] * x[0]));

    hess[2][0] = -(exp(-c[1] * c[0] * x[0]) * (c[1] * x[0]));
    hess[2][1] = -(exp(-c[1] * c[0] * x[0]) * (c[0] * x[0]));
    hess[2][2] = 0;
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

void exponentiated_demand_hessian(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, real_2d_array &hess, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);

    func = c[0] * pow(10, (k * (exp(-c[1] * c[0] * x[0]) - 1)));

    grad[0] = pow(10,(k * (exp(-c[1] * c[0] * x[0]) - 1))) -
            c[0] * (pow(10,(k * (exp(-c[1] * c[0] * x[0]) - 1))) *
                  (log(10) * (k * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0])))));

    grad[1] = -(c[0] * (pow(10,(k * (exp(-c[1] * c[0] * x[0]) - 1))) * (log(10) * (k * (exp(-c[1] * c[0] * x[0]) * (c[0] * x[0]))))));

    hess[0][0] = -(pow(10,(k * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (k * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0])))) +
            ((pow(10, (k * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (k * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0]))))) -
            c[0] * (pow(10,(k * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (k * (exp(-c[1] * c[0] * x[0]) *
            (c[1] * x[0]) * (c[1] * x[0])))) +
            pow(10, (k * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (k * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0])))) *
            (log(10) * (k * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0])))))));

    hess[0][1] = -(pow(10, (k * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (k * (exp(-c[1] * c[0] *x[0]) *
            (c[0] *x[0])))) + c[0] * (pow(10, (k * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (k * (exp(-c[1] * c[0] *x[0]) *x[0] - exp(-c[1] * c[0] * x[0]) *
            (c[0] *x[0]) * (c[1] *x[0])))) -
            pow(10, (k * (exp(-c[1] * c[0] *x[0]) - 1))) *
            (log(10) * (k * (exp(-c[1] * c[0] *x[0]) * (c[0] * x[0])))) *
            (log(10) * (k * (exp(-c[1] * c[0] *x[0]) * (c[1] * x[0]))))));

    hess[1][0] = -((pow(10, (k * (exp(-c[1] * c[0] *x[0]) - 1))) *
            (log(10) * (k * (exp(-c[1] * c[0] *x[0]) * (c[0] * x[0]))))) +
            c[0] * (pow(10, (k * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (k * (exp(-c[1] * c[0] *x[0]) *x[0] - exp(-c[1] * c[0] * x[0]) *
            (c[1] *x[0]) * (c[0] *x[0])))) -
            pow(10, (k * (exp(-c[1] * c[0] *x[0]) - 1))) *
            (log(10) * (k * (exp(-c[1] * c[0] *x[0]) * (c[1] * x[0])))) *
            (log(10) * (k * (exp(-c[1] * c[0] *x[0]) * (c[0] *x[0]))))));

    hess[1][1] = c[0] * (pow(10, (k * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (k * (exp(-c[1] * c[0] * x[0]) * (c[0] * x[0]) * (c[0] * x[0])))) +
            pow(10, (k * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (k * (exp(-c[1] * c[0] * x[0]) * (c[0] * x[0])))) *
            (log(10) * (k * (exp(-c[1] * c[0] * x[0]) * (c[0] * x[0])))));
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

void exponentiated_demand_with_k_hessian(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, real_2d_array &hess, void *)
{
    func = c[0] * pow(10, (c[2] * (exp(-c[1] * c[0] * x[0]) - 1)));

    grad[0] = pow(10,(c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) -
            c[0] * (pow(10,(c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
                  (log(10) * (c[2] * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0])))));

    grad[1] = -(c[0] * (pow(10,(c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) * (log(10) * (c[2] * (exp(-c[1] * c[0] * x[0]) * (c[0] * x[0]))))));

    grad[2] = c[0] * (pow(10,(c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) * (log(10) * (exp(-c[1] * c[0] * x[0]) - 1)));

    hess[0][0] = -(pow(10,(c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (c[2] * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0])))) +
            ((pow(10, (c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (c[2] * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0]))))) -
            c[0] * (pow(10,(c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (c[2] * (exp(-c[1] * c[0] * x[0]) *
            (c[1] * x[0]) * (c[1] * x[0])))) +
            pow(10, (c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (c[2] * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0])))) *
            (log(10) * (c[2] * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0])))))));

    hess[0][1] = -(pow(10, (c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (c[2] * (exp(-c[1] * c[0] *x[0]) *
            (c[0] *x[0])))) + c[0] * (pow(10, (c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (c[2] * (exp(-c[1] * c[0] *x[0]) *x[0] - exp(-c[1] * c[0] * x[0]) *
            (c[0] *x[0]) * (c[1] *x[0])))) -
            pow(10, (c[2] * (exp(-c[1] * c[0] *x[0]) - 1))) *
            (log(10) * (c[2] * (exp(-c[1] * c[0] *x[0]) * (c[0] * x[0])))) *
            (log(10) * (c[2] * (exp(-c[1] * c[0] *x[0]) * (c[1] * x[0]))))));

    hess[0][2] = pow(10, (c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (exp(-c[1] * c[0] * x[0]) - 1)) -
            c[0] * (pow(10, (c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (exp(-c[1] * c[0] * x[0]) - 1)) *
            (log(10) * (c[2] * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0])))) +
            pow(10, (c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0]))));

    hess[1][0] = -((pow(10, (c[2] * (exp(-c[1] * c[0] *x[0]) - 1))) *
            (log(10) * (c[2] * (exp(-c[1] * c[0] *x[0]) * (c[0] * x[0]))))) +
            c[0] * (pow(10, (c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (c[2] * (exp(-c[1] * c[0] *x[0]) *x[0] - exp(-c[1] * c[0] * x[0]) *
            (c[1] *x[0]) * (c[0] *x[0])))) -
            pow(10, (c[2] * (exp(-c[1] * c[0] *x[0]) - 1))) *
            (log(10) * (c[2] * (exp(-c[1] * c[0] *x[0]) * (c[1] * x[0])))) *
            (log(10) * (c[2] * (exp(-c[1] * c[0] *x[0]) * (c[0] *x[0]))))));

    hess[1][1] = c[0] * (pow(10, (c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (c[2] * (exp(-c[1] * c[0] * x[0]) * (c[0] * x[0]) * (c[0] * x[0])))) +
            pow(10, (c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (c[2] * (exp(-c[1] * c[0] * x[0]) * (c[0] * x[0])))) *
            (log(10) * (c[2] * (exp(-c[1] * c[0] * x[0]) * (c[0] * x[0])))));

    hess[1][2] = -(c[0] * (pow(10, (c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (exp(-c[1] * c[0] * x[0]) - 1)) *
            (log(10) * (c[2] * (exp(-c[1] * c[0] * x[0]) * (c[0] * x[0])))) +
            pow(10, (c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (exp(-c[1] * c[0] * x[0]) * (c[0] * x[0])))));

    hess[2][0] = (pow(10, (c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (exp(-c[1] * c[0] * x[0]) - 1))) -
            c[0] * (pow(10, (c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0]))) +
            pow(10, (c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (c[2] * (exp(-c[1] * c[0] * x[0]) * (c[1] * x[0])))) *
            (log(10) * (exp(-c[1] * c[0] * x[0]) - 1)));

    hess[2][1] = -(c[0] * (pow(10, (c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (exp(-c[1] * c[0] * x[0]) * (c[0] * x[0]))) +
            pow(10, (c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (c[2] * (exp(-c[1] * c[0] * x[0]) * (c[0] * x[0])))) *
            (log(10) * (exp(-c[1] * c[0] * x[0]) - 1))));

    hess[2][2] = c[0] * (pow(10, (c[2] * (exp(-c[1] * c[0] * x[0]) - 1))) *
            (log(10) * (exp(-c[1] * c[0] * x[0]) - 1)) *
            (log(10) * (exp(-c[1] * c[0] * x[0]) - 1)));
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

    if (fittingAlgorithm == FittingAlgorithm::Function)
    {
        lsfitcreatef(x,
                     y,
                     c,
                     diffstep,
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
                         NULL,
                         &mParams);

    }
    else if (fittingAlgorithm == FittingAlgorithm::FunctionGradient)
    {
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
    }
    else if (fittingAlgorithm == FittingAlgorithm::FunctionGradientHessian)
    {
        lsfitcreatefgh(x,
                       y,
                       c,
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
                         exponential_demand_hessian,
                         NULL,
                         &mParams);
    }

    lsfitresults(state, info, c, rep);

    if (alternativePmax)
    {
        exactSolutionPmax = ExactSolutionPmax(c[0], c[1], mParams[0]);
        //BootstrapPmaxExponential(c[0], c[1], mParams[0]);
    }
}

void demandmodeling::FitExponentialWithK(const char *mStarts)
{
    SetStarts(mStarts);

    if (fittingAlgorithm == FittingAlgorithm::Function)
    {
        lsfitcreatef(x,
                     y,
                     c,
                     diffstep,
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
                         exponential_demand_with_k);
    }
    else if (fittingAlgorithm == FittingAlgorithm::FunctionGradient)
    {
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
                         exponential_demand_with_k,
                         exponential_demand_with_k_grad);
    }
    else if (fittingAlgorithm == FittingAlgorithm::FunctionGradientHessian)
    {
        lsfitcreatefgh(x,
                       y,
                       c,
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
                         exponential_demand_with_k,
                         exponential_demand_with_k_grad,
                         exponential_demand_with_k_hessian);
    }

    lsfitresults(state, info, c, rep);

    if (alternativePmax)
    {
        exactSolutionPmax = ExactSolutionPmax(c[0], c[1], c[2]);
        //BootstrapPmaxExponential(c[0], c[1], c[2]);
    }
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

    if (fittingAlgorithm == FittingAlgorithm::Function)
    {
        lsfitcreatef(x,
                     y,
                     c,
                     diffstep,
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
                         NULL,
                         &mParams);
    }
    else if (fittingAlgorithm == FittingAlgorithm::FunctionGradient)
    {
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
    }
    else if (fittingAlgorithm == FittingAlgorithm::FunctionGradientHessian)
    {
        lsfitcreatefgh(x,
                       y,
                       c,
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
                         exponentiated_demand_hessian,
                         NULL,
                         &mParams);
    }

    lsfitresults(state, info, c, rep);

    if (alternativePmax)
    {
        exactSolutionPmax = ExactSolutionPmax(c[0], c[1], mParams[0]);
        //BootstrapPmaxExponentiated(c[0], c[1], mParams[0]);
    }
}

void demandmodeling::FitExponentiatedWithK(const char *mStarts)
{
    SetStarts(mStarts);

    if (fittingAlgorithm == FittingAlgorithm::Function)
    {
        lsfitcreatef(x,
                     y,
                     c,
                     diffstep,
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
                         exponentiated_demand_with_k);
    }
    else if (fittingAlgorithm == FittingAlgorithm::FunctionGradient)
    {
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
                         exponentiated_demand_with_k,
                         exponentiated_demand_with_k_grad);
    }
    else if (fittingAlgorithm == FittingAlgorithm::FunctionGradientHessian)
    {
        lsfitcreatefgh(x,
                       y,
                       c,
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
                         exponentiated_demand_with_k,
                         exponentiated_demand_with_k_grad,
                         exponentiated_demand_with_k_hessian);
    }

    lsfitresults(state, info, c, rep);

    if (alternativePmax)
    {
        exactSolutionPmax = ExactSolutionPmax(c[0], c[1], c[2]);
        //BootstrapPmaxExponentiated(c[0], c[1], c[2]);
    }
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

    try
    {
        alglib::lsfitfit(state,
                         exponentiated_demand_with_k_shared,
                         caller,
                         arrayHolder);

        lsfitresults(state, info, c, rep);
    }
    catch (alglib::ap_error err)
    {
        info = -8;

        throw alglib::ap_error("Error in getting shared k");
    }
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

void exponential_work_curve(const real_1d_array &x, double &func, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double Q = param->at(0);
    double A = param->at(1);
    double K = param->at(2);

    func = -(pow(10, ((log(Q)/log(10)) + K * (exp(-A * Q * x[0]) - 1))) * x[0]);
}

double demandmodeling::ExactSolutionPmax(double Q0, double A, double K)
{
    LambertResult result = gsl_sf_lambert_W0(-1/log(pow(10,K)));

    if (!result.Success)
    {
        return NULL;
    }
    else
    {
        return -result.Val/(A*Q0);
    }
}

/*

void demandmodeling::BootstrapPmaxExponential(double Q0, double A, double K)
{
    QString mString = QString::number((1/(Q0 * A * pow(K, 1.5)) * (0.083 * K + 0.65)));
    mString.prepend('[');
    mString.append(']');

    workOutputX = mString.toUtf8().constData();

    double epsg = 0.0000000001;
    double epsf = 0;
    double epsx = 0;
    double diffstep = 1.0e-6;

    QList<double> mParams;
    mParams << Q0;
    mParams << A;
    mParams << K;

    minlbfgscreatef(1, workOutputX, diffstep, workOutputState);
    minlbfgssetcond(workOutputState, epsg, epsf, epsx, 50);
    alglib::minlbfgsoptimize(workOutputState, exponential_work_curve, NULL, &mParams);
    minlbfgsresults(workOutputState, workOutputX, workOutputRep);
}

void exponentiated_work_curve(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double Q = param->at(0);
    double A = param->at(1);
    double K = param->at(2);

    func = -((Q * pow(10.0,(K * (exp(-A * Q * x[0]) - 1)))) * x[0]);

    grad[0] = -(Q * (pow(10,(K * (exp(-A * Q * x[0]) - 1))) * (log(10) * (K * (exp(-A * Q * x[0]) * (-A * Q))))) *
                x[0] + (Q * pow(10, (K * (exp(-A * Q * x[0]) - 1)))));
}

void demandmodeling::BootstrapPmaxExponentiated(double Q0, double A, double K)
{
    QString mString = QString::number((1/(Q0 * A * pow(K, 1.5)) * (0.083 * K + 0.65)));
    mString.prepend('[');
    mString.append(']');

    workOutputX = mString.toUtf8().constData();

    double epsg = 0.0000000001;
    double epsf = 0;
    double epsx = 0;
    //double diffstep = 1.0e-6;
    //ae_int_t maxits = 0;

    QList<double> mParams;
    mParams << Q0;
    mParams << A;
    mParams << K;

    //minlbfgscreatef(1, workOutputX, diffstep, workOutputState);
    minlbfgscreate(1, workOutputX, workOutputState);

    minlbfgssetcond(workOutputState, epsg, epsf, epsx, 50);
    alglib::minlbfgsoptimize(workOutputState, exponentiated_work_curve, NULL, &mParams);
    minlbfgsresults(workOutputState, workOutputX, workOutputRep);
}

*/

double demandmodeling::GetBootStrapPmax()
{
    return exactSolutionPmax;
}

double demandmodeling::GetBootStrapPmaxExponentialSlope(double Q, double A, double K, double pMax)
{
    double P1 = pMax - 0.05;
    double P2 = pMax + 0.05;

    double Q1 = (log(Q)/log(10)) + K * (exp(-A * Q * P1) - 1);
    double Q2 = (log(Q)/log(10)) + K * (exp(-A * Q * P2) - 1);

    Q1 = pow(10, Q1);
    Q2 = pow(10, Q2);

    double QD = ((Q2-Q1)/((Q2+Q1)/2.0));
    double PD = ((P2-P1)/((P2+P1)/2.0));

    return (QD / PD);
}

double demandmodeling::GetBootStrapPmaxExponentiatedSlope(double Q, double A, double K, double pMax)
{
    double P1 = pMax - 0.05;
    double P2 = pMax + 0.05;

    double Q1 = Q * pow(10,(K * (exp(-A * Q * P1) - 1)));
    double Q2 = Q * pow(10,(K * (exp(-A * Q * P2) - 1)));

    double QD = ((Q2-Q1)/((Q2+Q1)/2.0));
    double PD = ((P2-P1)/((P2+P1)/2.0));

    return (QD / PD);
}

/**
 * @brief demandmodeling::series_eval
 * @param r
 * @return
 */
double demandmodeling::series_eval(double r) {
  double c[12]
  {
    -1.0,
    2.331643981597124203363536062168,
    -1.812187885639363490240191647568,
    1.936631114492359755363277457668,
    -2.353551201881614516821543561516,
    3.066858901050631912893148922704,
    -4.175335600258177138854984177460,
    5.858023729874774148815053846119,
    -8.401032217523977370984161688514,
    12.250753501314460424,
    -18.100697012472442755,
    27.029044799010561650
  };

  double t_8 = c[8] + r * (c[9] + r * (c[10] + r * c[11]));
  double t_5 = c[5] + r * (c[6] + r * (c[7] + r * t_8));
  double t_1 = c[1] + r * (c[2] + r * (c[3] + r * (c[4] + r * t_5)));

  return c[0] + r * t_1;
}

LambertResult demandmodeling::halley_iteration(double x, double w_initial, int max_iters) {
  double w = w_initial;
  int i;

  LambertResult result;

  for (i = 0; i < max_iters; i++) {
    double tol;
    double e = exp(w);
    double p = w + 1.0;
    double t = w * e - x;

    if (w > 0) {
      t = (t / p) / e;
      /* Newton iteration */
    } else {
      t /= e * p - 0.5 * (p + 1.0) * t / p;
      /* Halley iteration */
    }

    w -= t;

    double optTolOne = abs(w);
    double optTolTwo = 1.0 / (abs(p) * e);

    if (optTolOne > optTolTwo)
    {
        tol = GSL_DBL_EPSILON * optTolOne;
    }
    else
    {
        tol = GSL_DBL_EPSILON * optTolTwo;
    }

    result.Val = w;
    result.Err = 2 * tol;
    result.Iterations = i;
    result.Success = true;

    if (abs(t) < tol) {
      return result;
    }
  }
  /* should never get here */

  result.Val = w;
  result.Err = abs(w);
  result.Iterations = i;
  result.Success = false;

  return result;

}

/**
 * @brief demandmodeling::gsl_sf_lambert_W0 Based on GNU Scientific Library
 * @param x
 * @return
 */
LambertResult demandmodeling::gsl_sf_lambert_W0(double x)
{
    const double one_over_E = 1.0 / M_E;
    const double q = x + one_over_E;

    LambertResult result;

    if (x == 0.0) {
      result.Val = 0.0;
      result.Err = 0.0;
      result.Success = true;
      return result;
    }
    else if (q < 0.0) {
      /* Strictly speaking this is an error. But because of the
       * arithmetic operation connecting x and q, I am a little
       * lenient in case of some epsilon overshoot. The following
       * answer is quite accurate in that case. Anyway, we have
       * to return GSL_EDOM.
       */
      result.Val = -1.0;
      result.Err = sqrt(-q);
      result.Success = false; // GSL_EDOM
      return result;
    }
    else if (q == 0.0) {
      result.Val = -1.0;
      result.Err = GSL_DBL_EPSILON;
      /* cannot error is zero, maybe q == 0 by "accident" */
      result.Success = true;
      return result;
    }
    else if (q < 1.0e-03) {
      /* series near -1/E in sqrt(q) */
      const double r = sqrt(q);
      result.Val = series_eval(r);
      result.Err = 2.0 * GSL_DBL_EPSILON * abs(result.Val);
      result.Success = true;
      return result;
    }
    else {
      int MAX_ITERS = 100;
      double w;

      if (x < 1.0) {
        /* obtain initial approximation from series near x=0;
         * no need for extra care, since the Halley iteration
         * converges nicely on this branch
         */
        double p = sqrt(2.0 * M_E * q);
        w = -1.0 + p * (1.0 + p * (-1.0 / 3.0 + p * 11.0 / 72.0));
      }
      else {
        /* obtain initial approximation from rough asymptotic */
        w = log(x);
        if (x > 3.0) w -= log(w);
      }

      return halley_iteration(x, w, MAX_ITERS);
    }
}

demandmodeling::demandmodeling()
{

}
