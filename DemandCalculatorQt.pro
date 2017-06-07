#-------------------------------------------------
#
# Demand Curve Analyzer, Qt Port
# Copyright 2017, Shawn P. Gilroy
# Released under the GPL-V3 license
#
# Source code for project hosted at:
#
# https://github.com/miyamot0/DemandCalculatorQT
#
#-------------------------------------------------

#-------------------------------------------------
#
# Project created by QtCreator 2017-05-08T14:58:04
#
#-------------------------------------------------

TEST_FEATURES = 0

VERSION_MAJOR = 1
VERSION_MINOR = 3
VERSION_BUILD = 0

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
       "VERSION_MINOR=$$VERSION_MINOR"\
       "VERSION_BUILD=$$VERSION_BUILD"\
       "VERSION_TESTING=$$TEST_FEATURES"

QT       += core gui widgets xlsx charts network xml

TARGET = DemandCalculatorQt

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp\
    aboutdialog.cpp \
    creditsdialog.cpp \
    licensedialog.cpp \
    resultsdialog.cpp \
    sheetselectdialog.cpp \
    sheetwidget.cpp \
    demandsettingsdialog.cpp \
    Libraries/alglib-3.11.0/src/alglibinternal.cpp \
    Libraries/alglib-3.11.0/src/alglibmisc.cpp \
    Libraries/alglib-3.11.0/src/ap.cpp \
    Libraries/alglib-3.11.0/src/dataanalysis.cpp \
    Libraries/alglib-3.11.0/src/diffequations.cpp \
    Libraries/alglib-3.11.0/src/fasttransforms.cpp \
    Libraries/alglib-3.11.0/src/integration.cpp \
    Libraries/alglib-3.11.0/src/interpolation.cpp \
    Libraries/alglib-3.11.0/src/linalg.cpp \
    Libraries/alglib-3.11.0/src/optimization.cpp \
    Libraries/alglib-3.11.0/src/solvers.cpp \
    Libraries/alglib-3.11.0/src/specialfunctions.cpp \
    Libraries/alglib-3.11.0/src/statistics.cpp \
    demandmodeling.cpp \
    chartwindow.cpp \
    steincheck.cpp \
    commanding.cpp \
    sheetdelegate.cpp

HEADERS  += \
    aboutdialog.h \
    creditsdialog.h \
    licensedialog.h \
    resultsdialog.h \
    sheetselectdialog.h \
    sheetwidget.h \
    demandsettingsdialog.h \
    Libraries/alglib-3.11.0/src/alglibinternal.h \
    Libraries/alglib-3.11.0/src/alglibmisc.h \
    Libraries/alglib-3.11.0/src/ap.h \
    Libraries/alglib-3.11.0/src/dataanalysis.h \
    Libraries/alglib-3.11.0/src/diffequations.h \
    Libraries/alglib-3.11.0/src/fasttransforms.h \
    Libraries/alglib-3.11.0/src/integration.h \
    Libraries/alglib-3.11.0/src/interpolation.h \
    Libraries/alglib-3.11.0/src/linalg.h \
    Libraries/alglib-3.11.0/src/optimization.h \
    Libraries/alglib-3.11.0/src/solvers.h \
    Libraries/alglib-3.11.0/src/specialfunctions.h \
    Libraries/alglib-3.11.0/src/statistics.h \
    Libraries/alglib-3.11.0/src/stdafx.h \
    demandmodeling.h \
    chartwindow.h \
    steincheck.h \
    commanding.h \
    sheetdelegate.h

FORMS    += \
    aboutdialog.ui \
    creditsdialog.ui \
    licensedialog.ui \
    resultsdialog.ui \
    sheetselectdialog.ui \
    demandsettingsdialog.ui \
    steincheck.ui

RESOURCES += \
    spreadsheet.qrc

###
#
# Win macro's, route to appropriate directories for installation prep
#
###
win32 {
    INCLUDEPATH += Libraries/alglib-3.11.0/src

    win32:RC_ICONS += SNS.ico

    DCA_FILES.files = License_ALGLIB.txt \
                    License_Tango.txt \
                    License_Beezdemand.txt \
                    License_Qt.txt \
                    COPYING \
                    SNS.ico


    CONFIG(debug, debug|release) {
        DESTDIR = $$OUT_PWD/build/debug
    } else {
        DESTDIR = $$OUT_PWD/build/release
    }

    DCA_FILES.path = $$DESTDIR

    INSTALLS += DCA_FILES
}

###
#
# Mac macro's, route to appropriate directories for installation prep
#
###
macx {
    INCLUDEPATH += Libraries/alglib-3.11.0/src

    macx:ICON = $${PWD}/SNS.icns

    DCA_FILES.files = License_ALGLIB.txt \
                    License_Tango.txt \
                    License_Beezdemand.txt \
                    License_Qt.txt \
                    COPYING \
                    SNS.icns

    DCA_FILES.path = Contents/Resources

    QMAKE_BUNDLE_DATA += DCA_FILES
}

DISTFILES += \
    License_ALGLIB.txt \
    License_Tango.txt \
    License_Beezdemand.txt \
    License_Qt.txt \
    COPYING \
    README.md \
    SNS.ico \
    SNS.icns
