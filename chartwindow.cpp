#include <QtGui>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include "chartwindow.h"

chartwindow::chartwindow(QList<QStringList> stringList, QString mModel, QWidget *parent)
{
    mDisplayData = stringList;

    /*
    for (int i=0; i<mDisplayData.length(); i++)
    {
        mList = mDisplayData[i];

        if (mModel == "linear")
        {
            rawPrices = mList.at(16);
            rawValues = mList.at(17);
        }
        else if (mModel == "hs")
        {
            rawPrices = mList.at(18);
            rawValues = mList.at(19);
        }
        else if (mModel == "koff")
        {
            rawPrices = mList.at(19);
            rawValues = mList.at(20);
        }

        rawPrices = rawPrices.replace(QString("["), QString(""));
        rawPrices = rawPrices.replace(QString("]"), QString(""));

        rawValues = rawValues.replace(QString("["), QString(""));
        rawValues = rawValues.replace(QString("]"), QString(""));

        if (mModel == "linear")
        {
            //mDisplayData[i].replace(16, rawPrices);
            //mDisplayData[i].replace(17, rawValues);

            buildLinearPlot();
        }
        else if (mModel == "hs")
        {
            //mDisplayData[i].replace(18, rawPrices);
            //mDisplayData[i].replace(19, rawValues);
        }
        else if (mModel == "koff")
        {
            //mDisplayData[i].replace(19, rawPrices);
            //mDisplayData[i].replace(20, rawValues);
        }
    }
    */

    QVBoxLayout *windowLayout = new QVBoxLayout;

    currentIndexShown = 0;

    installEventFilter(this);

    chart = new QChart();

    QFont mTitle("Serif", 14, -1, false);
    chart->setTitleFont(mTitle);

    auto mLegend = chart->legend();

    QFont mLegendFont("Serif", 10, -1, false);
    mLegend->setFont(mLegendFont);
    mLegend->setAlignment(Qt::AlignBottom);

    axisX = new QLogValueAxis;
    axisX->setGridLineColor(Qt::transparent);
    axisX->setTitleText("Unit Price");
    axisX->setBase(10);
    axisX->setLabelsFont(mLegendFont);
    axisX->setLinePenColor(Qt::black);
    axisX->setLinePen(QPen(Qt::black));
    axisX->setMin(0.01);

    if (mModel == "linear")
    {
        setWindowTitle("Linear Demand Model Plots");

        axisY = new QLogValueAxis;
        axisY->setBase(10);
        axisY->setGridLineColor(Qt::transparent);
        axisY->setTitleText("Overall Consumption");
        axisY->setLabelsFont(mLegendFont);
        axisY->setLinePenColor(Qt::black);
        axisY->setLinePen(QPen(Qt::black));
        axisY->setMin(0.01);

        buildLinearPlot();
    }
    else if (mModel == "hs")
    {
        setWindowTitle("Exponential Demand Model Plots");

        axisY = new QLogValueAxis;
        axisY->setBase(10);
        axisY->setGridLineColor(Qt::transparent);
        axisY->setTitleText("Overall Consumption");
        axisY->setLabelsFont(mLegendFont);
        axisY->setLinePenColor(Qt::black);
        axisY->setLinePen(QPen(Qt::black));
        axisY->setMin(0.01);

        buildExponentialPlot();
    }
    else if (mModel == "koff")
    {

    }

    // build plot

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    windowLayout->addWidget(chartView);

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

    plotLinearSeries(0);
}

