#-------------------------------------------------
#
# Project created by QtCreator 2017-05-08T14:58:04
#
#-------------------------------------------------

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
        mainwindow.cpp \
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
    demandsettingsdialog.cpp

HEADERS  += mainwindow.h \
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
    demandsettingsdialog.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    creditsdialog.ui \
    graphicaloutputdialog.ui \
    licensedialog.ui \
    resultsdialog.ui \
    sheetselectdialog.ui \
    statusdialog.ui \
    demandsettingsdialog.ui

RESOURCES += \
    spreadsheet.qrc

win32 {
    DMS_FILES.files = scripts/DiscountingAreaComputation.R \
                    scripts/DiscountingED50Computation.R scripts/installDependencyBase64.R \
                    scripts/installDependencyJsonlite.R scripts/installDependencyReshape.R \
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

    DMS_FILES.path = $$DESTDIR

    INSTALLS += DMS_FILES
}
macx {
    DMS_FILES.files = scripts/DiscountingAreaComputation.R \
                    scripts/DiscountingED50Computation.R scripts/installDependencyBase64.R \
                    scripts/installDependencyJsonlite.R scripts/installDependencyReshape.R \
                    License_base64enc.txt \
                    License_BDS.txt \
                    License_gnome_icons.txt \
                    License_jsonlite.txt \
                    License_NLS.txt \
                    License_Qt.txt \
                    License_R.txt \
                    License_reshape.txt \
                    COPYING

    DMS_FILES.path = Contents/Resources

    QMAKE_BUNDLE_DATA += DMS_FILES
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
    scripts/DiscountingAreaComputation.R \
    scripts/DiscountingED50Computation.R \
    scripts/installDependencyBase64.R \
    scripts/installDependencyJsonlite.R \
    scripts/installDependencyReshape.R
