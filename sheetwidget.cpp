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

   The SheetWidget class was inspired by the Qt 5.8 Spreadsheet example and Recent Files example, its license is below:

   =======================================================================================================

   Copyright (C) 2016 The Qt Company Ltd.
   Contact: https://www.qt.io/licensing/

   This file is part of the demonstration applications of the Qt Toolkit.

   Commercial License Usage
   Licensees holding valid commercial Qt licenses may use this file in
   accordance with the commercial license agreement provided with the
   Software or, alternatively, in accordance with the terms contained in
   a written agreement between you and The Qt Company. For licensing terms
   and conditions see https://www.qt.io/terms-conditions. For further
   information use the contact form at https://www.qt.io/contact-us.

   BSD License Usage
   Alternatively, you may use this file under the terms of the BSD license
   as follows:

   "Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in
       the documentation and/or other materials provided with the
       distribution.
     * Neither the name of The Qt Company Ltd nor the names of its
       contributors may be used to endorse or promote products derived
       from this software without specific prior written permission.


   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."

   ====================================================================================

   ALGLIB 3.11.0 (source code generated 2017-05-11)
   Copyright (c) Sergey Bochkanov (ALGLIB project).

   >>> SOURCE LICENSE >>>
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation (www.fsf.org); either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   A copy of the GNU General Public License is available at
   http://www.fsf.org/licensing/licenses
   >>> END OF LICENSE >>>

  */

#include <QtWidgets>
#include <QtXlsx>
#include <QTableWidgetItem>

#include "sheetwidget.h"
#include "resultsdialog.h"
#include "commanding.h"
#include "sheetdelegate.h"
#include "fittingdata.h"

QTXLSX_USE_NAMESPACE

struct QPairFirstComparer
{
    template<typename T1, typename T2>
    bool operator()(const QPair<T1,T2> &one, const QPair<T1,T2> &two) const
    {
        return one.first < two.first;
    }
};

SheetWidget::SheetWidget(QWidget *parent) : QMainWindow(parent)
{
    table = new QTableWidget(10000, 10000, this);
    table->setSizeAdjustPolicy(QTableWidget::AdjustToContents);

    undoStack = new QUndoStack(this);
    table->setItemDelegate(new SheetDelegate());

    #ifdef TARGET_OS_MAC
        table->setStyleSheet("QTableView {selection-background-color: #73E2A7; }");
    #endif

    QString value;

    for (int c = 0; c < 10000; ++c)
    {
        value = "";
        convertExcelColumn(value, c);
        table->setHorizontalHeaderItem(c, new QTableWidgetItem(value));
    }

    buildMenus();
    setCentralWidget(table);

    setWindowTitle("Demand Curve Analyzer v" + QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUILD));

    this->layout()->setSizeConstraint(QLayout::SetNoConstraint);

    resize(QDesktopWidget().availableGeometry(this).size() * 0.7);

    #ifdef _WIN32

    this->window()->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            window()->size(),
            qApp->desktop()->availableGeometry()
        )
    );

    #endif

    table->installEventFilter( this );

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadedFile(QNetworkReply*)) );

    #ifdef _WIN32
        manager->get(QNetworkRequest(QUrl("http://www.smallnstats.com/DemandCurveRepository/Updates.xml")));
    #elif TARGET_OS_MAC
        manager->get(QNetworkRequest(QUrl("http://www.smallnstats.com/DemandCurveRepositoryMac/Updates.xml")));
    #endif

    mObj = new demandmodeling();
}

