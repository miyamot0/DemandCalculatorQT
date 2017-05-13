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

#include <QtWidgets>
#include <QSvgRenderer>
#include <QPixmap>
#include "graphicaloutputdialog.h"
#include "ui_graphicaloutputdialog.h"

GraphicalOutputDialog::GraphicalOutputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GraphicalOutputDialog)
{
    ui->setupUi(this);

    mSVG = new QSvgWidget();

    QSizePolicy qsp(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mSVG->setSizePolicy(qsp);
    mSVG->resize(500, 500);

    ui->verticalLayout->insertWidget(0, mSVG, 0, Qt::AlignCenter);

    currentIndexShown = 0;

    setContextMenuPolicy(Qt::CustomContextMenu);

    dialogMenu = new QMenu(this);
    savePNG = dialogMenu->addAction("Save PNG");

    connect(this, SIGNAL(customContextMenuRequested(const QPoint)),this, SLOT(contextMenuRequested(const QPoint&)));
    connect(savePNG, SIGNAL(triggered()), this, SLOT(saveSVGasPNG()));

    installEventFilter(this);
}

void GraphicalOutputDialog::contextMenuRequested(const QPoint& point)
{
     dialogMenu->popup(mapToGlobal(point));
}

bool GraphicalOutputDialog::eventFilter(QObject *object, QEvent *e)
{
    if (e->type() == QEvent::KeyPress)
    {
        //QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
        //qDebug() << keyEvent;
        //QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
        //std::cout << event->key() << "\n";
    }
    return false;
}

void GraphicalOutputDialog::saveSVGasPNG()
{
    QString file_name;
    QString fileFilter = "PNG (*.png)";

#ifdef _WIN32

        file_name = QFileDialog::getSaveFileName(this, "Open spreadsheet file", QDir::homePath(),
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
        QSvgRenderer *renderer = mSVG->renderer();
        QImage image(600, 600, QImage::Format_RGB32);
        QPainter painter;
        painter.begin(&image);
        renderer->render(&painter);
        painter.end();
        image.save(file_name, "PNG", 9);
    }
}

GraphicalOutputDialog::~GraphicalOutputDialog()
{
    delete ui;
}

void GraphicalOutputDialog::on_NextButton_clicked()
{
    if (currentIndexShown >= mDisplayData.count() - 1)
    {
        return;
    }

    currentIndexShown++;

    displayPlot();
}

void GraphicalOutputDialog::on_PreviousButton_clicked()
{
    if (currentIndexShown <= 0)
    {
        return;
    }

    currentIndexShown--;

    displayPlot();
}

void GraphicalOutputDialog::appendBase64(QString chartData)
{
    QFile file("tempFileNew.tmp");
    file.open(QIODevice::WriteOnly);
    file.write(QByteArray::fromBase64(chartData.toUtf8()));
    file.close();

    QFile infile("tempFileNew.tmp");
    infile.open(QFile::ReadOnly);

    QFile outfile ("tempFileOut.tmp");
    outfile.open(QFile::WriteOnly | QFile::Truncate);

    QTextStream in(&infile);
    QTextStream out(&outfile);

    QRegExp rx1("<symbol");
    QRegExp rx2("</symbol");

    while (!in.atEnd()) {
        QString line = in.readLine();
        line.replace(rx1, "<g"); // so '<symbol' becomes '<g ...'
        line.replace(rx2, "</g");// and '</symbol becomes '</g'
        out << line << "\n";
    }

    infile.close();
    outfile.close();

    QFile readFile ("tempFileOut.tmp");
    readFile.open(QFile::ReadOnly);

    mDisplayData.append(readFile.readAll().toBase64());

    readFile.close();

    if (mDisplayData.count() == 1)
    {
        displayPlot();
    }

    ui->currentItem->setText("Showing #" + QString::number(currentIndexShown + 1) + " of " + QString::number(mDisplayData.count()) + " plots");
}

void GraphicalOutputDialog::displayPlot()
{
    chartString = mDisplayData.at(currentIndexShown);

    mSVG->load(QByteArray::fromBase64(chartString.toUtf8()));

    //mSVG->resize(mSVG->sizeHint()) ;

    ui->currentItem->setText("Showing #" + QString::number(currentIndexShown + 1) + " of " + QString::number(mDisplayData.count()) + " plots");
}

void GraphicalOutputDialog::on_pushButton_clicked()
{
    saveSVGasPNG();
}
