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

    QList<FittingData> mLocalStoredValues;
    QStringList mTempHolder;

    CalculationSettings calculationSettings;

    demandmodeling *mObj;
    DemandModel modelType;

    double getPbar(QList<double> &yValues);

    QString getCodeString(ae_int_t code);
    QString getKMessage(BehaviorK call);
    QString getPmaxEString(QList<double> &yValues, QList<double> &xValues);
    QString getOmaxEString(QList<double> &yValues, QList<double> &xValues);
    QString getIntensityString(QList<double> &yValues, QList<double> &xValues);
    QString getBP0String(QList<double> &yValues, QList<double> &xValues);
    QString getBP1String(QList<double> &yValues, QList<double> &xValues);

    QList<real_1d_array> sharedHolder;

signals:
    void workStarted();
    void statusUpdate(const QString &value);
    void workingResult(const QStringList &value);
    void workFinished();

public slots:
    void startWork();
    void working();
};

#endif // CALCULATIONWORKER_H
