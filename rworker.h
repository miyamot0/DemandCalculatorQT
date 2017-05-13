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

   The RWorker class was based on earlier work by Fabien Pierre-Nicolas; its license is presented below

   =======================================================================================================

   Copyright 2013 Fabien Pierre-Nicolas.
      - Primarily authored by Fabien Pierre-Nicolas

   This file is part of simple-qt-thread-example, a simple example to demonstrate how to use threads.

   This example is explained on http://fabienpn.wordpress.com/qt-thread-simple-and-stable-with-sources/

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This progra is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

  */

#ifndef RWORKER_H
#define RWORKER_H

#include <QObject>
#include <QProcess>

class Rworker : public QObject
{
    Q_OBJECT

public:
    /** Constructor, explicit to avoid parent ownership issues
     * @brief Rworker
     * @param mCommand - Path/command to Rscript
     * @param mCommandArgs - Arguments to pass to Rscript
     * @param parent
     */
    explicit Rworker(QString mCommand, QStringList mCommandArgs, QObject *parent = 0);

private:
    QString command;
    QStringList commandParameterList;

signals:
    /** Begin work signal
     * @brief workStarted
     */
    void workStarted();

    /** Work update signal, series by series
     * @brief workingResult
     * @param value - List of values received
     */
    void workingResult(const QString &value);

    /** Signal work completed
     * @brief workFinished
     */
    void workFinished();

public slots:
    /** Slot, begin job
     * @brief startWork
     */
    void startWork();

    /** Slot, begin heavy lifting
     * @brief working
     */
    void working();

};

#endif // RWORKER_H
