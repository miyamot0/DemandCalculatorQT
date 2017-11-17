#ifndef FITRESULT_H
#define FITRESULT_H

#include "calculationsettings.h"

class FitResult
{
public:
    FitResult();
    FitResult(DemandModel mModel);

    DemandModel Model;

    QList<QPair<QString, double>> Params;

    double RMS;
    double BIC;
    double AIC;

    QString Status;
};

#endif // FITRESULT_H
