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

#include <QtGui>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QtMath>
#include "chartwindow.h"

#include "Models/calculationsettings.h"

#include "Utilities/qcustomplot.h"

#include <QDebug>

chartwindow::chartwindow(QList<QStringList> stringList, bool showChartsStandardized, DemandModel mModel, bool alternativePmax, QWidget *parent)
{
    mDisplayData = stringList;
    modelType = mModel;
    showStandardized = showChartsStandardized;
    isAlternativePmaxUsed = alternativePmax;

    QVBoxLayout *windowLayout = new QVBoxLayout;

    currentIndexShown = 0;

    installEventFilter(this);

    chart = new QCustomPlot();
    chartError = new QCustomPlot();
    chartQQ = new QCustomPlot();

    // Create regular plot

    buildPlot();
    stackedWidget.addWidget(chart);

    buildResidualPlot();
    stackedWidget.addWidget(chartError);

    buildResidualQQPlot();
    stackedWidget.addWidget(chartQQ);

    windowLayout->addWidget(&stackedWidget);

    // Set layout in QWidget
    QWidget *window = new QWidget(parent);
    window->setLayout(windowLayout);

    QToolBar *tb = new QToolBar();

    saveAction = new QAction(tr("Save"), this);
    connect(saveAction, &QAction::triggered, this, &chartwindow::saveSVGasPNG);

    prevAction = new QAction(tr("Previous"), this);
    connect(prevAction, &QAction::triggered, this, &chartwindow::on_PreviousButton_clicked);

    nextAction = new QAction(tr("Next"), this);
    connect(nextAction, &QAction::triggered, this, &chartwindow::on_NextButton_clicked);

    tb->addAction(saveAction);
    tb->addAction(prevAction);
    tb->addAction(nextAction);

    windowLayout->setMenuBar(tb);
    setCentralWidget(window);
    resize(800, 600);

    setWindowFlags(Qt::WindowStaysOnTopHint);

    plotResiduals(0);
    plotQQResiduals(0);

}

/*  Build main plots
 *
 */
void chartwindow::buildPlot()
{
    QString mTitleDescription = (showStandardized) ? "Standardized " : "";

    if (modelType == DemandModel::Linear)
    {
        setWindowTitle(QString("%1Linear Demand Model Plots").arg(mTitleDescription));

        chart->legend->setVisible(true);

        chart->yAxis->setRangeLower(0.001);
        chart->yAxis->setLabel("Overall Consumption");
        chart->yAxis->setBasePen(QPen(Qt::black));
        chart->yAxis->setScaleType(QCPAxis::stLogarithmic);

        chart->xAxis->setScaleType(QCPAxis::stLogarithmic);
        chart->xAxis->setLabel("Unit Price");

        buildLinearPlot();
    }
    else if (modelType == DemandModel::Exponential)
    {
        setWindowTitle(QString("%1Exponential Demand Model Plots").arg(mTitleDescription));

        chart->legend->setVisible(true);

        chart->yAxis->setRangeLower(0.001);
        chart->yAxis->setLabel("Overall Consumption");
        chart->yAxis->setBasePen(QPen(Qt::black));
        chart->yAxis->setScaleType(QCPAxis::stLogarithmic);

        chart->xAxis->setScaleType(QCPAxis::stLogarithmic);
        chart->xAxis->setLabel("Unit Price");

        buildExponentialPlot();
    }
    else if (modelType == DemandModel::Exponentiated)
    {
        setWindowTitle(QString("%1Exponentiated Demand Model Plots").arg(mTitleDescription));

        chart->legend->setVisible(true);

        chart->yAxis->setRangeLower(0);
        chart->yAxis->setLabel("Overall Consumption");
        chart->yAxis->setBasePen(QPen(Qt::black));
        chart->yAxis->setScaleType(QCPAxis::stLinear);

        chart->xAxis->setScaleType(QCPAxis::stLogarithmic);
        chart->xAxis->setLabel("Unit Price");


        buildExponentiatedPlot();
    }
}

/*  Build residual plots (sequential)
 *
 */
