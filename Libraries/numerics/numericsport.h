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

#ifndef NUMERICSPORT_H
#define NUMERICSPORT_H

#include <math.h>

class NumericsPort
{
public:
    NumericsPort();

    double NormInv(double probability, double mean, double standardDev);

    double InvCDF(double mean, double stddev, double p);

    double ErfcInv(double z);

    double ErfInvImpl(double p, double q, double s);

    double Polynomial(double z, double *array, int count);

private:
    const double Sqrt2 = 1.4142135623730950488016887242096980785696718753769;

    /// <summary>
    /// **************************************
    /// COEFFICIENTS FOR METHOD ErfInvImp    *
    /// **************************************
    /// </summary>
    /// <summary> Polynomial coefficients for a numerator of ErfInvImp
    /// calculation for Erf^-1(z) in the interval [0, 0.5].
    /// </summary>
    double ErvInvImpAn[8] = { -0.000508781949658280665617, -0.00836874819741736770379, 0.0334806625409744615033, -0.0126926147662974029034, -0.0365637971411762664006, 0.0219878681111168899165, 0.00822687874676915743155, -0.00538772965071242932965 };

    /// <summary> Polynomial coefficients for a denominator of ErfInvImp
    /// calculation for Erf^-1(z) in the interval [0, 0.5].
    /// </summary>
    double ErvInvImpAd[10] = { 1, -0.970005043303290640362, -1.56574558234175846809, 1.56221558398423026363, 0.662328840472002992063, -0.71228902341542847553, -0.0527396382340099713954, 0.0795283687341571680018, -0.00233393759374190016776, 0.000886216390456424707504 };

    /// <summary> Polynomial coefficients for a numerator of ErfInvImp
    /// calculation for Erf^-1(z) in the interval [0.5, 0.75].
    /// </summary>
    double ErvInvImpBn[9] = { -0.202433508355938759655, 0.105264680699391713268, 8.37050328343119927838, 17.6447298408374015486, -18.8510648058714251895, -44.6382324441786960818, 17.445385985570866523, 21.1294655448340526258, -3.67192254707729348546 };

    /// <summary> Polynomial coefficients for a denominator of ErfInvImp
    /// calculation for Erf^-1(z) in the interval [0.5, 0.75].
    /// </summary>
    double ErvInvImpBd[9] = { 1, 6.24264124854247537712, 3.9713437953343869095, -28.6608180499800029974, -20.1432634680485188801, 48.5609213108739935468, 10.8268667355460159008, -22.6436933413139721736, 1.72114765761200282724 };

    /// <summary> Polynomial coefficients for a numerator of ErfInvImp
    /// calculation for Erf^-1(z) in the interval [0.75, 1] with x less than 3.
    /// </summary>
    double ErvInvImpCn[11] = { -0.131102781679951906451, -0.163794047193317060787, 0.117030156341995252019, 0.387079738972604337464, 0.337785538912035898924, 0.142869534408157156766, 0.0290157910005329060432, 0.00214558995388805277169, -0.679465575181126350155e-6, 0.285225331782217055858e-7, -0.681149956853776992068e-9 };

    /// <summary> Polynomial coefficients for a denominator of ErfInvImp
    /// calculation for Erf^-1(z) in the interval [0.75, 1] with x less than 3.
    /// </summary>
    double ErvInvImpCd[8] = { 1, 3.46625407242567245975, 5.38168345707006855425, 4.77846592945843778382, 2.59301921623620271374, 0.848854343457902036425, 0.152264338295331783612, 0.01105924229346489121 };

    /// <summary> Polynomial coefficients for a numerator of ErfInvImp
    /// calculation for Erf^-1(z) in the interval [0.75, 1] with x between 3 and 6.
    /// </summary>
    double ErvInvImpDn[9] = { -0.0350353787183177984712, -0.00222426529213447927281, 0.0185573306514231072324, 0.00950804701325919603619, 0.00187123492819559223345, 0.000157544617424960554631, 0.460469890584317994083e-5, -0.230404776911882601748e-9, 0.266339227425782031962e-11 };

    /// <summary> Polynomial coefficients for a denominator of ErfInvImp
    /// calculation for Erf^-1(z) in the interval [0.75, 1] with x between 3 and 6.
    /// </summary>
    double ErvInvImpDd[7] = { 1, 1.3653349817554063097, 0.762059164553623404043, 0.220091105764131249824, 0.0341589143670947727934, 0.00263861676657015992959, 0.764675292302794483503e-4 };

    /// <summary> Polynomial coefficients for a numerator of ErfInvImp
    /// calculation for Erf^-1(z) in the interval [0.75, 1] with x between 6 and 18.
    /// </summary>
    double ErvInvImpEn[9] = { -0.0167431005076633737133, -0.00112951438745580278863, 0.00105628862152492910091, 0.000209386317487588078668, 0.149624783758342370182e-4, 0.449696789927706453732e-6, 0.462596163522878599135e-8, -0.281128735628831791805e-13, 0.99055709973310326855e-16 };

    /// <summary> Polynomial coefficients for a denominator of ErfInvImp
    /// calculation for Erf^-1(z) in the interval [0.75, 1] with x between 6 and 18.
    /// </summary>
    double ErvInvImpEd[7] = { 1, 0.591429344886417493481, 0.138151865749083321638, 0.0160746087093676504695, 0.000964011807005165528527, 0.275335474764726041141e-4, 0.282243172016108031869e-6 };

    /// <summary> Polynomial coefficients for a numerator of ErfInvImp
    /// calculation for Erf^-1(z) in the interval [0.75, 1] with x between 18 and 44.
    /// </summary>
    double ErvInvImpFn[8] = { -0.0024978212791898131227, -0.779190719229053954292e-5, 0.254723037413027451751e-4, 0.162397777342510920873e-5, 0.396341011304801168516e-7, 0.411632831190944208473e-9, 0.145596286718675035587e-11, -0.116765012397184275695e-17 };

    /// <summary> Polynomial coefficients for a denominator of ErfInvImp
    /// calculation for Erf^-1(z) in the interval [0.75, 1] with x between 18 and 44.
    /// </summary>
    double ErvInvImpFd[7] = { 1, 0.207123112214422517181, 0.0169410838120975906478, 0.000690538265622684595676, 0.145007359818232637924e-4, 0.144437756628144157666e-6, 0.509761276599778486139e-9 };

    /// <summary> Polynomial coefficients for a numerator of ErfInvImp
    /// calculation for Erf^-1(z) in the interval [0.75, 1] with x greater than 44.
    /// </summary>
    double ErvInvImpGn[8] = { -0.000539042911019078575891, -0.28398759004727721098e-6, 0.899465114892291446442e-6, 0.229345859265920864296e-7, 0.225561444863500149219e-9, 0.947846627503022684216e-12, 0.135880130108924861008e-14, -0.348890393399948882918e-21 };

    /// <summary> Polynomial coefficients for a denominator of ErfInvImp
    /// calculation for Erf^-1(z) in the interval [0.75, 1] with x greater than 44.
    /// </summary>
    double ErvInvImpGd[7] = { 1, 0.0845746234001899436914, 0.00282092984726264681981, 0.468292921940894236786e-4, 0.399968812193862100054e-6, 0.161809290887904476097e-8, 0.231558608310259605225e-11 };
};

#endif // NUMERICSPORT_H
