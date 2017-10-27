#include "fittingdata.h"

FittingData::FittingData()
{

}

FittingData::FittingData(QString _Prices, QString _Consumption, QList<double> _PriceValues, QList<double> _ConsumptionValues, double _localMin, double _localMax)
{
    Prices = _Prices;
    Consumption = _Consumption;

    PriceValues = _PriceValues;
    ConsumptionValues = _ConsumptionValues;

    LocalMin = _localMin;
    LocalMax = _localMax;
}
