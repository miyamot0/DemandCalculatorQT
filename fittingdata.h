#ifndef FITTINGDATA_H
#define FITTINGDATA_H

#include <QObject>

class FittingData
{
public:
    FittingData();

    FittingData(QString _Prices, QString _Consumption, QList<double> _PriceValues, QList<double> _ConsumptionValues, double _localMin, double _localMax);

    QString Prices;
    QString Consumption;

    QList<double> PriceValues;
    QList<double> ConsumptionValues;

    double LocalMin;
    double LocalMax;
};

#endif // FITTINGDATA_H