void chartwindow::buildResidualPlot()
{
    titleError = new QCPTextElement(chartError, "test", QFont("sans", 12, QFont::Bold));

    chartError->plotLayout()->insertRow(0);
    chartError->plotLayout()->addElement(0, 0, titleError);

    chartError->legend->setVisible(true);

    chartError->yAxis->setLabel(tr("Error Value Value"));
    chartError->yAxis->setBasePen(QPen(Qt::black));
    chartError->yAxis->setScaleType(QCPAxis::stLinear);
    chartError->yAxis->setRangeLower(-1);
    chartError->yAxis->setRangeUpper(1);

    chartError->xAxis->setLabel(tr("Residual"));
    chartError->xAxis->setBasePen(QPen(Qt::black));
    chartError->xAxis->setScaleType(QCPAxis::stLinear);

    chartError->addGraph();
    chartError->graph(0)->setLineStyle(QCPGraph::lsNone);
    chartError->graph(0)->setName("Error");
    chartError->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
                                                     Qt::blue,
                                                     Qt::white,
                                                     7));

    chartError->addGraph();
    chartError->graph(1)->setLineStyle(QCPGraph::lsLine);
    chartError->graph(1)->setName("");
    chartError->graph(1)->removeFromLegend();
    chartError->graph(1)->setPen(QPen(Qt::black));
}

/*  Build residual plots (distributed)
 *
 */
void chartwindow::buildResidualQQPlot()
{
    titleQQ = new QCPTextElement(chartQQ, "test", QFont("sans", 12, QFont::Bold));

    chartQQ->plotLayout()->insertRow(0);
    chartQQ->plotLayout()->addElement(0, 0, titleQQ);

    chartQQ->legend->setVisible(false);

    chartQQ->yAxis->setLabel(tr("Observed Values"));
    chartQQ->yAxis->setBasePen(QPen(Qt::black));
    chartQQ->yAxis->setScaleType(QCPAxis::stLinear);
    chartQQ->yAxis->setRangeLower(-1);
    chartQQ->yAxis->setRangeUpper(1);

    chartQQ->xAxis->setLabel(tr("Theoretical Quantiles"));
    chartQQ->xAxis->setBasePen(QPen(Qt::black));
    chartQQ->xAxis->setScaleType(QCPAxis::stLinear);
    chartQQ->xAxis->setRangeLower(-2);
    chartQQ->xAxis->setRangeUpper(2);
    //chartQQ->xAxis->setAutoTickStep(false);
    //chartQQ->xAxis->setTickStep(1);

    chartQQ->addGraph();
    chartQQ->graph(0)->setLineStyle(QCPGraph::lsNone);
    chartQQ->graph(0)->setName("");
    chartQQ->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
                                                     Qt::blue,
                                                     Qt::white,
                                                     7));

    chartQQ->addGraph();
    chartQQ->graph(1)->setLineStyle(QCPGraph::lsLine);
    chartQQ->graph(1)->setName("");
    chartQQ->graph(1)->removeFromLegend();
    chartQQ->graph(1)->setPen(QPen(Qt::black));
}

/*  Build linear figure
 *
 */
void chartwindow::buildLinearPlot()
{
    titleMainChart = new QCPTextElement(chart, "test", QFont("sans", 12, QFont::Bold));

    chart->plotLayout()->insertRow(0);
    chart->plotLayout()->addElement(0, 0, titleMainChart);

    // Add Points
    chart->addGraph();
    chart->graph(0)->setLineStyle(QCPGraph::lsNone);
    chart->graph(0)->setName("Raw Data");
    chart->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
                                                     Qt::blue,
                                                     Qt::white,
                                                     7));

    chart->addGraph();
    chart->graph(1)->setLineStyle(QCPGraph::lsLine);
    chart->graph(1)->setName("Linear Demand");
    chart->graph(1)->setPen(QPen(Qt::black));

    chart->addGraph();
    chart->graph(2)->setLineStyle(QCPGraph::lsLine);
    chart->graph(2)->setName("Pmax");
    chart->graph(2)->setPen(QPen(Qt::red));

    plotLinearSeries(0);
}

/*  Plot linear series
 *  TODO Fix
 */
