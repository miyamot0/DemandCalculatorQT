#ifndef CALCULATIONWORKER_H
#define CALCULATIONWORKER_H

#include <QObject>

#include "demandmodeling.h"
#include "calculationsettings.h"
#include "fitresult.h"
#include "fittingdata.h"

class CalculationWorker : public QObject
{
    Q_OBJECT

public:
    CalculationWorker(QList<FittingData> mStoredValues, CalculationSettings *mCalculationSettings);

    DemandModel modelType;

    QList<FittingData> mLocalStoredValues;

    CalculationSettings calculationSettings;

    demandmodeling *mObj;

    QStringList mTempHolder;

    QString getCodeString(ae_int_t code);
    QString getKMessage(BehaviorK call);

    double getPbar(QStringList &xValues);

    QString getPmaxEString(QList<double> &yValues, QList<double> &xValues);
    QString getOmaxEString(QList<double> &yValues, QList<double> &xValues);
    QString getIntensityString(QList<double> &yValues, QList<double> &xValues);
    QString getBP0String(QList<double> &yValues, QList<double> &xValues);
    QString getBP1String(QList<double> &yValues, QList<double> &xValues);

signals:
    void workStarted();
    void workingResult(const QStringList &value);
    void workFinished();

public slots:
    void startWork();
    void working();
};

#endif // CALCULATIONWORKER_H
