#-------------------------------------------------
#
# Project created by QtCreator 2012-10-13T15:52:48
#
#-------------------------------------------------

CONFIG   += qwt
LIBS += -L/home/ajn/qwt-6.0.2/lib -lqwt
DEPENDPATH += /home/ajn/qwt-6.0.2/lib

INCLUDEPATH +=/home/ajn/qwt-6.0.2/include



QT       += qwt
QT       += core gui
QT       += network
QT	 += webkit
QT       += sql


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = JackStock
TEMPLATE = app


SOURCES += main.cpp\
    WebBrowserTab.cpp \
    util.cpp \
    tableTab.cpp \
    tabdialog.cpp \
    selIndicatorsDlg.cpp \
    parseYieldData.cpp \
    parsePriceData.cpp \
    parseKeyNumberData.cpp \
    myOs.cpp \
    jackStock.cpp \
    httpwindow.cpp \
    httpdownloader.cpp \
    guiUtils.cpp \
    extendedTable.cpp \
    dbHndl.cpp \
    extendedQwtPlot.cpp \
    TADlg.cpp \
    assistant.cpp \
    findfiledialog.cpp \
    textedit.cpp \
    parsesnapshotdata.cpp \
    debug.cpp \
    graphtab.cpp \
    stockPlotUtil.cpp \
    yahooStockPlotUtil.cpp \
    parseavanzadata.cpp \
    portfolio.cpp \
    parseTaData.cpp \
    fifo.cpp \
    importYahooTaData.cpp \
    canvaspicker.cpp \
    matrix.cpp \
    taAnalysis.cpp \
    efficientPortfolio.cpp \
    efficentbargraphdlg.cpp \
    threadcalcefficentfront.cpp \
    nordnetparsecompanyid.cpp \
    parsekeytabridgedata.cpp \
    importdata.cpp \
    parsecompanyeconomyinfo.cpp\
    taskQue.cpp \
    linkedList.cpp\
    tabhelp.cpp \
    parsecompdescription.cpp \
    fatab.cpp

HEADERS  += WebBrowserTab.h \
    util.h \
    tableTab.h \
    tabdialog.h \
    selIndicatorsDlg.h \
    parseYieldData.h \
    parsePriceData.h \
    parseKeyNumberData.h \
    myOs.h \
    jackStock.h \
    httpwindow.h \
    httpdownloader.h \
    guiUtils.h \
    extendedTable.h \
    dbHndl.h \
    common.h \
    extendedQwtPlot.h \
    TADlg.h \
    findfiledialog.h \
    assistant.h \
    textedit.h \
    parsesnapshotdata.h \
    debug.h \
    graphtab.h \
    stockPlotUtil.h \
    yahooStockPlotUtil.h \
    parseTaData.h \
    parseavanzadata.h \
    portfolio.h \
    fifo.h \
    sublist.h \
    importYahooTaData.h \
    commonFinance.h \
    canvaspicker.h \
    matrix.h \
    taAnalysis.h \
    efficientPortfolio.h \
    efficentbargraphdlg.h \
    threadcalcefficentfront.h \
    nordnetparsecompanyid.h \
    parsekeytabridgedata.h \
    importdata.h \
    parsecompanyeconomyinfo.h \
    taskQue.h \
    linkedList.h \
    tabhelp.h \
    parsecompdescription.h \
    fatab.h

FORMS    += \
    WebBrowserTab.ui \
    tableTab.ui \
    selIndicatorsDlg.ui \
    jackStock.ui \
    TADlg.ui \
    graphtab.ui \
    portfolio.ui \
    taAnalysis.ui \
    efficientPortfolio.ui \
    efficentbargraphdlg.ui \
    importdata.ui \
    tabhelp.ui \
    fatab.ui


