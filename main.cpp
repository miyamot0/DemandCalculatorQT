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

#include <QApplication>
#include "sheetwidget.h"

//#include "DifferentialEvolution.h"
//#include "TestFunctions.h"
//#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SheetWidget mNewSheet;
    mNewSheet.setWindowIcon(QPixmap(":/images/applications-other.png"));
    mNewSheet.show();

    /*
    double values[11] = { 1000,
                          1000,
                          1000,
                          800,
                          800,
                          700,
                          600,
                          500,
                          400,
                          200,
                          100 };

    double price[11] = {0.0,
                        0.5,
                        1.0,
                        1.5,
                        2.0,
                        2.5,
                        3.0,
                        4.0,
                        5.0,
                        10.0,
                        15.0};

    unsigned int popSize = 100;
    int lengthOfArray = 11;
    double upperQ0Bound = 100000;
    double upperKBound = 100;
    */

    //de::ExponentialDemand objectiveExponentialFunction(price, values, lengthOfArray, upperQ0Bound);
    //de::DifferentialEvolution de(objectiveExponentialFunction, popSize);

    //de::ExponentialDemandFitK objectiveExponentialFunctionFitK(price, values, lengthOfArray, upperQ0Bound, upperKBound);
    //de::DifferentialEvolution de(objectiveExponentialFunctionFitK, popSize);

    //de::ExponentiatedDemand objectiveExponentiatedFunction(price, values, lengthOfArray, upperQ0Bound);
    //de::DifferentialEvolution de(objectiveExponentiatedFunction, popSize);

    //de::ExponentiatedDemandFitK objectiveExponentiatedFunctionFitK(price, values, lengthOfArray, upperQ0Bound, upperKBound);
    //de::DifferentialEvolution de(objectiveExponentiatedFunctionFitK, popSize);

    //de.Optimize(1000, false);

    //auto bestAgent = de.GetBestAgent();

    //qDebug() << bestAgent;

    return app.exec();
}
