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

#ifndef GRAPHICALOUTPUTDIALOG_H
#define GRAPHICALOUTPUTDIALOG_H

#include <QMenu>
#include <QMenuBar>
#include <QDialog>
#include <QSvgWidget>
#include <QFile>
#include <QTextStream>

namespace Ui {
class GraphicalOutputDialog;
}

class GraphicalOutputDialog : public QDialog
{
    Q_OBJECT

public:
    /** Widget for displaying received plots
     * @brief GraphicalOutputDialog
     * @param parent
     */
    explicit GraphicalOutputDialog(QWidget *parent = 0);

    /** Method for adding base64 strings to list
     * @brief appendBase64
     * @param chartData - Base data
     */
    void appendBase64(QString chartData);

    bool eventFilter(QObject *object, QEvent *e);

    QStringList mDisplayData;

    ~GraphicalOutputDialog();

private slots:
    /** UI behavior slot
     * @brief on_NextButton_clicked
     */
    void on_NextButton_clicked();

    /** UI behavior slot
     * @brief on_PreviousButton_clicked
     */
    void on_PreviousButton_clicked();

    /** UI update slot, with current plot
     * @brief displayPlot
     */
    void displayPlot();

    void contextMenuRequested(const QPoint& point);

    void saveSVGasPNG();

    void on_pushButton_clicked();

private:
    Ui::GraphicalOutputDialog *ui;

    QString chartString;

    QImage img;

    int currentIndexShown;

    QSvgWidget *mSVG;

    QMenu *dialogMenu;
    QAction *savePNG;
};

#endif // GRAPHICALOUTPUTDIALOG_H
