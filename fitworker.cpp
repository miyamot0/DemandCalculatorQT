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

#include <QtWidgets>
#include <QProcess>

#include "fitworker.h"

FitWorker::FitWorker(QString mCommand, QStringList mCommandArgs, bool boundRachlin, bool isLogNormal)
{
    command = mCommand;
    commandParameterList = mCommandArgs;
    boundRachlinS = boundRachlin;
    transformNormal = isLogNormal;
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

    QJsonParseError err;
    QJsonDocument jsonDoc;
    QJsonArray jsonArr;
    QJsonValue jsonVal;
    QJsonObject jsonObj;
    QString cmdParameterString;

    for (int i=0; i<commandParameterList.count(); i++)
    {
        rachlinNotation = false;

        mArgList.clear();
        mArgList << command;
        mArgList << commandParameterList.at(i);

        cmdParameterString = QString(commandParameterList.at(i));

        process.start(mArgList.join(" "));
        process.waitForFinished(-1);

        output = process.readAllStandardOutput();
        error = process.readAllStandardError();

        QString mOutput(output);
        jsonDoc = QJsonDocument::fromJson(mOutput.toUtf8(), &err);
        jsonArr = jsonDoc.array();
        jsonVal = jsonArr.at(0);
        jsonObj = jsonVal.toObject();

        if (boundRachlinS && jsonObj["Rachlin.s"].toDouble() >= 1.0)
        {
            rachlinNotation = true;
            QString mListString1 = commandParameterList.at(i);
            QStringList mList1   = mListString1.split(" ");
            QString scriptString = mList1.at(0);

            QString delayString = mList1.at(1);
            QString valueString = mList1.at(2);
            QString mListString2 = mList1.at(3);
            QStringList mList2   = mListString2.split(",");
            mList2[3] = "0";

            mArgList.clear();
            mArgList << command;
            mArgList << scriptString;
            mArgList << delayString;
            mArgList << valueString;
            mArgList << mList2.join(",");

            process.start(mArgList.join(" "));
            process.waitForFinished();

            output = process.readAllStandardOutput();
            error = process.readAllStandardError();

            QString mOutput(output);
            jsonDoc = QJsonDocument::fromJson(mOutput.toUtf8(), &err);
            jsonArr = jsonDoc.array();
            jsonVal = jsonArr.at(0);
            jsonObj = jsonVal.toObject();
        }

        QStringList resultsList;

        resultsList << QString::number(i+1);
        resultsList << formatStringResult(jsonObj["Mazur.lnk"].toDouble(), transformNormal);
        resultsList << "";
        resultsList << formatStringResult(jsonObj["Mazur.RMSE"].toDouble(), false);
        resultsList << formatStringResult(jsonObj["Mazur.BIC"].toDouble(), false);
        resultsList << formatStringResult(jsonObj["Mazur.AIC"].toDouble(), false);
        resultsList << formatStringResult(jsonObj["Mazur.BF"].toDouble(), false);
        resultsList << formatStringResult(jsonObj["Mazur.prob"].toDouble(), false);

        resultsList << formatStringResult(jsonObj["exp.lnk"].toDouble(), transformNormal);
        resultsList << "";
        resultsList << formatStringResult(jsonObj["exp.RMSE"].toDouble(), false);
        resultsList << formatStringResult(jsonObj["exp.BIC"].toDouble(), false);
        resultsList << formatStringResult(jsonObj["exp.AIC"].toDouble(), false);
        resultsList << formatStringResult(jsonObj["exp.BF"].toDouble(), false);
        resultsList << formatStringResult(jsonObj["exp.prob"].toDouble(), false);

        resultsList << formatStringResult(jsonObj["BD.beta"].toDouble(), false);
        resultsList << formatStringResult(jsonObj["BD.delta"].toDouble(), false);
        resultsList << formatStringResult(jsonObj["BD.RMSE"].toDouble(), false);
        resultsList << formatStringResult(jsonObj["BD.BIC"].toDouble(), false);
        resultsList << formatStringResult(jsonObj["BD.AIC"].toDouble(), false);
        resultsList << formatStringResult(jsonObj["BD.BF"].toDouble(), false);
        resultsList << formatStringResult(jsonObj["BD.prob"].toDouble(), false);

        resultsList << formatStringResult(jsonObj["MG.lnk"].toDouble(), transformNormal);
        resultsList << formatStringResult(jsonObj["MG.s"].toDouble(), false);
        resultsList << formatStringResult(jsonObj["MG.RMSE"].toDouble(), false);
        resultsList << formatStringResult(jsonObj["MG.BIC"].toDouble(), false);
        resultsList << formatStringResult(jsonObj["MG.AIC"].toDouble(), false);
        resultsList << formatStringResult(jsonObj["MG.BF"].toDouble(), false);
        resultsList << formatStringResult(jsonObj["MG.prob"].toDouble(), false);

        if (rachlinNotation)
        {
            resultsList << "Exceeded Bounds";
            resultsList << "Exceeded Bounds";
            resultsList << "Exceeded Bounds";
            resultsList << "Exceeded Bounds";
            resultsList << "Exceeded Bounds";
            resultsList << "Exceeded Bounds";
            resultsList << "Exceeded Bounds";
        }
        else
        {
            resultsList << formatStringResult(jsonObj["Rachlin.lnk"].toDouble(), transformNormal);
            resultsList << formatStringResult(jsonObj["Rachlin.s"].toDouble(), false);
            resultsList << formatStringResult(jsonObj["Rachlin.RMSE"].toDouble(), false);
            resultsList << formatStringResult(jsonObj["Rachlin.BIC"].toDouble(), false);
            resultsList << formatStringResult(jsonObj["Rachlin.AIC"].toDouble(), false);
            resultsList << formatStringResult(jsonObj["Rachlin.BF"].toDouble(), false);
            resultsList << formatStringResult(jsonObj["Rachlin.prob"].toDouble(), false);
        }

        resultsList << QString::number(jsonObj["noise.mean"].toDouble());
        resultsList << "";
        resultsList << QString::number(jsonObj["noise.RMSE"].toDouble());
        resultsList << QString::number(jsonObj["noise.BIC"].toDouble());
        resultsList << QString::number(jsonObj["noise.AIC"].toDouble());
        resultsList << QString::number(jsonObj["noise.BF"].toDouble());
        resultsList << QString::number(jsonObj["noise.prob"].toDouble());

        resultsList << jsonObj["prob.frame.probmodel"].toString();

        if (cmdParameterString.contains("DiscountingAreaComputation.R", Qt::CaseInsensitive))
        {
            resultsList << QString::number(jsonObj["CurveArea"].toDouble());
        }
        else
        {
            resultsList << QString::number(jsonObj["lnED50.mostprob"].toDouble());
        }

        resultsList << jsonObj["chart"].toString();

        emit workingResult(resultsList);
    }

    emit workFinished();
}

QString FitWorker::formatStringResult(double value, bool returnLogNormal)
{
    if (!isnan(value))
    {
        if (value == 0)
        {
            return QString("NA");
        }
        else if (returnLogNormal)
        {
            qreal res = qExp(value);
            return QString::number(res);
        }
        else
        {
            return QString::number(value);
        }
    }
    else
    {
        return QString("NA");
    }
}
