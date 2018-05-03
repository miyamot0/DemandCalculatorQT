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
#include <QVector>
#include <QSharedPointer>

#include "Models/calculationsettings.h"
#include "Libraries/numerics/numericsport.h"

#include "Utilities/qcustomplot.h"

class chartwindow : public QMainWindow
{
public:
    explicit chartwindow(QList<QStringList> stringList, bool showChartsStandardized, DemandModel mModel, bool alternativePmax, QWidget *parent = 0);

    bool eventFilter(QObject *, QEvent *e);

    void buildPlot();
    void buildLinearPlot();
    void plotLinearSeries(int index);

    void buildExponentialPlot();
    void plotExponentialSeries(int index);

    void buildExponentiatedPlot();
    void plotExponentiatedSeries(int index);

    void buildResidualPlot();
    void plotResiduals(int index);

    void buildResidualQQPlot();
    void plotQQResiduals(int index);

private slots:
    void on_NextButton_clicked();
    void on_PreviousButton_clicked();
    void saveSVGasPNG();

private:
    QVector<double> rawX, rawY;
    QVector<double> projX, projY;
    QVector<double> pMaxX, pMaxY;

    QVector<double> qqX, qqY;

    QVector<double> errPointX, errPointY;
    QVector<double> qqPointX, qqPointY;

    QCustomPlot *chart;
    QCustomPlot *chartError;
    QCustomPlot *chartQQ;

    QCPTextElement *titleMainChart;
    QCPTextElement *titleError;
    QCPTextElement *titleQQ;

    QVector<double> chartXTicks;
    QVector<QString> chartXLabels;

    QVector<double> chartYTicks;
    QVector<QString> chartYLabels;

    QVector<double> chartErrorTicks;
    QVector<QString> chartErrorLabels;

    bool isAlternativePmaxUsed;

    QFont mTitle = QFont("Serif", 14, -1, false),
          mLegendFont = QFont("Serif", 10, -1, false);

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

    DemandModel modelType;

    double deltaVar = 3;

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

    QStackedWidget stackedWidget;

    QString xValues;
    QString yValues;
    QStringList mSplitX;
    QStringList mSplitY;

    double tempX, tempY;
    double tempFitY;

    bool canConvertY;
    bool canConvertX;

    double minList, maxList;

    QList<double> yValuesTempHolder;

    double tempQ0, tempA, tempK;
    double tempLinearL, tempLinearA, tempLinearB;

    NumericsPort qqCalculations;

    double meanResid;
    double sdRes;
    double seriesCount;
    double cdf;
    double expectedValue;
    double zValue;

    double MeanResiduals()
    {
        double residualSum = 0;

        for (double val : yValuesTempHolder)
        {
            residualSum += val;
        }

        return residualSum / (double) yValuesTempHolder.length();
    }

    double VarianceResiduals() {
        double mu = MeanResiduals();

        double residualSumSquared = 0;

        for (double val : yValuesTempHolder)
        {
            residualSumSquared += (val-mu)*(val-mu);
        }

        return residualSumSquared / ((double) yValuesTempHolder.length() -1);
    }

    double StdDevResiduals() {
        return sqrt(VarianceResiduals());
    }

    int GetAxisMaxLog10(QVector<double> curr)
    {
        int lower = -3;

        for (int i = 0; i < curr.length(); i++)
        {
            while (pow(10, lower) < curr[i])
            {
                lower++;
            }
        }

        return lower;
    }

    int GetAxisMaxLog10(double curr)
    {
        int lower = -3;

        while (pow(10, lower) < curr)
        {
            lower++;
        }

        return lower;
    }
};

#endif // CHARTWINDOW_H