void chartwindow::plotLinearSeries(int index)
{
    mList = mDisplayData.at(index);

    if (mList[6].contains("---", Qt::CaseInsensitive))
    {
        titleMainChart->setText(QString("Participant #%1: Dropped").arg(QString::number(currentIndexShown + 1)));

        pMaxX.clear();
        pMaxY.clear();

        return;
    }

    titleMainChart->setText(QString("Participant #%1").arg(QString::number(currentIndexShown + 1)));

    rawPrices = mList.at(19);

    rawPrices = rawPrices.replace(QString("["), QString(""));
    rawPrices = rawPrices.replace(QString("]"), QString(""));

    QList<QString> rawPricesSplit = rawPrices.split(",");

    rawValues = mList.at(20);
    rawValues = rawValues.replace(QString("["), QString(""));
    rawValues = rawValues.replace(QString("]"), QString(""));

    QList<QString> rawValuesSplit = rawValues.split(",");

    pMaxX.clear();
    pMaxY.clear();

    bool checkValue;

    linearL = mList[6].toDouble(&checkValue);

    if (!checkValue) return;

    linearb = mList[8].toDouble(&checkValue);

    if (!checkValue) return;

    lineara = mList[10].toDouble(&checkValue);

    if (!checkValue) return;

    derivedPmax = mList[17].toDouble(&checkValue);

    if (!checkValue) return;

    double highestPrice = rawPricesSplit[rawPricesSplit.count() - 1].toDouble();

    double highestConsumption = -1;

    for (int i = 0; i < rawPricesSplit.length(); i++)
    {
        param1 = rawPricesSplit[i].toDouble(&checkValue1);
        param2 = rawValuesSplit[i].toDouble(&checkValue2);

        if (!checkValue1 || !checkValue2)
        {
            break;
        }

        if (param2 > highestConsumption)
        {
            highestConsumption = param2;
        }
    }

    double scaling = 100 / exp(highestConsumption);

    pMaxX.append(derivedPmax);
    pMaxY.append(0.001);

    if (showStandardized)
    {
        pMaxX.append(derivedPmax);
        pMaxY.append(qExp(log(linearL) + (linearb * log(derivedPmax)) - lineara * (derivedPmax)) * scaling);
    }
    else
    {
        pMaxX.append(derivedPmax);
        pMaxY.append(qExp(log(linearL) + (linearb * log(derivedPmax)) - lineara * (derivedPmax)));
    }

    chart->graph(2)->setName(QString("pMax: %1").arg(QString::number(derivedPmax)));

    rawX.clear();
    rawY.clear();

    for (int i = 0; i < rawPricesSplit.length(); i++)
    {
        param1 = rawPricesSplit[i].toDouble(&checkValue1);
        param2 = rawValuesSplit[i].toDouble(&checkValue2);

        if (!checkValue1 || !checkValue2)
        {
            break;
        }

        if (param1 <= 0)
        {
            param1 = 0.001;
        }

        if (param2 <= 0)
        {
            param2 = 0.001;
        }

        if (showStandardized)
        {
            rawX.append(param1);
            rawY.append(param2 * scaling);
        }
        else
        {
            rawX.append(param1);
            rawY.append(param2);
        }
    }

    chart->graph(0)->setData(rawX, rawY);

    for (double i = 0.001; i <= highestPrice * 10; )
    {
        double projectedValue = log(linearL) + (linearb * log(i)) - lineara * (i);

        if (projectedValue > 0)
        {
            if (showStandardized)
            {
                projX.append(i);
                projY.append(exp(projectedValue) * scaling);
            }
            else
            {
                projX.append(i);
                projY.append(exp(projectedValue));
            }
        }

        if (projectedValue > highestConsumption)
        {
            highestConsumption = projectedValue;
        }

        if (i <= 0.1)
        {
            i += 0.01 / deltaVar;
        }
        else if (i > 0.1 && i <= 1.0)
        {
            i += 0.1 / deltaVar;
        }
        else if (i > 1 && i <= 10)
        {
            i += 1 / deltaVar;
        }
        else if (i > 10 && i <= 100)
        {
            i += 9 / deltaVar;
        }
        else if (i > 100 && i <= 1000)
        {
            i += 99 / deltaVar;
        }
        else if (i > 1000 && i <= 10000)
        {
            i += 999 / deltaVar;
        }
        else if (i > 10000)
        {
            i += 9999 / deltaVar;
        }
    }

    chart->graph(1)->setData(projX, projY);
    chart->graph(2)->setData(pMaxX, pMaxY);

    chart->yAxis->setRangeUpper(highestConsumption * 2.0);

    chart->xAxis->setRangeLower(0.0001);
    chart->xAxis->setRangeUpper(highestPrice * 2);

    chart->replot();
}

/*  Build exponential figure
 *
 */
void chartwindow::buildExponentialPlot()
{
    titleMainChart = new QCPTextElement(chart, "test", QFont("sans", 12, QFont::Bold));

    chart->plotLayout()->insertRow(0);
    chart->plotLayout()->addElement(0, 0, titleMainChart);

    // Add Points
    chart->addGraph();
    chart->graph(0)->setLineStyle(QCPGraph::lsNone);
    chart->graph(0)->setName("Raw Data");
    chart->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
                                                     Qt::blue,
                                                     Qt::white,
                                                     7));

    chart->addGraph();
    chart->graph(1)->setLineStyle(QCPGraph::lsLine);
    chart->graph(1)->setName("Exponential Demand");
    chart->graph(1)->setPen(QPen(Qt::black));

    chart->addGraph();
    chart->graph(2)->setLineStyle(QCPGraph::lsLine);
    chart->graph(2)->setName("Pmax");
    chart->graph(2)->setPen(QPen(Qt::red));

    plotExponentialSeries(0);
}

