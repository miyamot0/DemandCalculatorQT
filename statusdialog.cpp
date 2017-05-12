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

  */

#include <QProcess>
#include <QDesktopWidget>
#include <QDir>
#include "statusdialog.h"
#include "ui_statusdialog.h"

StatusDialog::StatusDialog(bool rInstalled, bool isSVGsupported, QString commandParameter, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatusDialog)
{
    ui->setupUi(this);

    this->window()->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            window()->size(),
            qApp->desktop()->availableGeometry()
        )
    );

    orderVar = 0;

    if (!rInstalled)
    {
        ui->statusR->setText("R was not found!");
    }
    else
    {
        ui->statusR->setText("R found!");
        ui->statusR->setStyleSheet("QLabel { color : green; }");
    }

    if (!isSVGsupported)
    {
        ui->statusSVG->setText("Install xQuartz to continue!");
        ui->statusSVG->setStyleSheet("QLabel { color : red; }");
    }
    else
    {
        ui->statusSVG->setText("SVG methods found!");
        ui->statusSVG->setStyleSheet("QLabel { color : green; }");
    }

    QStringList mPackageInstall;

    #ifdef _WIN32

    mPackageInstall << "installDependencyDevtools.R";
    mPackageInstall << "installDependencyDigest.R";
    mPackageInstall << "installDependencyBeezdemand.R";
    mPackageInstall << "installDependencyReshape.R";
    mPackageInstall << "installDependencyBase64.R";
    mPackageInstall << "installDependencyJsonlite.R";

    #endif

    #ifdef TARGET_OS_MAC

    QDir runDirectory = QDir(QCoreApplication::applicationDirPath());
    runDirectory.cdUp();
    runDirectory.cd("Resources");

    QString scriptDir = "\"" + runDirectory.path() + "/";

    mPackageInstall << scriptDir + "installDependencyDevtools.R\"";
    mPackageInstall << scriptDir + "installDependencyDigest.R\"";
    mPackageInstall << scriptDir + "installDependencyBeezdemand.R\"";
    mPackageInstall << scriptDir + "installDependencyReshape.R\"";
    mPackageInstall << scriptDir + "installDependencyBase64.R\"";
    mPackageInstall << scriptDir + "installDependencyJsonlite.R\"";

    #endif

    thread = new QThread();
    rWorker = new Rworker(commandParameter, mPackageInstall);

    rWorker->moveToThread(thread);

    connect(rWorker, SIGNAL(workStarted()), thread, SLOT(start()));
    connect(thread, SIGNAL(started()), rWorker, SLOT(working()));
    connect(rWorker, SIGNAL(workingResult(QString)), this, SLOT(WorkUpdate(QString)));
    connect(rWorker, SIGNAL(workFinished()), this, SLOT(DiagnosticsComplete()));
    connect(rWorker, SIGNAL(workFinished()), thread, SLOT(quit()), Qt::DirectConnection);

    thread->wait();
    rWorker->startWork();
}

void StatusDialog::WorkUpdate(QString status)
{
    bool isTrue = status.contains("TRUE");

    if (orderVar == 0)
    {
        if (!isTrue)
        {
            ui->statusDevtools->setText("devtools could not be installed!");
        }
        else
        {
            ui->statusDevtools->setText("devtools installed and ready!");
            ui->statusDevtools->setStyleSheet("QLabel { color : green; }");
        }
    }
    else if (orderVar == 1)
    {
        if (!isTrue)
        {
            ui->statusDigest->setText("digest could not be installed!");
        }
        else
        {
            ui->statusDigest->setText("digest installed and ready!");
            ui->statusDigest->setStyleSheet("QLabel { color : green; }");
        }
    }
    else if (orderVar == 2)
    {
        if (!isTrue)
        {
            ui->statusBeezdemand->setText("beezdemand could not be installed!");
        }
        else
        {
            ui->statusBeezdemand->setText("beezdemand installed and ready!");
            ui->statusBeezdemand->setStyleSheet("QLabel { color : green; }");
        }
    }
    else if (orderVar == 3)
    {
        if (!isTrue)
        {
            ui->statusReshape->setText("reshape could not be installed!");
        }
        else
        {
            ui->statusReshape->setText("reshape installed and ready!");
            ui->statusReshape->setStyleSheet("QLabel { color : green; }");
        }
    }
    else if (orderVar == 4)
    {
        if (!isTrue)
        {
            ui->statusBase64->setText("base64enc could not be installed!");
        }
        else
        {
            ui->statusBase64->setText("base64enc installed and ready!");
            ui->statusBase64->setStyleSheet("QLabel { color : green; }");
        }
    }
    else if (orderVar == 5)
    {
        if (!isTrue)
        {
            ui->statusJson->setText("jsonlite could not be installed!");
        }
        else
        {
            ui->statusJson->setText("jsonlite installed and ready!");
            ui->statusJson->setStyleSheet("QLabel { color : green; }");
        }
    }

    orderVar++;
}

void StatusDialog::DiagnosticsComplete()
{
    if (ui->statusDevtools->text().contains("not") ||
        ui->statusDigest->text().contains("not") ||
        ui->statusBeezdemand->text().contains("not") ||
        ui->statusR->text().contains("not") ||
        ui->statusReshape->text().contains("not") ||
        ui->statusBase64->text().contains("not") ||
        ui->statusJson->text().contains("not"))
    {
        ui->statusDiagnostics->setText("DMS is not yet functional!");
    }
    else
    {
        ui->statusDiagnostics->setText("DMS is installed and ready!");
        ui->statusDiagnostics->setStyleSheet("QLabel { color : green; }");
    }
}

StatusDialog::~StatusDialog()
{    
    thread->wait();

    delete thread;
    delete rWorker;

    delete ui;
}