void SheetWidget::downloadedFile(QNetworkReply *reply) {
    QByteArray data = reply->readAll();

    QDomDocument versionXML;

    if(!versionXML.setContent(data))
    {
        return;
    }

    QDomElement root = versionXML.documentElement();
    QDomElement mNode = root.namedItem("PackageUpdate").toElement();
    QDomElement mNode2 = mNode.namedItem("Version").toElement();

    QStringList mVersionList = mNode2.text().split('.');

    if (mVersionList.count() != 3)
    {
        return;
    }

    bool hasUpdate = false;

    QString mNetworkVersionString = QString("%1%2%3").arg(mVersionList[0]).arg(mVersionList[1]).arg(mVersionList[2]);

    QString mLocalVersionString = QString("%1%2%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUILD);

    if (mNetworkVersionString.toInt() > mLocalVersionString.toInt())
    {
        hasUpdate = true;
    }

    if (hasUpdate)
    {
        QMessageBox *msgBox = new QMessageBox;
        msgBox->setWindowTitle("Updates");
        msgBox->setText("There is an update available!");
        msgBox->setWindowModality(Qt::NonModal);
        msgBox->show();
    }
}

void SheetWidget::buildMenus()
{
    /** File actions
     * @brief
     */

    newSheetAction = new QAction("N&ew", this);
    newSheetAction->setShortcut(QKeySequence::New);
    newSheetAction->setIcon(QIcon(":/images/document-new.png"));
    connect(newSheetAction, &QAction::triggered, this, &SheetWidget::clearSheet);

    openSheetAction = new QAction("I&mport", this);
    openSheetAction->setShortcut(QKeySequence::Open);
    openSheetAction->setIcon(QIcon(":/images/document-open.png"));
    connect(openSheetAction, &QAction::triggered, this, &SheetWidget::showOpenFileDialog);

    saveSheetAction = new QAction("S&ave", this);
    saveSheetAction->setShortcut(QKeySequence::Save);
    saveSheetAction->setIcon(QIcon(":/images/document-save.png"));
    connect(saveSheetAction, &QAction::triggered, this, &SheetWidget::showSaveFileDialog);

    saveAsSheetAction = new QAction("S&ave As", this);
    saveAsSheetAction->setShortcut(QKeySequence::SaveAs);
    saveAsSheetAction->setIcon(QIcon(":/images/document-save-as.png"));
    connect(saveAsSheetAction, &QAction::triggered, this, &SheetWidget::showSaveAsFileDialog);

    updateProgramAction = new QAction("C&heck Updates", this);
    updateProgramAction->setIcon(QIcon(":/images/view-refresh.png"));
    connect(updateProgramAction, &QAction::triggered, this, &SheetWidget::checkUpdatesAction);

    exitSheetAction = new QAction("E&xit", this);
    exitSheetAction->setShortcut(QKeySequence::Quit);
    exitSheetAction->setIcon(QIcon(":/images/system-log-out.png"));
    connect(exitSheetAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    /** Window actions
     * @brief
     */

    openDemandWindow = new QAction("D&emand Curve Analysis", this);
    openDemandWindow->setIcon(QIcon(":/images/applications-system.png"));
    connect(openDemandWindow, &QAction::triggered, this, &SheetWidget::showDemandWindow);

    /** Edit actions
     * @brief
     */

    cutAction = new QAction("Cut", this);
    cutAction->setShortcut(QKeySequence::Cut);
    cutAction->setIcon(QIcon(":/images/edit-cut.png"));
    connect(cutAction, &QAction::triggered, this, &SheetWidget::cut);

    copyAction = new QAction("Copy", this);
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setIcon(QIcon(":/images/edit-copy.png"));
    connect(copyAction, &QAction::triggered, this, &SheetWidget::copy);

    pasteAction = new QAction("Paste", this);
    pasteAction->setShortcut(QKeySequence::Paste);
    pasteAction->setIcon(QIcon(":/images/edit-paste.png"));
    connect(pasteAction, &QAction::triggered, this, &SheetWidget::paste);

    pasteInvertedAction = new QAction("Paste Transposed", this);
    pasteInvertedAction->setIcon(QIcon(":/images/edit-paste.png"));
    connect(pasteInvertedAction, &QAction::triggered, this, &SheetWidget::pasteInverted);

    clearAction = new QAction("Clear", this);
    clearAction->setShortcut(Qt::Key_Delete);
    clearAction->setIcon(QIcon(":/images/edit-clear.png"));
    connect(clearAction, &QAction::triggered, this, &SheetWidget::clear);

    undoAction = undoStack->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setIcon(QIcon(":/images/edit-undo.png"));

    redoAction = undoStack->createRedoAction(this, tr("&Redo"));
    redoAction->setShortcut(QKeySequence::Redo);
    redoAction->setIcon(QIcon(":/images/edit-redo.png"));

    table->addAction(undoAction);
    table->addAction(redoAction);

    /** Window actions
     * @brief
     */

    demandWindowDialog = new DemandSettingsDialog(this);

    openLicenseDCA = new QAction("DCA License (GPL-V3)", this);
    openLicenseDCA->setIcon(QIcon(":/images/format-justify-center.png"));
    connect(openLicenseDCA, &QAction::triggered, this, &SheetWidget::showDCALicenseWindow);

    openLicenseBeezdemand = new QAction("Beezdemand License (GPL-V3)", this);
    openLicenseBeezdemand->setIcon(QIcon(":/images/format-justify-center.png"));
    connect(openLicenseBeezdemand, &QAction::triggered, this, &SheetWidget::showBeezdemandLicenseWindow);

    openLicenseALGLIB = new QAction("ALGLIB License (GPL-V3)", this);
    openLicenseALGLIB->setIcon(QIcon(":/images/format-justify-center.png"));
    connect(openLicenseALGLIB, &QAction::triggered, this, &SheetWidget::showALGLIBLicenseWindow);

    openLicenseQt = new QAction("Qt License (LGPL-V3, GPL-V3)", this);
    openLicenseQt->setIcon(QIcon(":/images/format-justify-center.png"));
    connect(openLicenseQt, &QAction::triggered, this, &SheetWidget::showQTLicenseWindow);

    openLicenseTango = new QAction("Tango Icons License (Public Domain)", this);
    openLicenseTango->setIcon(QIcon(":/images/format-justify-center.png"));
    connect(openLicenseTango, &QAction::triggered, this, &SheetWidget::showTangoLicenseWindow);

    openAbout = new QAction("Credits", this);
    openAbout->setIcon(QIcon(":/images/format-justify-center.png"));
    connect(openAbout, &QAction::triggered, this, &SheetWidget::showCreditsWindow);

    openFAQ = new QAction("FAQ", this);
    openFAQ->setIcon(QIcon(":/images/format-justify-center.png"));
    connect(openFAQ, &QAction::triggered, this, &SheetWidget::showFAQWindow);

    /** Window helper actions
     * @brief
     */

    priceAction = new QAction("Set Prices", this);
    priceAction->setIcon(QIcon(":/images/preferences-system.png"));
    connect(priceAction, &QAction::triggered, this, &SheetWidget::updatePriceModalWindow);

    consumptionAction = new QAction("Set Consumption", this);
    consumptionAction->setIcon(QIcon(":/images/preferences-system.png"));
    connect(consumptionAction, &QAction::triggered, this, &SheetWidget::updateConsumptionModalWindow);

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        recentFileActs[i]->setIcon(QIcon(":/images/format-justify-center.png"));
        connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }

    /** Menu area below
     * @brief
     */

    QMenu *sheetOptionsMenu = menuBar()->addMenu(tr("&File"));
    sheetOptionsMenu->addAction(newSheetAction);
    sheetOptionsMenu->addAction(openSheetAction);
    sheetOptionsMenu->addAction(saveSheetAction);
    sheetOptionsMenu->addAction(saveAsSheetAction);

    separatorAct = sheetOptionsMenu->addSeparator();

    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        sheetOptionsMenu->addAction(recentFileActs[i]);
    }

    sheetOptionsMenu->addSeparator();
    sheetOptionsMenu->addAction(updateProgramAction);
    sheetOptionsMenu->addSeparator();
    sheetOptionsMenu->addAction(exitSheetAction);

    updateRecentFileActions();

    QMenu *sheetEditMenu = menuBar()->addMenu(tr("&Edit"));
    sheetEditMenu->addAction(cutAction);
    sheetEditMenu->addAction(copyAction);
    sheetEditMenu->addAction(pasteAction);
    sheetEditMenu->addAction(pasteInvertedAction);
    sheetEditMenu->addSeparator();
    sheetEditMenu->addAction(redoAction);
    sheetEditMenu->addAction(undoAction);
    sheetEditMenu->addSeparator();
    sheetEditMenu->addAction(clearAction);

    QMenu *sheetCalculationsMenu = menuBar()->addMenu(tr("&Demand"));
    sheetCalculationsMenu->addAction(openDemandWindow);

    QMenu *sheetLicensesMenu = menuBar()->addMenu(tr("&Licenses"));
    sheetLicensesMenu->addAction(openLicenseDCA);
    sheetLicensesMenu->addAction(openLicenseBeezdemand);
    sheetLicensesMenu->addAction(openLicenseALGLIB);
    sheetLicensesMenu->addAction(openLicenseQt);
    sheetLicensesMenu->addAction(openLicenseTango);
    sheetLicensesMenu->addAction(openAbout);

    QMenu *sheetAboutMenu = menuBar()->addMenu(tr("&Help"));
    sheetAboutMenu->addAction(openFAQ);

    /** Context Menu
     * @brief
     */

    addAction(priceAction);
    addAction(consumptionAction);

    QAction *separatorTwo = new QAction(this);
    separatorTwo->setSeparator(true);
    addAction(separatorTwo);

    addAction(cutAction);
    addAction(copyAction);
    addAction(pasteAction);
    addAction(pasteInvertedAction);

    QAction *separatorThree = new QAction(this);
    separatorThree->setSeparator(true);
    addAction(separatorThree);

    addAction(clearAction);
    setContextMenuPolicy(Qt::ActionsContextMenu);
}

void SheetWidget::clearSheet()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    table->clearContents();

    curFile = "";
    setWindowFilePath(curFile);

    QApplication::restoreOverrideCursor();
}