/*  Plot exponential series
 *
 */
void chartwindow::plotExponentialSeries(int index)
{
    mList = mDisplayData.at(index);

    if (mList[2].contains("---", Qt::CaseInsensitive))
    {
        titleMainChart->setText(QString("Participant #%1: Dropped").arg(QString::number(currentIndexShown + 1)));

        pMaxX.clear();
        pMaxY.clear();

        return;
    }

    titleMainChart->setText(QString("Participant #%1").arg(QString::number(currentIndexShown + 1)));

    rawPrices = mList.at(22);
    rawPrices = rawPrices.replace(QString("["), QString(""));
    rawPrices = rawPrices.replace(QString("]"), QString(""));

    QList<QString> rawPricesSplit = rawPrices.split(",");

    rawValues = mList.at(23);
    rawValues = rawValues.replace(QString("["), QString(""));
    rawValues = rawValues.replace(QString("]"), QString(""));

    QList<QString> rawValuesSplit = rawValues.split(",");

    pMaxX.clear();
    pMaxY.clear();

    bool checkValue;

    exponentialAlpha = mList[2].toDouble(&checkValue);

    if (!checkValue) return;

    exponentialQ0 = mList[4].toDouble(&checkValue);

    if (!checkValue) return;

    exponentialK = mList[10].toDouble(&checkValue);

    if (!checkValue) return;

    derivedPmax = mList[14].toDouble(&checkValue);

    if (!checkValue) return;

    double highestPrice = rawPricesSplit[rawPricesSplit.count() - 1].toDouble();

    double highestConsumption = -1;

    for (int i = 0; i < rawPricesSplit.length(); i++)
    {
        param1 = rawPricesSplit[i].toDouble(&checkValue1);
        param2 = rawValuesSplit[i].toDouble(&checkValue2);

        if (!checkValue1 || !checkValue2)
        {
            break;
        }

        if (param2 > highestConsumption)
        {
            highestConsumption = param2;
        }
    }

    double scaling = 100 / highestConsumption;

    pMaxX.clear();
    pMaxY.clear();

    pMaxX.append(derivedPmax);
    pMaxY.append(0.001);

    if (showStandardized)
    {
        pMaxX.append(derivedPmax);
        pMaxY.append(pow(10, log10(exponentialQ0) + exponentialK * (exp(-exponentialAlpha * exponentialQ0 * derivedPmax) - 1)) * scaling);
    }
    else
    {
        pMaxX.append(derivedPmax);
        pMaxY.append(pow(10, (log10(exponentialQ0) + exponentialK * (exp(-exponentialAlpha * exponentialQ0 * derivedPmax) - 1))));
    }

    chart->graph(2)->setName(QString("pMax: %1").arg(QString::number(derivedPmax)));

    rawX.clear();
    rawY.clear();

    for (int i = 0; i < rawPricesSplit.length(); i++)
    {
        param1 = rawPricesSplit[i].toDouble(&checkValue1);
        param2 = rawValuesSplit[i].toDouble(&checkValue2);

        if (!checkValue1 || !checkValue2)
        {
            break;
        }

        if (param1 <= 0)
        {
            param1 = 0.001;
        }

        if (param2 <= 0)
        {
            param2 = 0.001;
        }

        if (showStandardized)
        {
            rawX.append(param1);
            rawY.append((param2) * scaling);
        }
        else
        {
            rawX.append(param1);
            rawY.append(param2);
        }
    }

    chart->graph(0)->setData(rawX, rawY);
    chart->yAxis->setRangeUpper(highestConsumption * 2.0);

    chart->xAxis->setRangeLower(0.0001);
    chart->xAxis->setRangeUpper(highestPrice * 2);

    projX.clear();
    projY.clear();

    for (double i = 0.001; i < highestPrice * 2; )
    {
        double projectedValue = log10(exponentialQ0) + exponentialK * (exp(-exponentialAlpha * exponentialQ0 * i) - 1);

        projectedValue = pow(10, projectedValue);

        if (projectedValue >= 0.001)
        {
            if (showStandardized)
            {
                projX.append(i);
                projY.append((projectedValue) * scaling);
            }
            else
            {
                projX.append(i);
                projY.append(projectedValue);
            }
        }

        if (projectedValue > highestConsumption)
        {
            highestConsumption = projectedValue;
        }

        if (i <= 0.1)
        {
            i += 0.01 / deltaVar;
        }
        else if (i > 0.1 && i <= 1.0)
        {
            i += 0.1 / deltaVar;
        }
        else if (i > 1 && i <= 10)
        {
            i += 1 / deltaVar;
        }
        else if (i > 10 && i <= 100)
        {
            i += 9 / deltaVar;
        }
        else if (i > 100 && i <= 1000)
        {
            i += 99 / deltaVar;
        }
        else if (i > 1000 && i <= 10000)
        {
            i += 999 / deltaVar;
        }
        else if (i > 10000)
        {
            i += 9999 / deltaVar;
        }
    }

    chart->graph(1)->setData(projX, projY);
    chart->graph(2)->setData(pMaxX, pMaxY);

    chart->replot();
}

