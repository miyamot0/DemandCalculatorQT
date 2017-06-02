#-------------------------------------------------
#
# Project created by QtCreator 2017-05-08T14:58:04
#
#-------------------------------------------------

TEST_FEATURES = 0

VERSION_MAJOR = 1
VERSION_MINOR = 2
VERSION_BUILD = 6

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
    fitworker.cpp \
    graphicaloutputdialog.cpp \
    licensedialog.cpp \
    resultsdialog.cpp \
    rworker.cpp \
    sheetselectdialog.cpp \
    sheetwidget.cpp \
    statusdialog.cpp \
    demandsettingsdialog.cpp \
    steincheckdialog.cpp \
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
    Libraries/alglib-3.11.0/src/statistics.cpp

HEADERS  += \
    aboutdialog.h \
    creditsdialog.h \
    fitworker.h \
    graphicaloutputdialog.h \
    licensedialog.h \
    resultsdialog.h \
    rworker.h \
    sheetselectdialog.h \
    sheetwidget.h \
    statusdialog.h \
    demandsettingsdialog.h \
    steincheckdialog.h \
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
    Libraries/alglib-3.11.0/src/stdafx.h

FORMS    += \
    aboutdialog.ui \
    creditsdialog.ui \
    graphicaloutputdialog.ui \
    licensedialog.ui \
    resultsdialog.ui \
    sheetselectdialog.ui \
    statusdialog.ui \
    demandsettingsdialog.ui \
    steincheckdialog.ui

RESOURCES += \
    spreadsheet.qrc

###
#
# Win macro's, route to appropriate directories for installation prep
#
###
win32 {
    DCA_FILES.files = scripts/checkSystematic.R \
                    scripts/fitDemand.R \
                    scripts/installDependencyBase64.R \
                    scripts/installDependencyJsonlite.R \
                    scripts/installDependencyBeezdemand.R \
                    scripts/installDependencyDevtools.R \
                    scripts/installDependencyDigest.R \
                    License_gnome_icons.txt \
                    License_Qt.txt \
                    License_R.txt \
                    License_Beezdemand.txt \
                    License_fitDemand.txt \
                    COPYING \
                    SNS.ico

    win32:RC_ICONS += SNS.ico

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

    DCA_FILES.files = scripts/checkSystematic.R \
                    scripts/fitDemand.R \
                    scripts/installDependencyBase64.R \
                    scripts/installDependencyJsonlite.R \
                    scripts/installDependencyBeezdemand.R \
                    scripts/installDependencyDevtools.R \
                    scripts/installDependencyDigest.R \
                    License_gnome_icons.txt \
                    License_Qt.txt \
                    License_R.txt \
                    License_Beezdemand.txt \
                    License_fitDemand.txt \
                    COPYING \
                    SNS.icns

    DCA_FILES.path = Contents/Resources

    QMAKE_BUNDLE_DATA += DCA_FILES
}

DISTFILES += \
    License_gnome_icons.txt \
    License_Qt.txt \
    License_R.txt \
    License_Beezdemand.txt \
    License_fitDemand.txt \
    COPYING \
    README.md \
    scripts/installDependencyBase64.R \
    scripts/installDependencyJsonlite.R \
    scripts/installDependencyBeezdemand.R \
    scripts/installDependencyDevtools.R \
    scripts/installDependencyDigest.R \
    scripts/checkSystematic.R \
    scripts/fitDemand.R \
    SNS.ico \
    SNS.icns \
    License_ALGLIB.txt \
    License_Tango.txt
