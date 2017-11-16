#ifndef CALCULATIONWORKER_H
#define CALCULATIONWORKER_H

#include <QObject>
#include <QVector>

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

    real_1d_array savedGlobalFits;

    QVector<QVector<QString>> tempMatrix;

    QList<real_1d_array> arrayHolder;

    // Params
    QList<double> mParams;

    QVector<QPair<double, double>> bestParams;
    QVector<QPair<double, double>> paramHolder;

    QStringList bu,
                bl,
                starts,
                xHolder,
                yHolder,
                xReference;

    int cntr;
    int values;
    int counter;

    double lowerK;
    double upperK;
    double kSpan;
    double tempK;

    double lowerQ;
    double upperQ;
    double qSpan;
    double tempQ;

    double lowerA;
    double upperA;
    double aSpan;

    double holdingBestK;
    double holdingBestSSR;
    double holdingTempSSR;

    double alpha;
    double alphase;

    double k;
    QString kse;
    double q0;
    double q0se;
    double pmaxd;
    double omaxd;
    double EV;

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