/*  Build exponentiated figure
 *
 */
void chartwindow::buildExponentiatedPlot()
{
    titleMainChart = new QCPTextElement(chart, "test", QFont("sans", 12, QFont::Bold));

    chart->plotLayout()->insertRow(0);
    chart->plotLayout()->addElement(0, 0, titleMainChart);

    // Add Points
    chart->addGraph();
    chart->graph(0)->setLineStyle(QCPGraph::lsNone);
    chart->graph(0)->setName("Raw Data");
    chart->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
                                                     Qt::blue,
                                                     Qt::white,
                                                     7));

    chart->addGraph();
    chart->graph(1)->setLineStyle(QCPGraph::lsLine);
    chart->graph(1)->setName("Exponentiated Demand");
    chart->graph(1)->setPen(QPen(Qt::black));

    chart->addGraph();
    chart->graph(2)->setLineStyle(QCPGraph::lsLine);
    chart->graph(2)->setName("Pmax");
    chart->graph(2)->setPen(QPen(Qt::red));

    plotExponentiatedSeries(0);
}

/*  Plot exponential series
 *
 */
void chartwindow::plotExponentiatedSeries(int index)
{
    mList = mDisplayData.at(index);

    if (mList[2].contains("---", Qt::CaseInsensitive))
    {
        titleMainChart->setText(QString("Participant #%1: Dropped").arg(QString::number(currentIndexShown + 1)));

        pMaxX.clear();
        pMaxY.clear();

        return;
    }

    titleMainChart->setText(QString("Participant #%1").arg(QString::number(currentIndexShown + 1)));

    bool checkValue;

    exponentiatedAlpha = mList[2].toDouble(&checkValue);

    if (!checkValue) return;

    exponentiatedQ0 = mList[4].toDouble(&checkValue);

    if (!checkValue) return;

    exponentiatedK = mList[10].toDouble(&checkValue);

    if (!checkValue) return;

    derivedPmax = mList[14].toDouble(&checkValue);

    if (!checkValue) return;

    rawPrices = mList.at(22);
    rawPrices = rawPrices.replace(QString("["), QString(""));
    rawPrices = rawPrices.replace(QString("]"), QString(""));

    QList<QString> rawPricesSplit = rawPrices.split(",");

    rawValues = mList.at(23);
    rawValues = rawValues.replace(QString("["), QString(""));
    rawValues = rawValues.replace(QString("]"), QString(""));

    QList<QString> rawValuesSplit = rawValues.split(",");

    double highestPrice = rawPricesSplit[rawPricesSplit.count() - 1].toDouble();

    double highestConsumption = -1;

    for (int i = 0; i < rawPricesSplit.length(); i++)
    {
        param1 = rawPricesSplit[i].toDouble(&checkValue1);
        param2 = rawValuesSplit[i].toDouble(&checkValue2);

        if (!checkValue1 || !checkValue2)
        {
            break;
        }

        if (param2 > highestConsumption)
        {
            highestConsumption = param2;
        }
    }

    double scaling = 100 / highestConsumption;

    pMaxX.clear();
    pMaxY.clear();

    pMaxX.append(derivedPmax);
    pMaxY.append(0.001);


    if (showStandardized)
    {
        pMaxX.append(derivedPmax);
        pMaxY.append(pow(10, (exponentiatedQ0 * pow(10, (exponentiatedK * (exp(-exponentiatedAlpha * exponentiatedQ0 * derivedPmax) - 1)))) * scaling));
    }
    else
    {
        pMaxX.append(derivedPmax);
        pMaxY.append(exponentiatedQ0 * pow(10, (exponentiatedK * (exp(-exponentiatedAlpha * exponentiatedQ0 * derivedPmax) - 1))));
    }

    chart->graph(2)->setName(QString("pMax: %1").arg(QString::number(derivedPmax)));

    rawX.clear();
    rawY.clear();

    for (int i = 0; i < rawPricesSplit.length(); i++)
    {
        param1 = rawPricesSplit[i].toDouble(&checkValue1);
        param2 = rawValuesSplit[i].toDouble(&checkValue2);

        if (!checkValue1 || !checkValue2)
        {
            break;
        }

        if (param1 <= 0)
        {
            param1 = 0.001;
        }

        if (param2 <= 0)
        {
            param2 = 0.001;
        }

        if (showStandardized)
        {
            rawX.append(param1);
            rawY.append((param2) * scaling);
        }
        else
        {
            rawX.append(param1);
            rawY.append(param2);
        }
    }

    chart->graph(0)->setData(rawX, rawY);
    chart->yAxis->setRangeUpper(highestConsumption + 1);

    chart->xAxis->setRangeLower(0);
    chart->xAxis->setRangeUpper(highestPrice + 1);

    projX.clear();
    projY.clear();

    for (double i = 0.001; i < highestPrice * 2; )
    {
        double projectedValue = exponentiatedQ0 * pow(10, (exponentiatedK * (exp(-exponentiatedAlpha * exponentiatedQ0 * i) - 1)));

        if (projectedValue >= 0.001)
        {
            if (showStandardized)
            {
                projX.append(i);
                projY.append((projectedValue) * scaling);
            }
            else
            {
                projX.append(i);
                projY.append(projectedValue);
            }
        }

        if (projectedValue > highestConsumption)
        {
            highestConsumption = projectedValue;
        }

        if (i <= 0.1)
        {
            i += 0.01 / deltaVar;
        }
        else if (i > 0.1 && i <= 1.0)
        {
            i += 0.1 / deltaVar;
        }
        else if (i > 1 && i <= 10)
        {
            i += 1 / deltaVar;
        }
        else if (i > 10 && i <= 100)
        {
            i += 9 / deltaVar;
        }
        else if (i > 100 && i <= 1000)
        {
            i += 99 / deltaVar;
        }
        else if (i > 1000 && i <= 10000)
        {
            i += 999 / deltaVar;
        }
        else if (i > 10000)
        {
            i += 9999 / deltaVar;
        }
    }

    chart->graph(1)->setData(projX, projY);
    chart->graph(2)->setData(pMaxX, pMaxY);

    chart->replot();
}