void SheetWidget::checkUpdatesAction()
{
    QString mCommand = "";

#ifdef _WIN32
    mCommand = "maintenancetool.exe";
#elif TARGET_OS_MAC
    QDir mDir = QDir(QCoreApplication::applicationDirPath());
    mDir.cdUp();
    mDir.cdUp();
    mDir.cdUp();

    mCommand = QDir::cleanPath(mDir.path() + QDir::separator() + "maintenancetool.app");
#endif

    if (QFile::exists(mCommand))
    {
        QProcess p;
        QStringList args;
        args << "--updater";
        p.start(mCommand, args);
        p.waitForStarted();
        p.waitForFinished(-1);
    }
}

/** Window methods
 * @brief
 */

void SheetWidget::closeEvent(QCloseEvent* event)
{
    QMessageBox::StandardButton confirm = QMessageBox::question( this, "Demand Curve Analyzer",
                                                                tr("Are you sure you want to quit?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (confirm != QMessageBox::Yes) {
        event->ignore();
    } else {
        saveSettings();
        event->accept();
    }
}

void SheetWidget::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString mFile = action->data().toString();

        if (QFile::exists(mFile))
        {
            Document xlsx2(mFile);
            QStringList sheets = xlsx2.sheetNames();

            sheetSelectDialog = new SheetSelectDialog(this);
            sheetSelectDialog->UpdateUI(sheets);
            sheetSelectDialog->setModal(true);

            if(sheetSelectDialog->exec())
            {
                table->clearContents();

                xlsx2.selectSheet(sheetSelectDialog->GetSelected());

                for (int w = 0; w < xlsx2.dimension().lastColumn() + 1; w++)
                {
                    for (int h = 0; h < xlsx2.dimension().lastRow() + 1; h++)
                    {
                        if (QXlsx::Cell *cell = xlsx2.cellAt(h, w))
                        {
                            if (cell->cellType() == Cell::NumberType || cell->cellType() == Cell::StringType || cell->cellType() == Cell::SharedStringType)
                            {
                                table->setItem(h-1, w-1, new QTableWidgetItem(cell->value().toString()));
                            }
                        }
                    }
                }

                setCurrentFile(mFile);
                statusBar()->showMessage(tr("File loaded"), 2000);
            }
        }
    }
}

void SheetWidget::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setWindowFilePath(curFile);

    QSettings settings(QSettings::UserScope, QLatin1String("Demand Curve Analyzer"));
    settings.beginGroup(QLatin1String("SheetWindow"));

    QStringList files = settings.value(QLatin1String("recentFileList")).toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);

    while (files.size() > MaxRecentFiles)
    {
        files.removeLast();
    }

    settings.setValue("recentFileList", files);
    settings.endGroup();
    settings.sync();

    updateRecentFileActions();
}

void SheetWidget::saveSettings()
{
    QSettings settings(QSettings::UserScope, QLatin1String("Demand Curve Analyzer"));
    settings.beginGroup(QLatin1String("SheetWindow"));

    QStringList files = settings.value("recentFileList").toStringList();

    settings.setValue(QLatin1String("recentFileList"), files);
    settings.endGroup();

    settings.sync();
}

/** Window methods
 * @brief
 */

bool SheetWidget::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyCode = static_cast<QKeyEvent *>(event);

        if (keyCode->key() == (int) Qt::Key_Return)
        {
            if (table->currentRow() + 1 >= table->rowCount())
            {
                return QObject::eventFilter(object, event);
            }

            table->setCurrentCell(table->currentRow() + 1, table->currentColumn());
        }
    }

    return QObject::eventFilter(object, event);
}

void SheetWidget::showOpenFileDialog()
{
    QString file_name;
    QString fileFilter = "Spreadsheet (*.xlsx)";

    #ifdef _WIN32

    file_name = QFileDialog::getOpenFileName(this, "Open spreadsheet file", QDir::homePath(),
                                             fileFilter);

    #elif TARGET_OS_MAC

    file_name = QFileDialog::getOpenFileName(nullptr, "Open spreadsheet file", QDir::homePath(),
                                             fileFilter, nullptr, QFileDialog::Option::DontUseNativeDialog);

    #endif

    if(!file_name.trimmed().isEmpty())
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        Document xlsx2(file_name);
        QStringList sheets = xlsx2.sheetNames();

        sheetSelectDialog = new SheetSelectDialog(this);
        sheetSelectDialog->UpdateUI(sheets);
        sheetSelectDialog->setModal(true);

        if(sheetSelectDialog->exec())
        {
            table->clearContents();

            xlsx2.selectSheet(sheetSelectDialog->GetSelected());

            for (int w = 0; w < xlsx2.dimension().lastColumn() + 1; w++)
            {
                for (int h = 0; h < xlsx2.dimension().lastRow() + 1; h++)
                {
                    if (QXlsx::Cell *cell = xlsx2.cellAt(h, w))
                    {
                        if (cell->cellType() == Cell::NumberType || cell->cellType() == Cell::StringType || cell->cellType() == Cell::SharedStringType)
                        {
                            table->setItem(h-1, w-1, new QTableWidgetItem(cell->value().toString()));
                        }
                    }
                }
            }

            setCurrentFile(file_name);
            statusBar()->showMessage(tr("File loaded"), 2000);
        }

        QApplication::restoreOverrideCursor();
    }
}

void SheetWidget::updateRecentFileActions()
{
    QSettings settings(QSettings::UserScope, QLatin1String("Demand Curve Analyzer"));
    settings.beginGroup(QLatin1String("SheetWindow"));
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }

    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
    {
        recentFileActs[j]->setVisible(false);
    }

    separatorAct->setVisible(numRecentFiles > 0);
}

void SheetWidget::showSaveFileDialog()
{
    if (curFile == "")
    {
        showSaveAsFileDialog();

        return;
    }

    if(!curFile.trimmed().isEmpty())
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        QXlsx::Document xlsx;

        int rows = table->rowCount();
        int cols = table->columnCount();

        QString temp;

        for (int i=0; i<rows; i++)
        {
            for (int j=0; j<cols; j++)
            {
                QTableWidgetItem *item = table->item(i, j);

                if (item != NULL && !item->text().isEmpty())
                {
                    temp = table->item(i, j)->data(Qt::DisplayRole).toString();
                    xlsx.write(i + 1, j + 1, temp);
                }
            }
        }

        xlsx.saveAs(curFile);

        QApplication::restoreOverrideCursor();

        statusBar()->showMessage(tr("File saved"), 2000);
    }
}

