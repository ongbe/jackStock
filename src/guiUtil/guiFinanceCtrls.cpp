/**********************************************************************************
 *
 * Filename:    guiFinanceCtrl.cpp
 *
 *
 * Description:
 *
 *
 *
 ********************************************************************************/

#include "../../inc/guiUtil/guiFinanceCtrls.h"
#include "../../inc/guiUtil/guiFinanceColor.h"



/**********************************************************************************
 *
 * Function:
 *
 *
 * Description:
 *
 *
 *
 ********************************************************************************/
GuiFinanceCtrls::GuiFinanceCtrls(QWidget *parent) :
    QWidget(parent)
{

}



/**********************************************************************************
 *
 * Function:        fa5AddData()
 *
 *
 * Description:     Add data and set text color on 5 fundamental analysis controls
 *
 *
 *
 ********************************************************************************/
void GuiFinanceCtrls::fa5AddData(QString inEarningsDivDividend,  QLineEdit *earningsDivDividend,
                                 QString inNavDivStockPrice,     QLineEdit *navDivStockPrice,
                                 QString inPe,                   QLineEdit *pe,
                                 QString inPs,                   QLineEdit *ps,
                                 QString inYield,                QLineEdit *yield,
                                 QString assetType)
{

    GuiFinanceColor gfc;

    // Remove data
    fa5Reset(earningsDivDividend,
             navDivStockPrice,
             pe,
             ps,
             yield);

    // Add data
    earningsDivDividend->insert(inEarningsDivDividend);
    navDivStockPrice->insert(inNavDivStockPrice);
    pe->insert(inPe);
    ps->insert(inPs);
    yield->insert(inYield);

    // Set text color
    gfc.setTxtColorEarningsDivDividend(inEarningsDivDividend, earningsDivDividend);
    gfc.setTxtColorNavDivStockPrice(inNavDivStockPrice, navDivStockPrice);
    gfc.setTxtColorPe(inPe, pe, assetType);
    gfc.setTxtColorPs(inPs, ps);
    gfc.setTxtColorYield(inYield, inEarningsDivDividend, yield);
}




/**********************************************************************************
 *
 * Function:        fa5CtrlReset()
 *
 *
 * Description: Remove data from 5 fundamental analysis controls
 *
 *
 *
 ********************************************************************************/
void GuiFinanceCtrls::fa5Reset(QLineEdit *earningsDivDividend,
                               QLineEdit *navDivStockPrice,
                               QLineEdit *pe,
                               QLineEdit *ps,
                               QLineEdit *yield)
{
    earningsDivDividend->clear();
    navDivStockPrice->clear();
    pe->clear();
    ps->clear();
    yield->clear();
}


/*******************************************************************
 *
 * Function:    initHistoricalPe()
 *
 * Description: Init headers in treeWidget.
 *
 *
 *******************************************************************/
void GuiFinanceCtrls::
initHistoricalPe(QTreeWidget *treeWidget,
                 QString header0, int headerWidth0,
                 QString header1, int headerWidth1,
                 QString header2, int headerWidth2,
                 QString header3, int headerWidth3)
{

    MyTreeWidget mtw;

    mtw.setHeader4ColumnTreeWidget(treeWidget,
                               header0, headerWidth0,
                               header1, headerWidth1,
                               header2, headerWidth2,
                               header3, headerWidth3);
}



/*******************************************************************
 *
 * Function:    initHistoricalStockPrice()
 *
 * Description: Init headers in treeWidget.
 *
 *
 *
 *******************************************************************/
void GuiFinanceCtrls::
initHistoricalStockPrice(QTreeWidget *treeWidget,
                         QString header0, int headerWidth0,
                         QString header1, int headerWidth1,
                         QString header2, int headerWidth2,
                         QString header3, int headerWidth3)
{


    MyTreeWidget mtw;

    mtw.setHeader4ColumnTreeWidget(treeWidget,
                               header0, headerWidth0,
                               header1, headerWidth1,
                               header2, headerWidth2,
                               header3, headerWidth3);
}






/*******************************************************************
 *
 * Function:    initSalesGrowth()
 *
 * Description: Init headers in treeWidget.
 *
 * Försäljningstillväxt
 *
 *******************************************************************/
void GuiFinanceCtrls::
initSalesGrowth(QTreeWidget *treeWidget,
                QString header0, int headerWidth0,
                QString header1, int headerWidth1,
                QString header2, int headerWidth2,
                QString header3, int headerWidth3)
{


    MyTreeWidget mtw;

    mtw.setHeader4ColumnTreeWidget(treeWidget,
                                   header0, headerWidth0,
                                   header1, headerWidth1,
                                   header2, headerWidth2,
                                   header3, headerWidth3);
}




/*******************************************************************
 *
 * Function:    initProfitGrowth()
 *
 * Description: Init headers in treeWidget.
 *
 * Vinsttillväxt
 *
 *******************************************************************/
void GuiFinanceCtrls::
initProfitGrowth(QTreeWidget *treeWidget,
                QString header0, int headerWidth0,
                QString header1, int headerWidth1,
                QString header2, int headerWidth2,
                QString header3, int headerWidth3)
{


    MyTreeWidget mtw;

    mtw.setHeader4ColumnTreeWidget(treeWidget,
                                   header0, headerWidth0,
                                   header1, headerWidth1,
                                   header2, headerWidth2,
                                   header3, headerWidth3);
}




/*******************************************************************
 *
 * Function:    initProfitabilityAnalysis()
 *
 * Description: Init headers in treeWidget.
 *
 * Lönsamhetsanalys
 *
 *******************************************************************/
void GuiFinanceCtrls::
initProfitabilityAnalysis(QTreeWidget *treeWidget,
                          QString header0, int headerWidth0,
                          QString header1, int headerWidth1,
                          QString header2, int headerWidth2)
{


    MyTreeWidget mtw;

    mtw.setHeader3ColumnTreeWidget(treeWidget,
                                   header0, headerWidth0,
                                   header1, headerWidth1,
                                   header2, headerWidth2);
}



/*******************************************************************
 *
 * Function:    initStockList()
 *
 * Description: Init headers in treeWidget.
 *
 *
 *
 *
 *******************************************************************/
void GuiFinanceCtrls::initStockList(QTreeWidget *treeWidget,
                                    QString header0, int headerWidth0, bool hideCol0,
                                    QString header1, int headerWidth1, bool hideCol1,
                                    QString header2, int headerWidth2, bool hideCol2)
{

    MyTreeWidget mtw;

    mtw.setHeader3ColumnTreeWidget(treeWidget,
                                   header0, headerWidth0, hideCol0,
                                   header1, headerWidth1, hideCol1,
                                   header2, headerWidth2, hideCol2);
}


