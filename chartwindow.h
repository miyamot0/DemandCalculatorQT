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

#ifndef CHARTWINDOW_H
#define CHARTWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QWidget>
#include <QHBoxLayout>

#include <QMenu>
#include <QMenuBar>
#include <QDialog>
#include <QFile>
#include <QTextStream>
#include <QtCharts>

#include "calculationsettings.h"

class chartwindow : public QMainWindow
{
public:
    explicit chartwindow(QList<QStringList> stringList, bool showChartsStandardized, DemandModel mModel, QWidget *parent = 0);

    bool eventFilter(QObject *, QEvent *e);

    void buildLinearPlot();
    void plotLinearSeries(int index);

    void buildExponentialPlot();
    void plotExponentialSeries(int index);

    void buildExponentiatedPlot();
    void plotExponentiatedSeries(int index);

private slots:
    void on_NextButton_clicked();
    void on_PreviousButton_clicked();
    void saveSVGasPNG();

private:
    QChart *chart;
    QChartView *chartView;
    QLogValueAxis *axisX;
    QLogValueAxis *axisY;
    QValueAxis *axisY2;

    QList<QStringList> mDisplayData;

    int currentIndexShown;

    QAction *saveAction;
    QAction *prevAction;
    QAction *nextAction;

    QStringList mList;

    QStringList pricePoints;
    QStringList valuePoints;

    double lastPrice;

    QString rawPrices;
    QString rawValues;

    QLineSeries *pmaxLine;
    QLineSeries *demandCurve;
    QScatterSeries *dataPoints;

    DemandModel modelType;

    double deltaVar = 10;

    bool showStandardized;

    double param1;
    double param2;

    bool checkValue1;
    bool checkValue2;

    double derivedPmax;

    // Linear
    double linearL;
    double linearb;
    double lineara;

    // Exponential
    double exponentialAlpha;
    double exponentialQ0;
    double exponentialK;

    // Exponentiated
    double exponentiatedAlpha;
    double exponentiatedQ0;
    double exponentiatedK;
};

#endif // CHARTWINDOW_H