void SheetWidget::showSaveAsFileDialog()
{

    QString file_name;
    QString fileFilter = "Spreadsheet (*.xlsx)";

#ifdef _WIN32

        file_name = QFileDialog::getSaveFileName(this, "Save spreadsheet file", QDir::homePath(),
                                         fileFilter);

#elif TARGET_OS_MAC

        file_name = QFileDialog::getSaveFileName(this, "Save spreadsheet file", QDir::homePath(),
                                         fileFilter, &fileFilter, QFileDialog::Option::DontUseNativeDialog);

        if (!file_name.contains(".xlsx"))
        {
            file_name.append(".xlsx");
        }

#endif

    if(!file_name.trimmed().isEmpty())
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        QXlsx::Document xlsx;

        int rows = table->rowCount();
        int cols = table->columnCount();

        QString temp;

        for (int i=0; i<rows; i++)
        {
            for (int j=0; j<cols; j++)
            {
                QTableWidgetItem *item = table->item(i, j);

                if (item != NULL && !item->text().isEmpty())
                {
                    temp = table->item(i, j)->data(Qt::DisplayRole).toString();
                    xlsx.write(i + 1, j + 1, temp);
                }
            }
        }

        xlsx.saveAs(file_name);
        setCurrentFile(file_name);

        QApplication::restoreOverrideCursor();

        statusBar()->showMessage(tr("File saved"), 2000);
    }
}

void SheetWidget::showDemandWindow()
{
    if (isToolWindowShown())
    {
        return;
    }

    demandWindowDialog = new DemandSettingsDialog(this);
    demandWindowDialog->setModal(false);
    demandWindowDialog->show();
}

void SheetWidget::showBeezdemandLicenseWindow()
{
    QString mFilePath = "";

    #ifdef _WIN32
            mFilePath = "License_Beezdemand.txt";
    #elif TARGET_OS_MAC
            QDir runDirectory = QDir(QCoreApplication::applicationDirPath());
            runDirectory.cdUp();
            runDirectory.cd("Resources");
            mFilePath = runDirectory.filePath("License_Beezdemand.txt");
    #endif

    licenseDialog = new LicenseDialog(mFilePath, this);
    licenseDialog->setWindowTitle("Beezdemand License (GPL-V3)");
    licenseDialog->setModal(true);
    licenseDialog->show();
}

void SheetWidget::showALGLIBLicenseWindow()
{
    QString mFilePath = "";

    #ifdef _WIN32
            mFilePath = "License_ALGLIB.txt";
    #elif TARGET_OS_MAC
            QDir runDirectory = QDir(QCoreApplication::applicationDirPath());
            runDirectory.cdUp();
            runDirectory.cd("Resources");
            mFilePath = runDirectory.filePath("License_ALGLIB.txt");
    #endif

    licenseDialog = new LicenseDialog(mFilePath, this);
    licenseDialog->setWindowTitle("ALGLIB License (GPL-V3)");
    licenseDialog->setModal(true);
    licenseDialog->show();
}

void SheetWidget::showDCALicenseWindow()
{
    QString mFilePath = "";

    #ifdef _WIN32
            mFilePath = "COPYING";
    #elif TARGET_OS_MAC
            QDir runDirectory = QDir(QCoreApplication::applicationDirPath());
            runDirectory.cdUp();
            runDirectory.cd("Resources");
            mFilePath = runDirectory.filePath("COPYING");
    #endif

    licenseDialog = new LicenseDialog(mFilePath, this);
    licenseDialog->setWindowTitle("DCA License (GPL-V3)");
    licenseDialog->setModal(true);
    licenseDialog->show();
}

void SheetWidget::showQTLicenseWindow()
{
    QString mFilePath = "";

    #ifdef _WIN32
            mFilePath = "License_Qt.txt";
    #elif TARGET_OS_MAC
            QDir runDirectory = QDir(QCoreApplication::applicationDirPath());
            runDirectory.cdUp();
            runDirectory.cd("Resources");
            mFilePath = runDirectory.filePath("License_Qt.txt");
    #endif

    licenseDialog = new LicenseDialog(mFilePath, this);
    licenseDialog->setWindowTitle("Qt License (LGPL-V3)");
    licenseDialog->setModal(true);
    licenseDialog->show();
}

void SheetWidget::showTangoLicenseWindow()
{
    QString mFilePath = "";

    #ifdef _WIN32
            mFilePath = "License_Tango.txt";
    #elif TARGET_OS_MAC
            QDir runDirectory = QDir(QCoreApplication::applicationDirPath());
            runDirectory.cdUp();
            runDirectory.cd("Resources");
            mFilePath = runDirectory.filePath("License_Tango.txt");
    #endif

    licenseDialog = new LicenseDialog(mFilePath, this);
    licenseDialog->setWindowTitle("Tango Icons License (Public Domain)");
    licenseDialog->setModal(true);
    licenseDialog->show();
}

void SheetWidget::showCreditsWindow()
{
    creditsDialog = new CreditsDialog();
    creditsDialog->setModal(true);
    creditsDialog->show();
}

void SheetWidget::showFAQWindow()
{
    aboutDialog = new AboutDialog();
    aboutDialog->setModal(true);
    aboutDialog->show();
}

/** Edit methods
 * @brief
 */

void SheetWidget::cut()
{
    SheetWidget::copy();
    SheetWidget::clear();
}

void SheetWidget::copy()
{
    QList<QTableWidgetSelectionRange> range = table->selectedRanges();
    QTableWidgetSelectionRange mRange = range.first();

    QString str;

    for (int i = 0; i < mRange.rowCount(); ++i) {
        if (i > 0)
        {
            str += "\n";
        }

        for (int j = 0; j < mRange.columnCount(); ++j) {
            if (j > 0)
            {
                str += "\t";
            }

            if (table->item(mRange.topRow() + i, mRange.leftColumn() + j) == NULL)
            {
                str += "";
            }
            else
            {
                str += table->item(mRange.topRow() + i, mRange.leftColumn() + j)->data(Qt::DisplayRole).toString();
            }
        }
    }

    QApplication::clipboard()->setText(str);
}

void SheetWidget::paste()
{
    QTableWidgetSelectionRange range = table->selectedRanges().first();
    QString pasteString = QApplication::clipboard()->text();

    QStringList pasteRows = pasteString.split('\n');

    int nRows = pasteRows.count();
    int nCols = pasteRows.first().count('\t') + 1;

    if (nRows < 1 || nCols < 1)
    {
        return;
    }
    else if (nRows == 1 && nCols == 1)
    {
        const QModelIndex index = table->model()->index(range.topRow(), range.leftColumn(), QModelIndex());
        QString mOldTest(table->model()->index(range.topRow(), range.leftColumn()).data(Qt::EditRole).toString());
        QString mNewTest(pasteRows[0].split('\t')[0]);

        undoStack->push(new UpdateCommand(&index, mOldTest, mNewTest));
    }
    else
    {
        QStringList mOlderHolder;
        QStringList mTemp;

        for (int i = 0; i < nRows; ++i)
        {
            QStringList columns = pasteRows[i].split('\t');

            mTemp.clear();

            for (int j = 0; j < nCols; ++j) {
                int row = range.topRow() + i;
                int column = range.leftColumn() + j;

                if (row < 10000 && column < 10000)
                {
                    if (table->item(row, column) != NULL)
                    {
                        if (j < columns.length())
                        {
                            mTemp << table->item(row, column)->data(Qt::EditRole).toString();
                        }
                    }
                    else
                    {
                        if (j < columns.length())
                        {
                            table->setItem(row, column, new QTableWidgetItem(""));

                            mTemp << "";
                        }
                    }
                }
            }

            mOlderHolder << mTemp.join('\t');
        }

        const QModelIndex index = table->model()->index(range.topRow(), range.leftColumn(), QModelIndex());

        undoStack->push(new UpdateCommandBlock(&index, mOlderHolder, pasteRows));
    }
}

