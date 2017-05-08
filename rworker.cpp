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

#include <QThread>
#include <QProcess>

#include "rworker.h"

Rworker::Rworker(QString mCommand, QStringList mCommandArgs, QObject *parent) : QObject(parent)
{
    command = mCommand;
    commandParameterList = mCommandArgs;
}

void Rworker::startWork()
{
    emit workStarted();
}

void Rworker::working()
{
    QProcess process;
    QByteArray output;
    QByteArray error;

    QStringList mArgList;

    for (int i=0; i<commandParameterList.count(); i++)
    {
        mArgList.clear();
        mArgList << command;
        mArgList << commandParameterList.at(i);

        process.start(mArgList.join(" "));
        process.waitForFinished(-1);

        output = process.readAllStandardOutput();
        error = process.readAllStandardError();

        emit workingResult(output);
    }

    emit workFinished();
}
