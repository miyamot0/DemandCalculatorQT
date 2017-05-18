#-------------------------------------------------
#
# Project created by QtCreator 2017-05-08T14:58:04
#
#-------------------------------------------------

VERSION_MAJOR = 1
VERSION_MINOR = 0
VERSION_BUILD = 6

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
       "VERSION_MINOR=$$VERSION_MINOR"\
       "VERSION_BUILD=$$VERSION_BUILD"

QT       += core gui widgets xlsx svg

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
    steincheckdialog.cpp

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
    steincheckdialog.h

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

win32 {
    DCA_FILES.files = scripts/checkSystematic.R \
                    scripts/fitDemand.R \
                    scripts/installDependencyBase64.R \
                    scripts/installDependencyJsonlite.R \
                    scripts/installDependencyReshape.R \
                    scripts/installDependencyBeezdemand.R \
                    scripts/installDependencyDevtools.R \
                    scripts/installDependencyDigest.R \
                    License_base64enc.txt \
                    License_BDS.txt \
                    License_gnome_icons.txt \
                    License_jsonlite.txt \
                    License_NLS.txt \
                    License_Qt.txt \
                    License_R.txt \
                    License_reshape.txt \
                    COPYING

    release: DESTDIR = $$OUT_PWD/build/release
    debug:   DESTDIR = $$OUT_PWD/build/debug

    DCA_FILES.path = $$DESTDIR

    INSTALLS += DCA_FILES
}
macx {
    DCA_FILES.files = scripts/checkSystematic.R \
                    scripts/fitDemand.R \
                    scripts/installDependencyBase64.R \
                    scripts/installDependencyJsonlite.R \
                    scripts/installDependencyReshape.R \
                    scripts/installDependencyBeezdemand.R \
                    scripts/installDependencyDevtools.R \
                    scripts/installDependencyDigest.R \
                    License_base64enc.txt \
                    License_BDS.txt \
                    License_gnome_icons.txt \
                    License_jsonlite.txt \
                    License_NLS.txt \
                    License_Qt.txt \
                    License_R.txt \
                    License_reshape.txt \
                    COPYING

    DCA_FILES.path = Contents/Resources

    QMAKE_BUNDLE_DATA += DCA_FILES
}

DISTFILES += \
    License_base64enc.txt \
    License_BDS.txt \
    License_gnome_icons.txt \
    License_jsonlite.txt \
    License_NLS.txt \
    License_Qt.txt \
    License_R.txt \
    License_reshape.txt \
    COPYING \
    README.md \
    scripts/installDependencyBase64.R \
    scripts/installDependencyJsonlite.R \
    scripts/installDependencyReshape.R \
    scripts/installDependencyBeezdemand.R \
    scripts/installDependencyDevtools.R \
    scripts/installDependencyDigest.R \
    scripts/checkSystematic.R \
    scripts/fitDemand.R
