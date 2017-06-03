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

class chartwindow : public QMainWindow
{
public:
    explicit chartwindow(QList<QStringList> stringList, QString mModel, QWidget *parent = 0);

    bool eventFilter(QObject *object, QEvent *e);

    void buildLinearPlot();
    void plotLinearSeries(int index);

    void buildExponentialPlot();
    void plotExponentialSeries(int index);

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

    QLineSeries *demandCurve;
    QScatterSeries *dataPoints;

    double param1;
    double param2;

    bool checkValue1;
    bool checkValue2;

    // Linear

    double linearL;
    double linearb;
    double lineara;

    // Exponential

    double exponentialAlpha;
    double exponentialQ0;
    double exponentialK;

};

#endif // CHARTWINDOW_H
