#include "demandmodeling.h"
#include <QElapsedTimer>

void demandmodeling::SetX(const char *mString)
{
    x = mString;
}

void demandmodeling::SetY(const char *mString)
{
    y = mString;
}

void demandmodeling::SetStarts(const char *mString)
{
    c = mString;
}

void demandmodeling::SetBounds(const char *mUpperString, const char *mLowerString)
{
    bndu = mUpperString;
    bndl = mLowerString;
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

void exponential_demand(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    func = log10(c[0]) + k * (exp(-c[1] * c[0] * x[0]) - 1);
}

void exponential_demand_with_k(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    func = log10(c[1]) + c[0] * (exp(-c[2] * c[1] * x[0]) - 1);
}

void exponentiated_demand(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    func = c[0] * pow(10, (k * (exp(-c[1] * c[0] * x[0]) - 1)));
}

void exponentiated_demand_with_k(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    func = c[1] * pow(10, (c[0] * (exp(-c[2] * c[1] * x[0]) - 1)));
}

void linear_demand(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    func = log(1) + (c[1] * log(x[0])) - c[0] * x[0];
}

void demandmodeling::FitLinear(const char *mStarts)
{
    QElapsedTimer timer;
    timer.start();
    SetStarts(mStarts);

    lsfitcreatef(x, y, c, diffstep, state);
    lsfitsetbc(state, bndl, bndu);
    lsfitsetcond(state, epsx, maxits);

    alglib::lsfitfit(state, linear_demand);

    lsfitresults(state, info, c, rep);

    qDebug() << "a: " << c[0];
    qDebug() << "b: " << c[1];
    qDebug() << "Iterations: " << rep.iterationscount;
    qDebug() << "Code: " << info;
    qDebug() << "nanoseconds elapsed: " << timer.nsecsElapsed();
}

void demandmodeling::FitExponential(const char *mStarts, QList<double> mParams)
{
    QElapsedTimer timer;
    timer.start();
    SetStarts(mStarts);

    lsfitcreatef(x, y, c, diffstep, state);
    lsfitsetbc(state, bndl, bndu);

    //real_1d_array s = "[1.0e+1, 1.0e-4]";
    //lsfitsetscale(state, s);

    lsfitsetcond(state, epsx, maxits);

    alglib::lsfitfit(state, exponential_demand, NULL, &mParams);

    lsfitresults(state, info, c, rep);

    qDebug() << "Q0: " << c[0];
    qDebug() << "alpha: " << c[1];
    qDebug() << "k (log range): " << mParams.first();
    qDebug() << "Iterations: " << rep.iterationscount;
    qDebug() << "Code: " << info;
    qDebug() << "nanoseconds elapsed: " << timer.nsecsElapsed();
}

void demandmodeling::FitExponentialWithK(const char *mStarts)
{
    QElapsedTimer timer;
    timer.start();
    SetStarts(mStarts);

    QList<double> mParams;
    mParams << 3;

    lsfitcreatef(x, y, c, diffstep, state);
    lsfitsetbc(state, bndl, bndu);

    real_1d_array s = "[1.0e+1, 1.0e-3, 1.0e+1]";
    lsfitsetscale(state, s);

    lsfitsetcond(state, epsx, maxits);

    alglib::lsfitfit(state, exponential_demand_with_k, NULL, &mParams);

    lsfitresults(state, info, c, rep);

    qDebug() << "Q0: " << c[1];
    qDebug() << "alpha: " << c[2];
    qDebug() << "K: " << c[0];
    qDebug() << "Iterations: " << rep.iterationscount;
    qDebug() << "Code: " << info;
    qDebug() << "nanoseconds elapsed: " << timer.nsecsElapsed();
}

void demandmodeling::FitExponentiated(const char *mStarts, QList<double> mParams)
{
    QElapsedTimer timer;
    timer.start();
    SetStarts(mStarts);

    lsfitcreatef(x, y, c, diffstep, state);
    lsfitsetbc(state, bndl, bndu);

    real_1d_array s = "[1.0, 1.0e-3]";
    lsfitsetscale(state, s);

    lsfitsetcond(state, epsx, maxits);

    alglib::lsfitfit(state, exponentiated_demand, NULL, &mParams);

    lsfitresults(state, info, c, rep);

    qDebug() << "Q0: " << c[0];
    qDebug() << "alpha: " << c[1];
    qDebug() << "k (log range): " << mParams.first();
    qDebug() << "Iterations: " << rep.iterationscount;
    qDebug() << "Code: " << info;
    qDebug() << "nanoseconds elapsed: " << timer.nsecsElapsed();
}

void demandmodeling::FitExponentiatedWithK(const char *mStarts)
{
    QElapsedTimer timer;
    timer.start();
    SetStarts(mStarts);

    lsfitcreatef(x, y, c, diffstep, state);
    lsfitsetbc(state, bndl, bndu);

    real_1d_array s = "[1.0, 1.0e-3, 1.0]";
    lsfitsetscale(state, s);

    lsfitsetcond(state, epsx, maxits);

    alglib::lsfitfit(state, exponentiated_demand_with_k);

    lsfitresults(state, info, c, rep);

    qDebug() << "Q0: " << c[1];
    qDebug() << "alpha: " << c[2];
    qDebug() << "K: " << c[0];
    qDebug() << "Iterations: " << rep.iterationscount;
    qDebug() << "Code: " << info;
    qDebug() << "nanoseconds elapsed: " << timer.nsecsElapsed();
}

demandmodeling::demandmodeling()
{

}