void SheetWidget::pasteInverted()
{
    QTableWidgetSelectionRange range = table->selectedRanges().first();
    QString pasteString = QApplication::clipboard()->text();
    QStringList pasteRows = pasteString.split('\n');

    int nRows = pasteRows.count();
    int nCols = pasteRows.first().count('\t') + 1;

    if (nRows < 1 || nCols < 1)
    {
        return;
    }
    else if (nRows == 1 && nCols == 1)
    {
        const QModelIndex index = table->model()->index(range.topRow(), range.leftColumn(), QModelIndex());
        QString mOldTest(table->model()->index(range.topRow(), range.leftColumn()).data(Qt::EditRole).toString());
        QString mNewTest(pasteRows[0].split('\t')[0]);

        undoStack->push(new UpdateCommand(&index, mOldTest, mNewTest));
    }
    else
    {
        QStringList mOlderHolder;
        QStringList mTemp;

        for (int i = 0; i < nRows; ++i)
        {
            QStringList columns = pasteRows[i].split('\t');

            mTemp.clear();

            for (int j = 0; j < nCols; ++j)
            {
                int row = range.topRow() + j;
                int column = range.leftColumn() + i;

                if (row < 10000 && column < 10000)
                {
                    if (table->item(row, column) != NULL)
                    {
                        if (j < columns.length())
                        {
                            mTemp << table->item(row, column)->data(Qt::EditRole).toString();
                        }
                    }
                    else
                    {
                        if (j < columns.length())
                        {
                            table->setItem(row, column, new QTableWidgetItem(""));

                            mTemp << "";
                        }
                    }
                }
            }

            mOlderHolder << mTemp.join('\t');
        }

        const QModelIndex index = table->model()->index(range.topRow(), range.leftColumn(), QModelIndex());

        undoStack->push(new UpdateCommandBlockInvert(&index, mOlderHolder, pasteRows));
    }
}

void SheetWidget::clear()
{
    QTableWidgetSelectionRange range = table->selectedRanges().first();

    int nRows = range.rowCount();
    int nCols = range.columnCount();

    if (nRows < 1 || nCols < 1)
    {
        return;
    }
    else if (nRows == 1 && nCols == 1)
    {
        const QModelIndex index = table->model()->index(range.topRow(), range.leftColumn(), QModelIndex());
        QString mOldTest(table->model()->index(range.topRow(), range.leftColumn()).data(Qt::EditRole).toString());
        QString clear("");

        undoStack->push(new UpdateCommand(&index, mOldTest, clear));
    }
    else
    {
        QStringList mNewerHolder;
        QStringList mOlderHolder;

        QStringList mTempNew;
        QStringList mTemp;

        for (int i = 0; i < nRows; ++i)
        {
            mTemp.clear();
            mTempNew.clear();

            for (int j = 0; j < nCols; ++j)
            {
                int row = range.topRow() + i;
                int column = range.leftColumn() + j;

                if (row < 10000 && column < 10000)
                {
                    if (table->item(row, column) != NULL)
                    {
                        mTemp << table->item(row, column)->data(Qt::EditRole).toString();
                        mTempNew << "";
                    }
                    else
                    {
                        table->setItem(row, column, new QTableWidgetItem(""));

                        mTemp << "";
                        mTempNew << "";
                    }
                }
            }

            mOlderHolder << mTemp.join('\t');
            mNewerHolder << mTempNew.join('\t');
        }

        const QModelIndex index = table->model()->index(range.topRow(), range.leftColumn(), QModelIndex());
        undoStack->push(new UpdateCommandBlock(&index, mOlderHolder, mNewerHolder));
    }
}

/** Window helper methods
 * @brief
 */

void SheetWidget::updatePriceModalWindow()
{
    if (!isToolWindowShown())
    {
        return;
    }

    QList<QTableWidgetSelectionRange> mList = table->selectedRanges();
    QTableWidgetSelectionRange range = mList.first();

    QString mLeft = "";
    convertExcelColumn(mLeft, range.leftColumn());

    QString mRight = "";
    convertExcelColumn(mRight, range.rightColumn());

    mLeft.append(QString::number(range.topRow() + 1));
    mLeft.append(":");
    mLeft.append(mRight);
    mLeft.append(QString::number(range.bottomRow() + 1));

    int mWidth = range.rightColumn() - range.leftColumn();
    int mHeight = range.bottomRow() - range.topRow();

    if (mWidth > 0 && mHeight > 0)
    {
        QMessageBox::critical(this, "Error", "Please select a vector of delays (e.g., one column or one row).");
        return;
    }
    else if (mWidth + mHeight < 3)
    {
        QMessageBox::critical(this, "Error", "Please select at least 3 delay points. At least 3 points are needed to proceed with curve fitting.");
        return;
    }

    if (demandWindowDialog->isVisible())
    {
        demandWindowDialog->UpdatePrices(mLeft, range.topRow(), range.leftColumn(), range.bottomRow(), range.rightColumn());
    }
}

void SheetWidget::updateConsumptionModalWindow()
{
    if (!isToolWindowShown())
    {
        return;
    }

    QList<QTableWidgetSelectionRange> mList = table->selectedRanges();
    QTableWidgetSelectionRange range = mList.first();

    QString mLeft = "";
    convertExcelColumn(mLeft, range.leftColumn());

    QString mRight = "";
    convertExcelColumn(mRight, range.rightColumn());

    mLeft.append(QString::number(range.topRow() + 1));
    mLeft.append(":");
    mLeft.append(mRight);
    mLeft.append(QString::number(range.bottomRow() + 1));

    if (demandWindowDialog->isVisible())
    {
        demandWindowDialog->UpdateConsumption(mLeft, range.topRow(), range.leftColumn(), range.bottomRow(), range.rightColumn());
    }
}

bool SheetWidget::isToolWindowShown()
{
    if (demandWindowDialog->isVisible())
    {
        return true;
    }

    return false;
}

/** Scoring methods
 * @brief
 */

