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
VERSION_MINOR = 5
VERSION_BUILD = 6

CONFIG   += c++11

DEFINES  += "VERSION_MAJOR=$$VERSION_MAJOR"\
            "VERSION_MINOR=$$VERSION_MINOR"\
            "VERSION_BUILD=$$VERSION_BUILD"\
            "VERSION_TESTING=$$TEST_FEATURES"

QT       += core gui widgets xlsx network xml printsupport

TARGET   = DemandCalculatorQt

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES  += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Controls/chartwindow.cpp \
    Controls/sheetwidget.cpp \
    Dialogs/aboutdialog.cpp \
    Dialogs/creditsdialog.cpp \
    Dialogs/demandsettingsdialog.cpp \
    Dialogs/licensedialog.cpp \
    Dialogs/resultsdialog.cpp \
    Dialogs/sheetselectdialog.cpp \
    Dialogs/steincheck.cpp \
    Helpers/Threading/calculationworker.cpp \
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
    Libraries/numerics/numericsport.cpp \
    Models/calculationsettings.cpp \
    Modeling/demandmodeling.cpp \
    Models/fittingdata.cpp \
    Models/lambertresult.cpp \
    Utilities/commanding.cpp \
    Utilities/sheetdelegate.cpp \
    Utilities/qcustomplot.cpp \
    main.cpp

HEADERS  += \
    Controls/chartwindow.h \
    Controls/sheetwidget.h \
    Dialogs/aboutdialog.h \
    Dialogs/creditsdialog.h \
    Dialogs/demandsettingsdialog.h \
    Dialogs/licensedialog.h \
    Dialogs/resultsdialog.h \
    Dialogs/sheetselectdialog.h \
    Dialogs/steincheck.h \
    Helpers/Demand/demandmeasures.h \
    Helpers/Threading/calculationworker.h \
    Helpers/GeneticAlgorithms/evolutionfunctions.h \
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
    Libraries/differential-evolution/differentialevolution.h \
    Libraries/numerics/numericsport.h \
    Models/calculationsettings.h \
    Models/fittingdata.h \
    Models/lambertresult.h \
    Modeling/demandmodeling.h \
    Utilities/commanding.h \
    Utilities/sheetdelegate.h \
    Utilities/tags.h \
    Utilities/qcustomplot.h

FORMS    += \
    Dialogs/aboutdialog.ui \
    Dialogs/creditsdialog.ui \
    Dialogs/licensedialog.ui \
    Dialogs/resultsdialog.ui \
    Dialogs/sheetselectdialog.ui \
    Dialogs/demandsettingsdialog.ui \
    Dialogs/steincheck.ui

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
                      License_Beezdemand.txt \
                      License_differential-evolution.txt \
                      License_GSL.txt \
                      License_Math.NET.txt \
                      License_Qt.txt \
                      License_Tango.txt \
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
                      License_Beezdemand.txt \
                      License_differential-evolution.txt \
                      License_GSL.txt \
                      License_Math.NET.txt \
                      License_Qt.txt \
                      License_Tango.txt \
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
    SNS.icns \
    License_GSL.txt \
    License_Math.NET.txt \
    License_differential-evolution.txt
