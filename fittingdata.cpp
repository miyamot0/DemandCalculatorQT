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

  */

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