/*  Plot residual series
 *
 */
void chartwindow::plotResiduals(int index)
{
    mList = mDisplayData.at(index);

    if (mList[2].contains("---", Qt::CaseInsensitive))
    {
        titleError->setText(QString("Residual Error for Participant #%1: Dropped").arg(QString::number(currentIndexShown + 1)));

        return;
    }

    titleError->setText(QString("Residual Error for Participant #%1").arg(QString::number(currentIndexShown + 1)));

    errPointX.clear();
    errPointY.clear();

    if (modelType == DemandModel::Exponential || modelType == DemandModel::Exponentiated)
    {
        if (isAlternativePmaxUsed)
        {
            xValues = mList[mList.length() - 4];
            yValues = mList[mList.length() - 3];
        }
        else
        {
            xValues = mList[mList.length() - 2];
            yValues = mList[mList.length() - 1];
        }
    }
    else
    {
        xValues = mList[mList.length() - 2];
        yValues = mList[mList.length() - 1];
    }

    QVector<double> xErr, yErr;

    xErr.append(0);
    yErr.append(0);

    xValues = xValues.remove('[');
    xValues = xValues.remove(']');

    mSplitX = xValues.split(',');

    yValues = yValues.remove('[');
    yValues = yValues.remove(']');

    mSplitY = yValues.split(',');

    canConvertY = false;
    canConvertX = false;

    yValuesTempHolder.clear();

    chartErrorTicks.clear();
    chartErrorLabels.clear();

    for (int j = 0; j < mSplitX.length() && j < mSplitY.length(); j++)
    {
        tempX = mSplitX[j].toDouble(&canConvertX);
        tempY = mSplitY[j].toDouble(&canConvertY);

        //tempX = (tempX == 0) ? 0.01 : tempX;

        if (modelType == DemandModel::Exponential || modelType == DemandModel::Exponentiated)
        {
            tempA = mList[2].toDouble();
            tempQ0 = mList[4].toDouble();

            if (modelType == DemandModel::Exponential)
            {
                tempK = mList[9].toDouble();

                tempFitY = log10(tempQ0) + tempK * (exp(-tempA * tempQ0 * tempX) - 1);
                tempFitY = pow(10, tempFitY);

                tempY = tempY;
            }
            else if (modelType == DemandModel::Exponentiated)
            {
                tempK = mList[10].toDouble();

                tempFitY = tempQ0 * pow(10, (tempK * (exp(-tempA * tempQ0 * tempX) - 1)));

                tempY = tempY;
            }
        }
        else
        {
            tempLinearL = mList[6].toDouble();
            tempLinearB = mList[8].toDouble();
            tempLinearA = mList[10].toDouble();

            tempFitY = log(tempLinearL) + (tempLinearB * log(tempX)) - tempLinearA * tempX;

            tempY = log(tempY);
        }

        if (canConvertX && canConvertY)
        {
            errPointX.append(j+1);
            errPointY.append(tempY - tempFitY);

            xErr.append(j+1);
            yErr.append(0);

            chartErrorTicks.append(j+1);
            chartErrorLabels.append(QString::number(j+1));

            yValuesTempHolder.append((tempY - tempFitY));
        }
    }

    if (mSplitX.length() > 0)
    {
        minList = std::abs(*std::min_element(yValuesTempHolder.begin(), yValuesTempHolder.end())) * 1.5;
        maxList = std::abs(*std::max_element(yValuesTempHolder.begin(), yValuesTempHolder.end())) * 1.5;

        chartError->yAxis->setRangeLower((maxList >= minList) ? -maxList : -minList);
        chartError->yAxis->setRangeUpper((maxList >= minList) ? maxList : minList);

        chartError->xAxis->setRangeLower(0);
        chartError->xAxis->setRangeUpper(errPointY.length());
    }

    chartError->graph(0)->setData(errPointX, errPointY);
    chartError->graph(1)->setData(xErr, yErr);

    QSharedPointer<QCPAxisTickerText> chartErrorTicker(new QCPAxisTickerText);
    chartErrorTicker->addTicks(chartErrorTicks, chartErrorLabels);
    chartError->xAxis->setTicker(chartErrorTicker);

    chartError->replot();
}