void SheetWidget::Calculate()
{
    if (demandWindowDialog->isVisible())
    {
        demandWindowDialog->ToggleButton(false);
    }

    // Display figures?

    bool isRowData = (calculationSettings->rightPrice - calculationSettings->leftPrice == 0) ? false :
                                                                                               true;
    int nSeries = (isRowData) ? calculationSettings->bottomConsumption - calculationSettings->topConsumption + 1 :
                                nSeries = calculationSettings->rightConsumption - calculationSettings->leftConsumption + 1;

    int dWidth = calculationSettings->rightPrice - calculationSettings->leftPrice + 1;
    int dLength = calculationSettings->bottomPrice - calculationSettings->topPrice + 1;

    int vWidth = calculationSettings->rightConsumption - calculationSettings->leftConsumption + 1;
    int vLength = calculationSettings->bottomConsumption - calculationSettings->topConsumption + 1;

    if (!areDimensionsValid(isRowData, dWidth, vWidth, dLength, vLength))
    {
        if (demandWindowDialog->isVisible())
        {
            demandWindowDialog->ToggleButton(true);
        }

        return;
    }

    QStringList pricePoints;

    if(!arePricePointsValid(pricePoints,
                            isRowData,
                            calculationSettings->topPrice,
                            calculationSettings->leftPrice,
                            calculationSettings->bottomPrice,
                            calculationSettings->rightPrice))
    {
        if (demandWindowDialog->isVisible())
        {
            demandWindowDialog->ToggleButton(true);
        }

        return;
    }

    QStringList valuePoints;
    QStringList pricePointsTemp;

    mSteinResults.clear();
    QStringList mTempSteinResults;

    bool raisedFlag = false;

    // Stein Checks, as needed
    if (calculationSettings->settingsCheck != SystematicCheck::Never)
    {
        steinCheckDialog = new SteinCheck();

        for (int i = 0; i < nSeries; i++)
        {
            areValuePointsValid(valuePoints,
                                pricePointsTemp,
                                pricePoints,
                                isRowData,
                                calculationSettings->topConsumption,
                                calculationSettings->leftConsumption,
                                calculationSettings->bottomConsumption,
                                calculationSettings->rightConsumption, i);

            mTempSteinResults.clear();
            mTempSteinResults = mObj->GetSteinTest(pricePointsTemp, valuePoints);
            mTempSteinResults[0] = QString::number(i + 1);

            if (mObj->raisedFlag)
            {
                raisedFlag = true;
            }

            mSteinResults.append(mTempSteinResults);
            steinCheckDialog->appendRow(mTempSteinResults);
        }

        if (calculationSettings->settingsCheck == SystematicCheck::Always || raisedFlag)
        {
            steinCheckDialog->exec();

            if (!steinCheckDialog->canProceed)
            {
                if (demandWindowDialog->isVisible())
                {
                    demandWindowDialog->ToggleButton(true);
                }

                return;
            }
        }
    }

    // Results dialog?

    statusBar()->showMessage("Beginning calculations...", 3000);
    allResults.clear();

    // Test for k settings (as needed)
    double globalMin,
           globalMax,
           globalFitK;

    if (calculationSettings->settingsK == BehaviorK::Range)
    {
        getGlobalMinAndMax(globalMin,
                           globalMax,
                           isRowData,
                           calculationSettings->topConsumption,
                           calculationSettings->leftConsumption,
                           calculationSettings->bottomConsumption,
                           calculationSettings->rightConsumption);
    }
    else if (calculationSettings->settingsK == BehaviorK::Share)
    {
        getGlobalMinAndMax(globalMin,
                           globalMax,
                           isRowData,
                           calculationSettings->topConsumption,
                           calculationSettings->leftConsumption,
                           calculationSettings->bottomConsumption,
                           calculationSettings->rightConsumption);

        getDataPointsGlobal(globalFitK,
                            globalMax,
                            isRowData,
                            calculationSettings->settingsModel,
                            calculationSettings->topPrice,
                            calculationSettings->leftPrice,
                            calculationSettings->bottomPrice,
                            calculationSettings->rightPrice,
                            calculationSettings->topConsumption,
                            calculationSettings->leftConsumption,
                            calculationSettings->bottomConsumption,
                            calculationSettings->rightConsumption);

    }

    calculationSettings->globalMinConsumption = globalMin;
    calculationSettings->globalMaxConsumption = globalMax;
    calculationSettings->globalFitK = globalFitK;

    // todo, results?

    statusBar()->showMessage("Beginning calculations...", 3000);
    allResults.clear();

    QList<QStringList> mStoredValues;
    QStringList mStoredValueHolder;

    QList<double> tempDataPrices;
    QList<double> tempDataConsumption;

    QList<FittingData> mStoredNewValues;

    bool xStart = false;
    bool yStart = false;

    double localMax = -std::numeric_limits<double>::max(),
           localMin = std::numeric_limits<double>::max();

    for (int i = 0; i < nSeries; i++)
    {
        xStart = false;
        yStart = false;

        localMax = -std::numeric_limits<double>::max(),
        localMin = std::numeric_limits<double>::max();

        valuePoints.clear();
        pricePointsTemp.clear();

        areValuePointsValid(valuePoints,
                            pricePointsTemp,
                            pricePoints,
                            isRowData,
                            calculationSettings->topConsumption,
                            calculationSettings->leftConsumption,
                            calculationSettings->bottomConsumption,
                            calculationSettings->rightConsumption, i);

        tempDataPrices.clear();
        tempDataConsumption.clear();

        mXString = "[";
        mYString = "[";

        // Return if doesn't match
        if (pricePointsTemp.length() != valuePoints.length())
        {
            return;
        }

        for (int i=0; i<pricePointsTemp.length() && i<valuePoints.length(); i++)
        {
            // Pass on zero consumptions?
            if ((calculationSettings->settingsZeroConsumption == Behavior::Drop && valuePoints[i].toDouble() <= 0) ||
                (calculationSettings->settingsModel == DemandModel::Exponential && valuePoints[i].toDouble() <= 0))
            {
                continue;
            }

            // Pass on Q0?
            if ((calculationSettings->settingsQ0 == Behavior::Drop && pricePointsTemp[i].toDouble() <= 0) ||
                (calculationSettings->settingsModel == DemandModel::Linear && valuePoints[i].toDouble() <= 0))
            {
                continue;
            }

            if (!xStart)
            {
                if (calculationSettings->settingsQ0 == Behavior::Modify && pricePointsTemp[i].toDouble() >= 0)
                {
                    mXString.append("[" + QString::number(calculationSettings->customQ0replacement) + "]");

                    tempDataPrices.append(calculationSettings->customQ0replacement);
                }
                else
                {
                    mXString.append("[" + pricePointsTemp[i] + "]");

                    tempDataPrices.append(pricePointsTemp[i].toDouble());
                }

                xStart = true;
            }
            else
            {
                if (calculationSettings->settingsQ0 == Behavior::Modify && pricePointsTemp[i].toDouble() >= 0)
                {
                    mXString.append(",[" + QString::number(calculationSettings->customQ0replacement) + "]");

                    tempDataPrices.append(calculationSettings->customQ0replacement);
                }
                else
                {
                    mXString.append(",[" + pricePointsTemp[i] + "]");

                    tempDataPrices.append(pricePointsTemp[i].toDouble());
                }
            }

            if (!yStart)
            {
                if (calculationSettings->settingsZeroConsumption == Behavior::Modify && valuePoints[i].toDouble() <= 0)
                {
                    mYString.append(QString::number(calculationSettings->customZeroConsumptionReplacement));

                    tempDataConsumption.append(calculationSettings->customZeroConsumptionReplacement);
                }
                else
                {
                    mYString.append(valuePoints[i]);

                    tempDataConsumption.append(valuePoints[i].toDouble());
                }

                yStart = true;
            }
            else
            {
                if (calculationSettings->settingsZeroConsumption == Behavior::Modify && valuePoints[i].toDouble() <= 0)
                {
                    mYString.append("," + QString::number(calculationSettings->customZeroConsumptionReplacement));

                    tempDataConsumption.append(calculationSettings->customZeroConsumptionReplacement);
                }
                else
                {
                    mYString.append("," + valuePoints[i]);

                    tempDataConsumption.append(valuePoints[i].toDouble());
                }
            }

            if (valuePoints[i].toDouble() > 0 && valuePoints[i].toDouble() > localMax)
            {
                localMax = valuePoints[i].toDouble();
            }

            if (valuePoints[i].toDouble() > 0 && valuePoints[i].toDouble() < localMin)
            {
                localMin = valuePoints[i].toDouble();
            }
        }

        mXString.append("]");
        mYString.append("]");

        mStoredValueHolder.clear();
        mStoredValueHolder << mXString << mYString << pricePointsTemp.join(",") << valuePoints.join(",");

        mStoredValues << mStoredValueHolder;

        mStoredNewValues.append(FittingData(mXString, mYString,
                                            tempDataPrices, tempDataConsumption,
                                            localMin, localMax));
    }

    workerThread = new QThread();

    worker = new CalculationWorker(mStoredNewValues, calculationSettings);

    worker->moveToThread(workerThread);

    connect(worker, SIGNAL(workStarted()), workerThread, SLOT(start()));
    connect(workerThread, SIGNAL(started()), worker, SLOT(working()));
    connect(worker, SIGNAL(workingResult(QStringList)), this, SLOT(WorkUpdate(QStringList)));
    connect(worker, SIGNAL(workFinished()), workerThread, SLOT(quit()), Qt::DirectConnection);
    connect(worker, SIGNAL(workFinished()), this, SLOT(WorkFinished()));

    resultsDialog = new ResultsDialog(this);
    allResults.clear();

    workerThread->wait();
    worker->startWork();
}

