#include "fitresult.h"

FitResult::FitResult()
{
    Model = DemandModel::Exponential;

    BIC = -1;
    AIC = -1;
}

FitResult::FitResult(DemandModel mModel)
{
    Model = mModel;

    BIC = -1;
    AIC = -1;
}
