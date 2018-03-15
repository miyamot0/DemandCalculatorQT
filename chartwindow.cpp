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
#include "calculationsettings.h"

chartwindow::chartwindow(QList<QStringList> stringList, bool showChartsStandardized, DemandModel mModel, bool alternativePmax, QWidget *parent)
{
    mDisplayData = stringList;
    modelType = mModel;
    showStandardized = showChartsStandardized;
    isAlternativePmaxUsed = alternativePmax;

    QVBoxLayout *windowLayout = new QVBoxLayout;

    currentIndexShown = 0;

    installEventFilter(this);

    chart = new QChart();

    // Create regular plot

    buildPlot();
    stackedWidget.addWidget(chartView);

    buildResidualPlot();
    stackedWidget.addWidget(chartViewError);

    buildResidualQQPlot();
    stackedWidget.addWidget(chartViewErrorQQ);

    //windowLayout->addWidget(chartView);
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

void chartwindow::buildPlot()
{
    QFont mTitle("Serif", 16, -1, false);
    chart->setTitleFont(mTitle);

    auto mLegend = chart->legend();

    QFont mLegendFont("Serif", 12, -1, false);
    mLegend->setFont(mLegendFont);
    mLegend->setAlignment(Qt::AlignBottom);

    axisX = new QLogValueAxis;
    //axisX->setGridLineColor(Qt::transparent);
    axisX->setTitleText("Unit Price");
    axisX->setBase(10);
    axisX->setLabelsFont(mLegendFont);
    axisX->setLinePenColor(Qt::black);
    axisX->setLinePen(QPen(Qt::black));
    axisX->setMin(0.001);

    QString mTitleDescription = (showStandardized) ? "Standardized " : "";

    if (modelType == DemandModel::Linear)
    {
        setWindowTitle(QString("%1Linear Demand Model Plots").arg(mTitleDescription));

        axisY = new QLogValueAxis;
        axisY->setBase(10);
        //axisY->setGridLineColor(Qt::transparent);
        axisY->setTitleText("Overall Consumption");
        axisY->setLabelsFont(mLegendFont);
        axisY->setLinePenColor(Qt::black);
        axisY->setLinePen(QPen(Qt::black));
        axisY->setMin(0.001);

        buildLinearPlot();
    }
    else if (modelType == DemandModel::Exponential)
    {
        setWindowTitle(QString("%1Exponential Demand Model Plots").arg(mTitleDescription));

        axisY = new QLogValueAxis;
        axisY->setBase(10);
        //axisY->setGridLineColor(Qt::transparent);
        axisY->setTitleText("Overall Consumption");
        axisY->setLabelsFont(mLegendFont);
        axisY->setLinePenColor(Qt::black);
        axisY->setLinePen(QPen(Qt::black));
        axisY->setMin(0.001);

        buildExponentialPlot();
    }
    else if (modelType == DemandModel::Exponentiated)
    {
        setWindowTitle(QString("%1Exponentiated Demand Model Plots").arg(mTitleDescription));
        setWindowTitle("Exponentiated Demand Model Plots");

        axisY2 = new QValueAxis;
        //axisY2->setGridLineColor(Qt::transparent);
        axisY2->setTitleText("Overall Consumption");
        axisY2->setTickCount(5);
        axisY2->setLabelsFont(mLegendFont);
        axisY2->setLinePenColor(Qt::black);
        axisY2->setLinePen(QPen(Qt::black));
        axisY2->setMin(0.001);

        buildExponentiatedPlot();
    }

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
}

void chartwindow::buildResidualPlot()
{
    chartError.setTitleFont(mTitle);
    chartError.setFont(mTitle);
    chartError.setTitleBrush(Qt::black);

    chartError.legend()->setFont(mLegendFont);
    chartError.legend()->setAlignment(Qt::AlignBottom);
    chartError.legend()->setBrush(Qt::black);
    chartError.legend()->setColor(Qt::black);

    // do we need a table?
    chartError.legend()->setVisible(false);

    axisXerror.setGridLineColor(Qt::transparent);
    axisXerror.setTitleText(tr("Residual"));
    axisXerror.setMin(0);
    axisXerror.setLabelsFont(QFont("Serif", 10, -1, false));
    axisXerror.setLabelsColor(Qt::black);
    axisXerror.setLabelFormat(QString("%.0f"));
    axisXerror.setLinePenColor(Qt::black);
    axisXerror.setLinePen(QPen(Qt::black));
    axisXerror.setTitleBrush(Qt::black);
    axisXerror.setTitleFont(QFont("Serif", 10, -1, false));

    axisYerror.setGridLineColor(Qt::transparent);
    axisYerror.setTitleText(tr("Error Value Value"));
    axisYerror.setTickCount(9);
    axisYerror.setLabelsFont(QFont("Serif", 10, -1, false));
    axisYerror.setLabelsColor(Qt::black);
    axisYerror.setMin(-1);
    axisYerror.setMax(1);
    axisYerror.setLinePenColor(Qt::black);
    axisYerror.setLinePen(QPen(Qt::black));
    axisYerror.setTitleBrush(Qt::black);
    axisYerror.setTitleFont(QFont("Serif", 10, -1, false));

    errSeries.setUseOpenGL(true);
    errSeries.setName("");
    errSeries.setPointsVisible(false);
    errSeries.setPen(QPen(Qt::black));
    chartError.addSeries(&errSeries);

    errDataPoints.setName(tr("err"));
    errDataPoints.setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    errDataPoints.setPen(QPen(Qt::black));
    errDataPoints.setBrush(QBrush(Qt::black));
    errDataPoints.setMarkerSize(10);
    chartError.addSeries(&errDataPoints);

    chartError.setAxisX(&axisXerror, &errSeries);
    chartError.setAxisY(&axisYerror, &errSeries);

    chartError.setAxisX(&axisXerror, &errDataPoints);
    chartError.setAxisY(&axisYerror, &errDataPoints);

    chartViewError = new QChartView(&chartError);
}

void chartwindow::buildResidualQQPlot()
{
    chartErrorQQ.setTitleFont(mTitle);
    chartErrorQQ.setFont(mTitle);
    chartErrorQQ.setTitleBrush(Qt::black);

    chartErrorQQ.legend()->setFont(mLegendFont);
    chartErrorQQ.legend()->setAlignment(Qt::AlignBottom);
    chartErrorQQ.legend()->setBrush(Qt::black);
    chartErrorQQ.legend()->setColor(Qt::black);

    // do we need a table?
    chartErrorQQ.legend()->setVisible(false);

    axisXerrorQQ.setGridLineColor(Qt::transparent);
    axisXerrorQQ.setTitleText(tr("Theoretical Quantiles"));
    axisXerrorQQ.setTickCount(5);
    axisXerrorQQ.setMin(-2);
    axisXerrorQQ.setMax(2);
    axisXerrorQQ.setLabelsFont(QFont("Serif", 10, -1, false));
    axisXerrorQQ.setLabelsColor(Qt::black);
    axisXerrorQQ.setLabelFormat(QString("%.0f"));
    axisXerrorQQ.setLinePenColor(Qt::black);
    axisXerrorQQ.setLinePen(QPen(Qt::black));
    axisXerrorQQ.setTitleBrush(Qt::black);
    axisXerrorQQ.setTitleFont(QFont("Serif", 10, -1, false));

    axisYerrorQQ.setGridLineColor(Qt::transparent);
    axisYerrorQQ.setTitleText(tr("Observed Values"));
    axisYerrorQQ.setTickCount(9);
    axisYerrorQQ.setLabelsFont(QFont("Serif", 10, -1, false));
    axisYerrorQQ.setLabelsColor(Qt::black);
    axisYerrorQQ.setMin(-1);
    axisYerrorQQ.setMax(1);
    axisYerrorQQ.setLinePenColor(Qt::black);
    axisYerrorQQ.setLinePen(QPen(Qt::black));
    axisYerrorQQ.setTitleBrush(Qt::black);
    axisYerrorQQ.setTitleFont(QFont("Serif", 10, -1, false));

    errSeriesQQ.setUseOpenGL(true);
    errSeriesQQ.setName("");
    errSeriesQQ.setPointsVisible(false);
    errSeriesQQ.setPen(QPen(Qt::black));
    chartErrorQQ.addSeries(&errSeriesQQ);

    errDataPointsQQ.setName(tr("err"));
    errDataPointsQQ.setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    errDataPointsQQ.setPen(QPen(Qt::black));
    errDataPointsQQ.setBrush(QBrush(Qt::black));
    errDataPointsQQ.setMarkerSize(10);
    chartErrorQQ.addSeries(&errDataPointsQQ);

    chartErrorQQ.setAxisX(&axisXerrorQQ, &errSeriesQQ);
    chartErrorQQ.setAxisY(&axisYerrorQQ, &errSeriesQQ);

    chartErrorQQ.setAxisX(&axisXerrorQQ, &errDataPointsQQ);
    chartErrorQQ.setAxisY(&axisYerrorQQ, &errDataPointsQQ);

    chartViewErrorQQ = new QChartView(&chartErrorQQ);
}

void chartwindow::buildLinearPlot()
{
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    dataPoints = new QScatterSeries();
    dataPoints->setName("Raw Data");
    dataPoints->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    dataPoints->setPen(QPen(Qt::black));
    dataPoints->setBrush(QBrush(Qt::black));
    dataPoints->setMarkerSize(10);
        chart->addSeries(dataPoints);

        dataPoints->attachAxis(axisX);
        dataPoints->attachAxis(axisY);

    demandCurve = new QLineSeries();
    demandCurve->setName("Linear Demand");
    demandCurve->setPen(QPen(Qt::black));
        chart->addSeries(demandCurve);

        demandCurve->attachAxis(axisX);
        demandCurve->attachAxis(axisY);

    pmaxLine = new QLineSeries();
    pmaxLine->setName("pMax");
    pmaxLine->setPen(QPen(Qt::red));
        chart->addSeries(pmaxLine);

        pmaxLine->attachAxis(axisX);
        pmaxLine->attachAxis(axisY);

    plotLinearSeries(0);
}

void chartwindow::plotLinearSeries(int index)
{
    mList = mDisplayData.at(index);

    demandCurve->clear();
    dataPoints->clear();

    if (mList[6].contains("---", Qt::CaseInsensitive))
    {
        chart->setTitle(QString("Participant #%1: Dropped").arg(QString::number(currentIndexShown + 1)));
        pmaxLine->clear();
        pmaxLine->setName("pMax");

        return;
    }

    rawPrices = mList.at(19);

    rawPrices = rawPrices.replace(QString("["), QString(""));
    rawPrices = rawPrices.replace(QString("]"), QString(""));

    QList<QString> rawPricesSplit = rawPrices.split(",");

    rawValues = mList.at(20);
    rawValues = rawValues.replace(QString("["), QString(""));
    rawValues = rawValues.replace(QString("]"), QString(""));

    QList<QString> rawValuesSplit = rawValues.split(",");

    chart->setTitle(QString("Participant #%1").arg(QString::number(currentIndexShown + 1)));

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

    pmaxLine->clear();

    *pmaxLine << QPointF(derivedPmax, 0.001);

    if (showStandardized)
    {
        *pmaxLine << QPointF(derivedPmax, qExp(log(linearL) + (linearb * log(derivedPmax)) - lineara * (derivedPmax)) * scaling);
    }
    else
    {
        *pmaxLine << QPointF(derivedPmax, qExp(log(linearL) + (linearb * log(derivedPmax)) - lineara * (derivedPmax)));
    }

    pmaxLine->setName(QString("pMax: %1").arg(QString::number(derivedPmax)));

    axisX->setMax(highestPrice * 2.0);

    axisY->setMin(0.001);

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
            *dataPoints << QPointF(param1, (param2) * scaling);
        }
        else
        {
            *dataPoints << QPointF(param1, (param2));
        }
    }

    for (double i = 0.001; i <= highestPrice * 10; )
    {
        double projectedValue = log(linearL) + (linearb * log(i)) - lineara * (i);

        if (projectedValue > 0)
        {
            if (showStandardized)
            {
                *demandCurve << QPointF(i, exp(projectedValue) * scaling);
            }
            else
            {
                *demandCurve << QPointF(i, exp(projectedValue));
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

    if (showStandardized)
    {
        axisY->setMax(200);
    }
    else
    {
        axisY->setMax(highestConsumption * 2);
    }

    axisX->setMax(highestPrice * 2);
    axisX->setMin(0.01);
}

void chartwindow::buildExponentialPlot()
{
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    dataPoints = new QScatterSeries();
    dataPoints->setName("Raw Data");
    dataPoints->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    dataPoints->setPen(QPen(Qt::black));
    dataPoints->setBrush(QBrush(Qt::black));
    dataPoints->setMarkerSize(10);
        chart->addSeries(dataPoints);

        dataPoints->attachAxis(axisX);
        dataPoints->attachAxis(axisY);

    demandCurve = new QLineSeries();
    demandCurve->setName("Exponential Demand");
    demandCurve->setPen(QPen(Qt::black));
        chart->addSeries(demandCurve);

        demandCurve->attachAxis(axisX);
        demandCurve->attachAxis(axisY);

    pmaxLine = new QLineSeries();
    pmaxLine->setName("pMax");
    pmaxLine->setPen(QPen(Qt::red));
        chart->addSeries(pmaxLine);

        pmaxLine->attachAxis(axisX);
        pmaxLine->attachAxis(axisY);

    plotExponentialSeries(0);
}

void chartwindow::plotExponentialSeries(int index)
{
    mList = mDisplayData.at(index);

    demandCurve->clear();
    dataPoints->clear();

    if (mList[2].contains("---", Qt::CaseInsensitive))
    {
        chart->setTitle(QString("Participant #%1: Dropped").arg(QString::number(currentIndexShown + 1)));
        pmaxLine->clear();
        pmaxLine->setName("pMax");

        return;
    }

    rawPrices = mList.at(21);
    rawPrices = rawPrices.replace(QString("["), QString(""));
    rawPrices = rawPrices.replace(QString("]"), QString(""));

    QList<QString> rawPricesSplit = rawPrices.split(",");

    rawValues = mList.at(22);
    rawValues = rawValues.replace(QString("["), QString(""));
    rawValues = rawValues.replace(QString("]"), QString(""));

    QList<QString> rawValuesSplit = rawValues.split(",");

    chart->setTitle(QString("Participant #%1").arg(QString::number(currentIndexShown + 1)));

    bool checkValue;

    exponentialAlpha = mList[2].toDouble(&checkValue);

    if (!checkValue) return;

    exponentialQ0 = mList[4].toDouble(&checkValue);

    if (!checkValue) return;

    exponentialK = mList[9].toDouble(&checkValue);

    if (!checkValue) return;

    derivedPmax = mList[13].toDouble(&checkValue);

    if (!checkValue) return;

    double highestPrice = rawPricesSplit[rawPricesSplit.count() - 1].toDouble();

    axisX->setMax(highestPrice * 2.0);

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

    pmaxLine->clear();

    *pmaxLine << QPointF(derivedPmax, 0.001);

    if (showStandardized)
    {
        *pmaxLine << QPointF(derivedPmax, pow(10, log10(exponentialQ0) + exponentialK * (exp(-exponentialAlpha * exponentialQ0 * derivedPmax) - 1)) * scaling);
    }
    else
    {
        *pmaxLine << QPointF(derivedPmax, pow(10, (log10(exponentialQ0) + exponentialK * (exp(-exponentialAlpha * exponentialQ0 * derivedPmax) - 1))));
    }

    pmaxLine->setName(QString("pMax: %1").arg(QString::number(derivedPmax)));

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
            *dataPoints << QPointF(param1, (param2) * scaling);
        }
        else
        {
            *dataPoints << QPointF(param1, (param2));
        }
    }

    for (double i = 0.001; i < highestPrice * 2; )
    {
        double projectedValue = log10(exponentialQ0) + exponentialK * (exp(-exponentialAlpha * exponentialQ0 * i) - 1);

        projectedValue = pow(10, projectedValue);

        if (projectedValue >= 0.001)
        {
            if (showStandardized)
            {
                *demandCurve << QPointF(i, (projectedValue) * scaling);
            }
            else
            {
                *demandCurve << QPointF(i, projectedValue);
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

    if (showStandardized)
    {
        axisY->setMax(200);
        axisY->setMin(0.001);
    }
    else
    {
        axisY->setMax(highestConsumption * 2);
        axisY->setMin(0.001);
    }

    axisX->setMax(highestPrice * 2);
    axisX->setMin(0.001);
}

void chartwindow::buildExponentiatedPlot()
{
    demandCurve = new QLineSeries();
    demandCurve->setName("Exponentiated Demand");
    demandCurve->setPen(QPen(Qt::black));
        chart->addSeries(demandCurve);

    dataPoints = new QScatterSeries();
    dataPoints->setName("Raw Data");
    dataPoints->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    dataPoints->setPen(QPen(Qt::black));
    dataPoints->setBrush(QBrush(Qt::black));
    dataPoints->setMarkerSize(10);
        chart->addSeries(dataPoints);

    pmaxLine = new QLineSeries();
    pmaxLine->setName("pMax");
    pmaxLine->setPen(QPen(Qt::red));
        chart->addSeries(pmaxLine);

    plotExponentiatedSeries(0);

    chart->setAxisX(axisX, demandCurve);
    chart->setAxisY(axisY2, demandCurve);

    chart->setAxisX(axisX, dataPoints);
    chart->setAxisY(axisY2, dataPoints);

    chart->setAxisX(axisX, pmaxLine);
    chart->setAxisY(axisY2, pmaxLine);
}

void chartwindow::plotExponentiatedSeries(int index)
{
    mList = mDisplayData.at(index);

    demandCurve->clear();
    dataPoints->clear();

    if (mList[2].contains("---", Qt::CaseInsensitive))
    {
        chart->setTitle(QString("Participant #%1: Dropped").arg(QString::number(currentIndexShown + 1)));
        pmaxLine->clear();
        pmaxLine->setName("pMax");

        return;
    }

    chart->setTitle(QString("Participant #%1").arg(QString::number(currentIndexShown + 1)));

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

    axisX->setMax(highestPrice * 2.0);

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

    pmaxLine->clear();

    *pmaxLine << QPointF(derivedPmax, 0.001);

    if (showStandardized)
    {
        *pmaxLine << QPointF(derivedPmax, (exponentiatedQ0 * pow(10, (exponentiatedK * (exp(-exponentiatedAlpha * exponentiatedQ0 * derivedPmax) - 1)))) * scaling);
    }
    else
    {
        *pmaxLine << QPointF(derivedPmax, exponentiatedQ0 * pow(10, (exponentiatedK * (exp(-exponentiatedAlpha * exponentiatedQ0 * derivedPmax) - 1))));
    }

    pmaxLine->setName(QString("pMax: %1").arg(QString::number(derivedPmax)));

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
            *dataPoints << QPointF(param1, param2 * scaling);
        }
        else
        {
            *dataPoints << QPointF(param1, param2);
        }
    }

    for (double i = 0.001; i < highestPrice * 2; )
    {
        double projectedValue = exponentiatedQ0 * pow(10, (exponentiatedK * (exp(-exponentiatedAlpha * exponentiatedQ0 * i) - 1)));

        if (projectedValue >= 0.001)
        {
            if (showStandardized)
            {
                *demandCurve << QPointF(i, projectedValue * scaling);
            }
            else
            {
                *demandCurve << QPointF(i, projectedValue);
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

    if (showStandardized)
    {
        axisY2->setMax(101);
        axisY2->setMin(0);
    }
    else
    {
        axisY2->setMax(highestConsumption * 1.2);
        axisY2->setMin(0);
    }

    axisX->setMax(highestPrice * 2);
    axisX->setMin(0.001);
}

void chartwindow::plotResiduals(int index)
{
    mList = mDisplayData.at(index);

    errSeries.clear();
    errDataPoints.clear();

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

    errSeries << QPointF(0, 0);

    xValues = xValues.remove('[');
    xValues = xValues.remove(']');

    mSplitX = xValues.split(',');

    yValues = yValues.remove('[');
    yValues = yValues.remove(']');

    mSplitY = yValues.split(',');

    canConvertY = false;
    canConvertX = false;

    yValuesTempHolder.clear();

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
            errSeries << QPointF(j+1, 0);
            errDataPoints << QPointF(j+1, (tempY - tempFitY));

            yValuesTempHolder.append((tempY - tempFitY));
        }
    }

    if (mSplitX.length() > 0)
    {
        minList = std::abs(*std::min_element(yValuesTempHolder.begin(), yValuesTempHolder.end())) * 1.5;
        maxList = std::abs(*std::max_element(yValuesTempHolder.begin(), yValuesTempHolder.end())) * 1.5;

        axisYerror.setMin((maxList >= minList) ? -maxList : -minList);
        axisYerror.setMax((maxList >= minList) ? maxList : minList);
        axisYerror.setTickCount(9);
    }

    axisXerror.setMin(0);
    axisXerror.setMax(mSplitX.length());
    axisXerror.setTickCount(mSplitX.length() + 1);

    chartError.setTitle(QString("Participant #%1: Residual Plot").arg(QString::number(currentIndexShown + 1)));
}

void chartwindow::plotQQResiduals(int index)
{
    mList = mDisplayData.at(index);

    errSeriesQQ.clear();
    errDataPointsQQ.clear();

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

        errSeriesQQ << QPointF(zValue, expectedValue);
        errDataPointsQQ << QPointF(zValue, yValuesTempHolder[j]);
    }

    if (mSplitX.length() > 0)
    {
        minList = std::abs(*std::min_element(yValuesTempHolder.begin(), yValuesTempHolder.end())) * 1.5;
        maxList = std::abs(*std::max_element(yValuesTempHolder.begin(), yValuesTempHolder.end())) * 1.5;

        axisYerrorQQ.setMin((maxList >= minList) ? -maxList : -minList);
        axisYerrorQQ.setMax((maxList >= minList) ? maxList : minList);
        axisYerrorQQ.setTickCount(9);
    }

    axisXerrorQQ.setMin(-2);
    axisXerrorQQ.setMax(2);
    axisXerrorQQ.setTickCount(5);

    chartErrorQQ.setTitle(QString("Participant #%1: Probability Plot").arg(QString::number(currentIndexShown + 1)));
}

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
        chartView->grab().save(file_name, "PNG", 9);
    }
}

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