void SheetWidget::WorkUpdate(QStringList results)
{
    allResults.append(results);
    statusBar()->showMessage(QString("Series #%1 Computed").arg(allResults.count()), 3000);
}

void SheetWidget::WorkFinished()
{
    statusBar()->showMessage("Calculations Complete.", 3000);

    if (calculationSettings->settingsChart != ChartingOptions::None)
    {
        statusBar()->showMessage("Drawing figures...", 3000);

        if (demandWindowDialog->isVisible())
        {
            chartWindow = new chartwindow(allResults,
                                          calculationSettings->settingsChart == ChartingOptions::Standardized,
                                          calculationSettings->settingsModel,
                                          this);
        }

        chartWindow->show();
    }

    if (demandWindowDialog->isVisible())
    {
        demandWindowDialog->ToggleButton(true);
        demandWindowDialog->setEnabled(true);

        resultsDialog->setResultsType(calculationSettings->settingsModel);
        resultsDialog->setResults(allResults);
        resultsDialog->show();
    }
}

bool SheetWidget::arePricePointsValid(QStringList &pricePoints, bool isRowData, int topDelay, int leftDelay, int bottomDelay, int rightDelay)
{
    pricePoints.clear();

    QString holder;
    bool valueCheck = true;

    if (isRowData)
    {
        int r = topDelay;

        for (int c = leftDelay; c <= rightDelay; c++)
        {
            if (table->item(r, c) == NULL)
            {
                QMessageBox::critical(this, "Error",
                                      "One of your price measures doesn't look correct. Please re-check these values or selections.");

                if (demandWindowDialog->isVisible())
                {
                    demandWindowDialog->ToggleButton(true);
                }

                return false;
            }

            holder = table->item(r, c)->data(Qt::DisplayRole).toString();
            holder.toDouble(&valueCheck);

            pricePoints << holder;

            if (!valueCheck)
            {
                QMessageBox::critical(this, "Error",
                                      "One of your price measures doesn't look correct. Please re-check these values or selections.");

                if (demandWindowDialog->isVisible())
                {
                    demandWindowDialog->ToggleButton(true);
                }

                return false;
            }
        }
    }
    else
    {
        int c = leftDelay;

        for (int r = topDelay; r <= bottomDelay; r++)
        {
            if (table->item(r, c) == NULL)
            {
                QMessageBox::critical(this, "Error",
                                      "One of your price measures doesn't look correct. Please re-check these values or selections.");

                if (demandWindowDialog->isVisible())
                {
                    demandWindowDialog->ToggleButton(true);
                }

                return false;
            }

            holder = table->item(r, c)->data(Qt::DisplayRole).toString();
            holder.toDouble(&valueCheck);

            pricePoints << holder;

            if (!valueCheck)
            {
                QMessageBox::critical(this, "Error",
                                      "One of your price measures doesn't look correct. Please re-check these values or selections.");

                if (demandWindowDialog->isVisible())
                {
                    demandWindowDialog->ToggleButton(true);
                }

                return false;
            }
        }
    }

    return true;
}

bool SheetWidget::areDimensionsValid(bool isRowData, int dWidth, int vWidth, int dLength, int vLength)
{
    if (isRowData)
    {
        if (dWidth != vWidth)
        {
            QMessageBox::critical(this, "Error",
                                  "You have row-based data, but the data selected appears to have different column counts. Please correct.");

            if (demandWindowDialog->isVisible())
            {
                demandWindowDialog->ToggleButton(true);
            }

            return false;
        }
    }
    else
    {
        if (dLength != vLength)
        {
            QMessageBox::critical(this, "Error",
                                  "You have column-based data, but the data selected appears to have different row counts. Please correct.");

            if (demandWindowDialog->isVisible())
            {
                demandWindowDialog->ToggleButton(true);
            }

            return false;
        }
    }

    return true;
}

void SheetWidget::getGlobalMinAndMax(double &globalMin, double &globalMax, bool isRowData, int topValue, int leftValue, int bottomValue, int rightValue)
{
    QString holder;
    bool valueCheck = true;
    double valHolder;

    globalMin = maxrealnumber;
    globalMax = minrealnumber;

    if (isRowData)
    {
        for (int r = topValue; r <= bottomValue; r++)
        {
            for (int c = leftValue; c <= rightValue; c++)
            {
                if (table->item(r, c) != NULL)
                {
                    holder = table->item(r, c)->data(Qt::DisplayRole).toString();
                    valHolder = holder.toDouble(&valueCheck);

                    if (valueCheck)
                    {
                        if (valHolder > 0 && valHolder > globalMax)
                        {
                            globalMax = valHolder;
                        }

                        if (valHolder > 0 && valHolder < globalMin)
                        {
                            globalMin = valHolder;
                        }
                    }
                }
            }
        }
    }
    else
    {
        for (int c = leftValue; c <= rightValue; c++)
        {
            for (int r = topValue; r <= bottomValue; r++)
            {
                if (table->item(r, c) != NULL)
                {
                    holder = table->item(r, c)->data(Qt::DisplayRole).toString();
                    valHolder = holder.toDouble(&valueCheck);

                    if (valueCheck)
                    {
                        if (valHolder > 0 && valHolder > globalMax)
                        {
                            globalMax = valHolder;
                        }

                        if (valHolder > 0 && valHolder < globalMin)
                        {
                            globalMin = valHolder;
                        }
                    }
                }
            }
        }
    }
}

