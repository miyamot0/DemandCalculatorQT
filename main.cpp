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
#include "fitresult.h"
#include <QDebug>

#include "demandmodeling.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qRegisterMetaType<FitResult>("FitResult");

    SheetWidget mNewSheet;
    mNewSheet.setWindowIcon(QPixmap(":/images/applications-other.png"));
    mNewSheet.show();

    try
    {
        demandmodeling mModel;
        mModel.TestMixedModel();
    }
    catch(alglib::ap_error err)
    {
        qDebug() << err.msg.c_str();
    }


    return app.exec();
}