/*  Plot residual series (distributed)
 *
 */
void chartwindow::plotQQResiduals(int index)
{
    mList = mDisplayData.at(index);

    if (mList[2].contains("---", Qt::CaseInsensitive))
    {
        titleQQ->setText(QString("QQ Plot for Participant #%1: Dropped").arg(QString::number(currentIndexShown + 1)));

        return;
    }

    titleQQ->setText(QString("QQ Plot for Participant #%1").arg(QString::number(currentIndexShown + 1)));

    if (modelType == DemandModel::Exponential || modelType == DemandModel::Exponentiated)
    {
        if (isAlternativePmaxUsed)
        {
            xValues = mList[mList.length() - 4];
            yValues = mList[mList.length() - 3];
        }
        else
        {
            xValues = mList[mList.length() - 2];
            yValues = mList[mList.length() - 1];
        }
    }
    else
    {
        xValues = mList[mList.length() - 2];
        yValues = mList[mList.length() - 1];
    }

    xValues = xValues.remove('[');
    xValues = xValues.remove(']');

    mSplitX = xValues.split(',');

    yValues = yValues.remove('[');
    yValues = yValues.remove(']');

    mSplitY = yValues.split(',');

    canConvertY = false;
    canConvertX = false;

    yValuesTempHolder.clear();

    qqPointX.clear();
    qqX.clear();

    qqPointY.clear();
    qqY.clear();

    for (int j = 0; j < mSplitX.length() && j < mSplitY.length(); j++)
    {
        tempX = mSplitX[j].toDouble(&canConvertX);
        tempY = mSplitY[j].toDouble(&canConvertY);

        tempX = (tempX == 0) ? 0.01 : tempX;

        if (modelType == DemandModel::Exponential || modelType == DemandModel::Exponentiated)
        {
            tempA = mList[2].toDouble();
            tempQ0 = mList[4].toDouble();

            if (modelType == DemandModel::Exponential)
            {
                tempK = mList[9].toDouble();

                tempFitY = log10(tempQ0) + tempK * (exp(-tempA * tempQ0 * tempX) - 1);

                tempY = log10(tempY);
            }
            else if (modelType == DemandModel::Exponentiated)
            {
                tempK = mList[10].toDouble();

                tempFitY = tempQ0 * pow(10, (tempK * (exp(-tempA * tempQ0 * tempX) - 1)));

                tempY = tempY;
            }
        }

        if (canConvertX && canConvertY)
        {
            yValuesTempHolder.append((tempY - tempFitY));
        }
    }

    meanResid = MeanResiduals();
    sdRes = StdDevResiduals();

    std::sort(yValuesTempHolder.begin(), yValuesTempHolder.end());

    seriesCount = (double) yValuesTempHolder.length();
    cdf = 0.0;
    expectedValue = 0;
    zValue = 0;

    for (int j = 0; j < yValuesTempHolder.count(); j++)
    {
        if (j == 0)
        {
            cdf = 1.0 / (2.0 * seriesCount);
        }
        else
        {
            cdf += 2.0 / (2.0 * seriesCount);
        }

        expectedValue = qqCalculations.NormInv(cdf, meanResid, sdRes);

        zValue = qqCalculations.InvCDF(0, 1, cdf);

        qqX.append(zValue);
        qqY.append(expectedValue);

        qqPointX.append(zValue);
        qqPointY.append(yValuesTempHolder[j]);

        //errSeriesQQ << QPointF(zValue, expectedValue);
        //errDataPointsQQ << QPointF(zValue, yValuesTempHolder[j]);
    }

    if (mSplitX.length() > 0)
    {
        minList = std::abs(*std::min_element(yValuesTempHolder.begin(), yValuesTempHolder.end())) * 1.5;
        maxList = std::abs(*std::max_element(yValuesTempHolder.begin(), yValuesTempHolder.end())) * 1.5;

        chartQQ->yAxis->setRangeLower((maxList >= minList) ? -maxList : -minList);
        chartQQ->yAxis->setRangeUpper((maxList >= minList) ? maxList : minList);
    }

    chartQQ->xAxis->setRangeLower(-2);
    chartQQ->xAxis->setRangeUpper(2);

    chartQQ->graph(0)->setData(qqPointX, qqPointY);
    chartQQ->graph(1)->setData(qqX, qqY);

    //chartErrorQQ.setTitle(QString("Participant #%1: Probability Plot").arg(QString::number(currentIndexShown + 1)));
    chartQQ->replot();
}

