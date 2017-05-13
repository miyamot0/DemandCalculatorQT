/**
   Copyright 2017 Shawn Gilroy

   This file is part of Discounting Model Selector, Qt port.

   Discounting Model Selector is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 3.

   Discounting Model Selector is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Discounting Model Selector.  If not, see http://www.gnu.org/licenses/.

   The Discounting Model Selector is a tool to assist researchers in behavior economics.

   Email: shawn(dot)gilroy(at)temple.edu

   The FitWorker class was based on earlier work by Fabien Pierre-Nicolas; its license is presented below

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

#include <QProcess>
#include <QDebug>

#include "fitworker.h"

FitWorker::FitWorker(QString mCommand, QStringList mCommandArgs)
{
    command = mCommand;
    commandParameterList = mCommandArgs;
}

void FitWorker::startWork()
{
    emit workStarted();
}

void FitWorker::working()
{
    bool rachlinNotation = false;

    QProcess process;
    QByteArray output;
    QByteArray error;

    QStringList mArgList;
    QString cmdParameterString;

    rachlinNotation = false;

    mArgList.clear();
    mArgList << command;
    mArgList << commandParameterList.at(0);

    cmdParameterString = QString(commandParameterList.at(0));

    process.start(mArgList.join(" "));
    process.waitForFinished(-1);

    output = process.readAllStandardOutput();
    error = process.readAllStandardError();

    qDebug() << "Output: " << output;
    qDebug() << "Error: " << error;

    QStringList resultsList;

    resultsList << commandParameterList.first();
    resultsList << output;

    emit workFinished(resultsList);
}
