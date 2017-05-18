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

#include <QApplication>
#include <QDir>

#include "sheetwidget.h"

#ifdef _WIN32
    #include <QSettings>
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QString commandString;
    bool isRActive = false;
    bool isSVGsupported = false;

    #ifdef _WIN32

    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\R-Core\\R", QSettings::NativeFormat);

    if (settings.contains("InstallPath"))
    {
        QString mKey = "\"" + settings.value("InstallPath").toString() + "\\bin\\Rscript.exe\"";
        mKey.replace("\\", "/");
        commandString = mKey;

        isRActive = true;
    }

    isSVGsupported = true;

    #elif TARGET_OS_MAC

    if(QFile::exists("/usr/local/bin/Rscript"))
    {
        isRActive = true;

        commandString = "/usr/local/bin/Rscript";
    }
    else if(QFile::exists("/usr/bin/Rscript"))
    {
        isRActive = true;

        commandString = "/usr/bin/Rscript";
    }

    QDir myDir("/Applications");
    QList<QFileInfo> mFiles = myDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);

    foreach (QFileInfo fileInfo, mFiles) {

        if (fileInfo.fileName().contains("xquartz", Qt::CaseInsensitive))
        {
            isSVGsupported = true;
        }
    }

    myDir = QString("/Applications/Utilities");
    mFiles = myDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);

    foreach (QFileInfo fileInfo, mFiles) {

        if (fileInfo.fileName().contains("xquartz", Qt::CaseInsensitive))
        {
            isSVGsupported = true;
        }
    }

    #endif

    SheetWidget mNewSheet(isRActive, isSVGsupported, commandString);
    mNewSheet.setWindowIcon(QPixmap(":/images/applications-other.png"));
    mNewSheet.show();

    return app.exec();
}