// Helpers

/**
 * @brief chartwindow::eventFilter
 * @param e
 * @return
 */
bool chartwindow::eventFilter(QObject *, QEvent *e)
{
    if (e->type() == QEvent::ShortcutOverride)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);

        if (keyEvent->key() == Qt::Key::Key_Left)
        {
            on_PreviousButton_clicked();
        }
        else if (keyEvent->key() == Qt::Key::Key_Right)
        {
            on_NextButton_clicked();
        }
        else if (keyEvent->key() == Qt::Key::Key_Up)
        {
            if (stackedWidget.currentIndex() <= 0)
            {
                return false;
            }

            stackedWidget.setCurrentIndex(stackedWidget.currentIndex() - 1);
        }
        else if (keyEvent->key() == Qt::Key::Key_Down)
        {
            if (stackedWidget.currentIndex() >= (stackedWidget.count() - 1))
            {
                return false;
            }

            stackedWidget.setCurrentIndex(stackedWidget.currentIndex() + 1);
        }
    }

    return false;
}

/**
 * @brief chartwindow::saveSVGasPNG
 */
void chartwindow::saveSVGasPNG()
{
    QString file_name;
    QString fileFilter = "PNG (*.png)";

#ifdef _WIN32

        file_name = QFileDialog::getSaveFileName(this, "Save PNG", QDir::homePath(),
                                         fileFilter);

#elif TARGET_OS_MAC

        file_name = QFileDialog::getSaveFileName(this, "Save PNG", QDir::homePath(),
                                         fileFilter, &fileFilter, QFileDialog::Option::DontUseNativeDialog);

        if (!file_name.contains(".png"))
        {
            file_name.append(".png");
        }

#endif

    if(!file_name.trimmed().isEmpty())
    {
        switch (stackedWidget.currentIndex()) {
        case 0:
            chart->savePng(file_name);
            break;
        case 1:
            chartError->savePng(file_name);
            break;
        case 2:
            chartQQ->savePng(file_name);
            break;
        default:
            break;
        }
    }
}

/**
 * @brief chartwindow::on_NextButton_clicked
 */
void chartwindow::on_NextButton_clicked()
{
    if (currentIndexShown >= mDisplayData.count() - 1)
    {
        return;
    }

    currentIndexShown++;

    if (modelType == DemandModel::Linear)
    {
        plotLinearSeries(currentIndexShown);
    }
    else if (modelType == DemandModel::Exponential)
    {
        plotExponentialSeries(currentIndexShown);
    }
    else if (modelType == DemandModel::Exponentiated)
    {
        plotExponentiatedSeries(currentIndexShown);
    }

    plotResiduals(currentIndexShown);
    plotQQResiduals(currentIndexShown);
}

/**
 * @brief chartwindow::on_PreviousButton_clicked
 */
void chartwindow::on_PreviousButton_clicked()
{
    if (currentIndexShown <= 0)
    {
        return;
    }

    currentIndexShown--;

    if (modelType == DemandModel::Linear)
    {
        plotLinearSeries(currentIndexShown);
    }
    else if (modelType == DemandModel::Exponential)
    {
        plotExponentialSeries(currentIndexShown);
    }
    else if (modelType == DemandModel::Exponentiated)
    {
        plotExponentiatedSeries(currentIndexShown);
    }

    plotResiduals(currentIndexShown);
    plotQQResiduals(currentIndexShown);
}
