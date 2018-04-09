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

   ================================================================================

   Math.NET Numerics License (MIT/X11)
   Copyright (c) 2002-2018 Math.NET

   Permission is hereby granted, free of charge, to any person obtaining a copy of
   this software and associated documentation files (the "Software"), to deal in the
   Software without restriction, including without limitation the rights to use, copy,
   modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
   and to permit persons to whom the Software is furnished to do so, subject to the
   following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
   INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
   PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
   OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

  */

#include "Libraries/numerics/numericsport.h"

NumericsPort::NumericsPort()
{

}

double NumericsPort::NormInv(double probability, double mean, double standardDev)
{
    return InvCDF(mean, standardDev, probability);
}

double NumericsPort::InvCDF(double mean, double stddev, double p)
{
    return mean - (stddev*Sqrt2*ErfcInv(2.0*p));
}

double NumericsPort::ErfcInv(double z)
{
    if (z <= 0.0)
    {
        //return double.PositiveInfinity;
    }

    if (z >= 2.0)
    {
        //return double.NegativeInfinity;
    }

    double p, q, s;

    if (z > 1)
    {
        q = 2 - z;
        p = 1 - q;
        s = -1;
    }
    else
    {
        p = 1 - z;
        q = z;
        s = 1;
    }

    return ErfInvImpl(p, q, s);
}

double NumericsPort::ErfInvImpl(double p, double q, double s)
{
    double result;

    if (p <= 0.5)
    {
        // Evaluate inverse erf using the rational approximation:
        //
        // x = p(p+10)(Y+R(p))
        //
        // Where Y is a constant, and R(p) is optimized for a low
        // absolute error compared to |Y|.
        //
        // double: Max error found: 2.001849e-18
        // long double: Max error found: 1.017064e-20
        // Maximum Deviation Found (actual error term at infinite precision) 8.030e-21
        const float y = 0.0891314744949340820313f;
        double g = p * (p + 10);
        double r = Polynomial(p, ErvInvImpAn, 8) / Polynomial(p, ErvInvImpAd, 10);
        result = (g*y) + (g*r);
    }
    else if (q >= 0.25)
    {
        // Rational approximation for 0.5 > q >= 0.25
        //
        // x = sqrt(-2*log(q)) / (Y + R(q))
        //
        // Where Y is a constant, and R(q) is optimized for a low
        // absolute error compared to Y.
        //
        // double : Max error found: 7.403372e-17
        // long double : Max error found: 6.084616e-20
        // Maximum Deviation Found (error term) 4.811e-20
        const float y = 2.249481201171875f;
        double g = sqrt(-2 * log(q));
        double xs = q - 0.25;
        double r = Polynomial(xs, ErvInvImpBn, 9) / Polynomial(xs, ErvInvImpBd, 9);
        result = g / (y + r);
    }
    else
    {
        // For q < 0.25 we have a series of rational approximations all
        // of the general form:
        //
        // let: x = sqrt(-log(q))
        //
        // Then the result is given by:
        //
        // x(Y+R(x-B))
        //
        // where Y is a constant, B is the lowest value of x for which
        // the approximation is valid, and R(x-B) is optimized for a low
        // absolute error compared to Y.
        //
        // Note that almost all code will really go through the first
        // or maybe second approximation.  After than we're dealing with very
        // small input values indeed: 80 and 128 bit long double's go all the
        // way down to ~ 1e-5000 so the "tail" is rather long...
        double x = sqrt(-log(q));
        if (x < 3)
        {
            // Max error found: 1.089051e-20
            const float y = 0.807220458984375f;
            double xs = x - 1.125;
            double r = Polynomial(xs, ErvInvImpCn, 11) / Polynomial(xs, ErvInvImpCd, 8);
            result = (y*x) + (r*x);
        }
        else if (x < 6)
        {
            // Max error found: 8.389174e-21
            const float y = 0.93995571136474609375f;
            double xs = x - 3;
            double r = Polynomial(xs, ErvInvImpDn, 9) / Polynomial(xs, ErvInvImpDd, 7);
            result = (y*x) + (r*x);
        }
        else if (x < 18)
        {
            // Max error found: 1.481312e-19
            const float y = 0.98362827301025390625f;
            double xs = x - 6;
            double r = Polynomial(xs, ErvInvImpEn, 9) / Polynomial(xs, ErvInvImpEd, 7);
            result = (y*x) + (r*x);
        }
        else if (x < 44)
        {
            // Max error found: 5.697761e-20
            const float y = 0.99714565277099609375f;
            double xs = x - 18;
            double r = Polynomial(xs, ErvInvImpFn, 8) / Polynomial(xs, ErvInvImpFd, 7);
            result = (y*x) + (r*x);
        }
        else
        {
            // Max error found: 1.279746e-20
            const float y = 0.99941349029541015625f;
            double xs = x - 44;
            double r = Polynomial(xs, ErvInvImpGn, 8) / Polynomial(xs, ErvInvImpGd, 7);
            result = (y*x) + (r*x);
        }
    }

    return s * result;
}

double NumericsPort::Polynomial(double z, double *array, int count)
{
    double sum = array[count - 1];

    for (int i = count - 2; i >= 0; --i)
    {
        sum *= z;
        sum += array[i];
    }

    return sum;
}