void chartwindow::plotLinearSeries(int index)
{
    mList = mDisplayData.at(index);

    demandCurve->clear();
    dataPoints->clear();

    if (mList[0].contains("dropped", Qt::CaseInsensitive))
    {
        chart->setTitle(QString("Participant #%1: Dropped").arg(QString::number(currentIndexShown + 1)));

        return;
    }

    rawPrices = mList.at(16);
    rawPrices = rawPrices.replace(QString("["), QString(""));
    rawPrices = rawPrices.replace(QString("]"), QString(""));

    QList<QString> rawPricesSplit = rawPrices.split(",");

    rawValues = mList.at(17);
    rawValues = rawValues.replace(QString("["), QString(""));
    rawValues = rawValues.replace(QString("]"), QString(""));

    QList<QString> rawValuesSplit = rawValues.split(",");

    chart->setTitle(QString("Participant #%1").arg(QString::number(currentIndexShown + 1)));

    bool checkValue;

    linearL = mList[6].toDouble(&checkValue);

    if (!checkValue) return;

    linearb = mList[7].toDouble(&checkValue);

    if (!checkValue) return;

    lineara = mList[8].toDouble(&checkValue);

    if (!checkValue) return;

    double highestPrice = rawPricesSplit[rawPricesSplit.count() - 1].toDouble();

    axisX->setMax(highestPrice * 2.0);

    for (int i = 0; i < rawPricesSplit.length(); i++)
    {
        param1 = rawPricesSplit[i].toDouble(&checkValue1);
        param2 = rawValuesSplit[i].toDouble(&checkValue2);

        if (!checkValue1 || !checkValue2)
        {
            break;
        }

        *dataPoints << QPointF(param1, param2);
    }

    for (double i = 0.01; i < highestPrice * 2; i = i + 0.1)
    {
        double projectedValue = log(linearL) + (linearb * log(i)) - lineara * (i);

        if (projectedValue > 0.01)
        {
            *demandCurve << QPointF(i, projectedValue);
        }
    }
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

    plotExponentialSeries(0);
}

void chartwindow::plotExponentialSeries(int index)
{
    mList = mDisplayData.at(index);

    demandCurve->clear();
    dataPoints->clear();

    if (mList[0].contains("dropped", Qt::CaseInsensitive))
    {
        chart->setTitle(QString("Participant #%1: Dropped").arg(QString::number(currentIndexShown + 1)));

        return;
    }

    rawPrices = mList.at(18);
    rawPrices = rawPrices.replace(QString("["), QString(""));
    rawPrices = rawPrices.replace(QString("]"), QString(""));

    QList<QString> rawPricesSplit = rawPrices.split(",");

    rawValues = mList.at(19);
    rawValues = rawValues.replace(QString("["), QString(""));
    rawValues = rawValues.replace(QString("]"), QString(""));

    QList<QString> rawValuesSplit = rawValues.split(",");

    chart->setTitle(QString("Participant #%1").arg(QString::number(currentIndexShown + 1)));

    bool checkValue;

    exponentialAlpha = mList[2].toDouble(&checkValue);

    if (!checkValue) return;

    exponentialQ0 = mList[3].toDouble(&checkValue);

    if (!checkValue) return;

    exponentialK = mList[7].toDouble(&checkValue);

    if (!checkValue) return;

    double highestPrice = rawPricesSplit[rawPricesSplit.count() - 1].toDouble();

    axisX->setMax(highestPrice * 2.0);

    for (int i = 0; i < rawPricesSplit.length(); i++)
    {
        param1 = rawPricesSplit[i].toDouble(&checkValue1);
        param2 = rawValuesSplit[i].toDouble(&checkValue2);

        if (!checkValue1 || !checkValue2)
        {
            break;
        }

        *dataPoints << QPointF(param1, param2);
    }

    for (double i = 0.01; i < highestPrice * 2; i = i + 0.1)
    {
        //(log(q0)/log(10)) + k * (exp(-alpha * q0 * x) - 1)
        double projectedValue = log10(exponentialQ0) + exponentialK * (exp(-exponentialAlpha * exponentialQ0 * i) - 1);

        if (projectedValue > 0.01)
        {
            *demandCurve << QPointF(i, projectedValue);
        }
    }
}

bool chartwindow::eventFilter(QObject *object, QEvent *e)
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

    //if (isAUCFigure)
    //{
        //plotAUCSeries(currentIndexShown);
    //}
    //else
    //{
        //plotED50Series(currentIndexShown);
    //}
}

void chartwindow::on_PreviousButton_clicked()
{
    if (currentIndexShown <= 0)
    {
        return;
    }

    currentIndexShown--;

    //if (isAUCFigure)
    //{
        //plotAUCSeries(currentIndexShown);
    //}
    //else
    //{
        //plotED50Series(currentIndexShown);
    //}
}