void SheetWidget::getDataPointsGlobal(double &returnK, double globalMax, bool isRowData, DemandModel mModel,
                                      int topPrice, int leftPrice, int, int,
                                      int topValue, int leftValue, int bottomValue, int rightValue)
{

    QList<QPair<double, double>> mDataPoints;

    QString holder1;
    QString holder2;

    bool valueCheck1 = true;
    double valHolder1;

    bool valueCheck2 = true;
    double valHolder2;

    if (isRowData)
    {
        for (int r = topValue; r <= bottomValue; r++)
        {
            for (int c = leftValue; c <= rightValue; c++)
            {
                if (table->item(r, c) != NULL && table->item(topPrice, c) != NULL)
                {
                    holder1 = table->item(r, c)->data(Qt::DisplayRole).toString();
                    valHolder1 = holder1.toDouble(&valueCheck1);

                    holder2 = table->item(topPrice, c)->data(Qt::DisplayRole).toString();
                    valHolder2 = holder2.toDouble(&valueCheck2);

                    // TODO check here, should linear be in as well?
                    if (mModel == DemandModel::Exponential)
                    {
                        // Drop consumption values of zero
                        if (valHolder1 <= 0)
                        {
                            continue;
                        }

                        valHolder1 = log10(valHolder1);

                        if (valueCheck1 && valueCheck2)
                        {
                            mDataPoints.append(QPair<double, double>(valHolder2, valHolder1));
                        }
                    }
                    else
                    {
                        if (valueCheck1 && valueCheck2)
                        {
                            mDataPoints.append(QPair<double, double>(valHolder2, valHolder1));
                        }
                    }
                }
            }
        }
    }
    else
    {
        for (int c = leftValue; c <= rightValue; c++)
        {
            for (int r = topValue; r <= bottomValue; r++)
            {
                if (table->item(r, c) != NULL && table->item(leftPrice, c) != NULL)
                {
                    holder1 = table->item(r, c)->data(Qt::DisplayRole).toString();
                    valHolder1 = holder1.toDouble(&valueCheck1);

                    holder2 = table->item(c, leftPrice)->data(Qt::DisplayRole).toString();
                    valHolder2 = holder2.toDouble(&valueCheck2);

                    if (mModel == DemandModel::Exponential)
                    {
                        // Drop consumption values of zero
                        if (valHolder1 <= 0)
                        {
                            continue;
                        }

                        valHolder1 = log10(valHolder1);

                        if (valueCheck1 && valueCheck2)
                        {                            
                            mDataPoints.append(QPair<double, double>(valHolder2, valHolder1));
                        }
                    }
                    else
                    {
                        if (valueCheck1 && valueCheck2)
                        {
                            mDataPoints.append(QPair<double, double>(valHolder2, valHolder1));
                        }
                    }
                }
            }
        }
    }

    QString x = "";
    QString y = "";

    x.append("[");
    y.append("[");

    qSort(mDataPoints.begin(), mDataPoints.end(), QPairFirstComparer());

    for (int i = 0; i < mDataPoints.count(); i++)
    {
        if (i == 0)
        {
            x.append("[" + QString::number(mDataPoints[i].first) + "]");
            y.append(QString::number(mDataPoints[i].second));
        }
        else
        {
            x.append(",[" + QString::number(mDataPoints[i].first) + "]");
            y.append("," + QString::number(mDataPoints[i].second));
        }
    }

    x.append("]");
    y.append("]");

    if (mModel == DemandModel::Exponential)
    {
        mObj->SetX(x.toUtf8().constData());
        mObj->SetY(y.toUtf8().constData());

        double mMaxK = log10(globalMax) + 0.5;

        QString mUpperBound("[" + QString::number(mMaxK) + ", +inf, 1]");

        mObj->SetBounds(mUpperBound.toUtf8().constData(), "[0.1, 0.1, -inf]");
        mObj->FitExponentialWithK("[1, 10, 0.0001]");

        returnK = mObj->GetParams()[0];
    }
    else if (mModel == DemandModel::Exponential)
    {
        mObj->SetX(x.toUtf8().constData());
        mObj->SetY(y.toUtf8().constData());

        double mMaxK = log10(globalMax) + 0.5;

        QString mUpperBound("[" + QString::number(mMaxK) + ", +inf, 1]");

        mObj->SetBounds(mUpperBound.toUtf8().constData(), "[0.1, 0.1, -inf]");

        // TODO fix here
        //mObj->FitExponentiatedWithK("[1, 10, 0.0001]");

        //returnK = mObj->GetParams()[0];
    }
}

void SheetWidget::areValuePointsValid(QStringList &valuePoints, QStringList &tempDelayPoints, QStringList delayPoints, bool isRowData, int topValue, int leftValue, int bottomValue, int rightValue, int i)
{
    valuePoints.clear();
    tempDelayPoints.clear();

    QString holder;
    bool valueCheck = true;
    double valHolder;

    int index = 0;

    if (isRowData)
    {
        int r = topValue;

        for (int c = leftValue; c <= rightValue; c++)
        {
            if (table->item(r + i, c) != NULL)
            {
                holder = table->item(r + i, c)->data(Qt::DisplayRole).toString();
                valHolder = holder.toDouble(&valueCheck);

                if (valueCheck)
                {
                    valuePoints << QString::number(valHolder);
                    tempDelayPoints << delayPoints.at(index);
                }
            }

            index++;
        }
    }
    else
    {
        int c = leftValue;

        for (int r = topValue; r <= bottomValue; r++)
        {
            if (table->item(r, c + i) != NULL)
            {
                holder = table->item(r, c + i)->data(Qt::DisplayRole).toString();
                valHolder = holder.toDouble(&valueCheck);

                if (valueCheck)
                {
                    valuePoints << QString::number(valHolder);
                    tempDelayPoints << delayPoints.at(index);
                }
            }

            index++;
        }
    }
}

/** Utilities
 * @brief
 */

QString SheetWidget::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void SheetWidget::convertExcelColumn(QString &mString, int column)
{
    int dividend = column + 1;
    QString columnName = "";

    int modulo;

    while (dividend > 0)
    {
        modulo = (dividend - 1) % 26;
        columnName = new QString(65 + modulo) + columnName;
        dividend = (int)((dividend - modulo) / 26);
    }

    mString = columnName;
}

