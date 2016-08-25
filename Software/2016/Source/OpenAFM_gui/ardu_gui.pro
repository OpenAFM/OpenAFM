#-------------------------------------------------
#
# Project created by QtCreator 2016-07-20T14:15:23
#
#-------------------------------------------------

QT       += datavisualization core gui serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = qt_ardu
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        tx_rx_protocol.cpp \
        surfacegraph.cpp \
        scannerwindow.cpp \
        qcustomplot.cpp \
        intensitymap.cpp


HEADERS  += mainwindow.h \
        def_commands.h \
        tx_rx_protocol.h \
        surfacegraph.h \
        scannerwindow.h \
        qcustomplot.h \
        intensitymap.h


FORMS    += mainwindow.ui \
            intensitymap.ui \


QMAKE_LFLAGS  += -Wl,-rpath,@executable_path/../Frameworks \
                 -Wl,-rpath,@executable_path/../PlugIns

