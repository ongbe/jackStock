#include "stockAnalysisTab.h"
#include "ui_stockanalysistab.h"
#include "util.h"
#include "inc/guiUtil/guiFinanceColor.h"
#include "dbHndl.h"
#include "extendedLineEdit.h"
#include "yahooStockPlotUtil.h"
#include "createstockanalysishtmlpage.h"
#include <QtWebKit/QWebView>
#include "util.h"
#include "math.h"
#include <qwt_scale_engine.h>
#include "extendedQwtPlot.h"
#include <qwt_plot_histogram.h>
#include "myLibCurl.h"
#include "myLineEdit.h"
#include "financemath.h"
#include "treewidgetfinance.h"

#define INDEX_MY_PORTFOLIO      ((int) 3)


//Sub Analys Tables defines
#define SAT_COLUMN_DATE                  0
#define SAT_COLUMN_DATA                  1
#define SAT_NOF_ROWS                    20
#define SAT_NOF_COLUMNS                  2
#define SAT_FONT_NAME                    QString::fromUtf8("Helvetica")
#define SAT_FONT_SIZE                    9




const QString StockAnalysisTab::m_companyTypesArr[NOF_COMPANY_TYPE_ARR_DATA] =
                            {QString::fromUtf8("Odefinierad"),
                            QString::fromUtf8("Handel"),
                            QString::fromUtf8("Industribolag"),
                            QString::fromUtf8("Bank"),
                            QString::fromUtf8("Fastighetsbolag"),
                            QString::fromUtf8("Försörjningsbolag"),
                            QString::fromUtf8("Investmentbolag"),
                            QString::fromUtf8("Allmännyttiga företag")}; // t.ex energibolag (Skulder/eget kapital exklusive immateriella tillgångar < 2)


/******************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *
 *
 *****************************************************************/
StockAnalysisTab::StockAnalysisTab(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StockAnalysisTab)
{


    QString path;

    m_red_palette = new QPalette();
    m_red_palette->setColor(QPalette::WindowText,Qt::red);

    m_blue_palette = new QPalette();
    m_blue_palette->setColor(QPalette::WindowText,Qt::blue);


    ui->setupUi(this);
    m_qwtcashFlowPlotData.nofStocksToPlot = 0;

    m_stockNameIsInit = false;

    initNetProfitAfterTaxTreeWidget();

    initSubAnalysTables();

    m_gfc.initStockList1(ui->treeWidgetStockListAnalysis);
    m_gfc.addAllStockListsToCombobox(ui->StockListComboBoxAnalysis);
    ui->StockListComboBoxAnalysis->setCurrentIndex(INDEX_MY_PORTFOLIO);


    m_gfc.initStockAnalysisDateList(ui->treeWidgetAnalysisDate);

    on_SelStockListButton_clicked();

    // Show html file that describes investment strategy
    ui->webView_2->load(QUrl("qrc:/database/AnalysDoc/AnalysDoc/Investeringskriterier.html"));

    path = QString::fromUtf8("file:///");
    path += qApp->applicationDirPath();
    path += QString::fromUtf8("/");
    path += QString::fromUtf8("/database/AnalysDoc/AnalysisStockhomLargeMidCap.html");

    qDebug() << path;

    //qDebug() << "App path : " << qApp->applicationDirPath();

//    ui->webViewAllAnalyzedComp->load(QUrl("file:///home/ajn/Documents/OldPC/swProj/MyQtProj/JackStockProj/JackStock/database/AnalysDoc/Investeringskriterier.html"));
    ui->webViewAllAnalyzedComp->load(QUrl(path));



    // New plots
    initAllAnalysisPlots();

    // Old plots
    initSubAnalysisPlots();
}




/******************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *
 *
 *****************************************************************/
StockAnalysisTab::~StockAnalysisTab()
{
    delete ui;
    delete m_barHist;
    delete m_red_palette;
    delete m_blue_palette;
}



/*******************************************************************
 *
 * Function:       initNetProfitAfterTaxTreeWidget()
 *
 * Description:
 *
 *
 *******************************************************************/
void StockAnalysisTab::initNetProfitAfterTaxTreeWidget(void)
{

    QString column0 = QString::fromUtf8("År");
    QString column1 = QString::fromUtf8("Kr/Aktie");
    QString column2 = QString::fromUtf8("Tillväxt (%)");

    ui->treeWidgetProfitGrowth->setColumnCount(3);
    ui->treeWidgetProfitGrowth->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeWidgetProfitGrowth->setSelectionBehavior(QAbstractItemView::SelectRows);



    if(QTreeWidgetItem* header = ui->treeWidgetProfitGrowth->headerItem())
    {
        header->setText(0, column0);
        header->setText(1, column1);
        header->setText(2, column2);
    }

    ui->treeWidgetProfitGrowth->setColumnWidth(0, 65);
    ui->treeWidgetProfitGrowth->setColumnWidth(1, 65);
    ui->treeWidgetProfitGrowth->setColumnWidth(2, 60);
    ui->treeWidgetProfitGrowth->setColumnWidth(3, 50);

    ui->treeWidgetProfitGrowth->setFont(QFont("Helvetica", 9));
}

/******************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::
addEarningAndGrowsToTreeWidget(bool &leastSqrtFitIsValid,
                               double &k,
                               double &m,
                               double &minX,
                               double &maxX)
{
    FinanceMath fm;
    TreeWidgetFinance twf;
    QString year;
    QString earningPerShare;
    QString earningGrowth;

    leastSqrtFitIsValid =false;

    if(m_nofEarningsArrData < 2)
    {
        return;
    }

    // Calc earning growth
    m_nofEarningsGrowthArrData = 0;
    if(false == fm.calcGrowth(m_earningsDataArr, m_nofEarningsArrData, m_earningsGrowthArr))
    {
        return;
    }

    m_nofEarningsGrowthArrData = m_nofEarningsArrData;
    ui->treeWidgetProfitGrowth->clear();

    for(int i = 0; i < m_nofEarningsArrData; i++)
    {
        year = m_earningsDataArr[i].date;
        earningPerShare = m_earningsDataArr[i].data;
        earningGrowth = m_earningsGrowthArr[i].data;
        if(false == twf.addTreeWidgetData(ui->treeWidgetProfitGrowth, year, earningPerShare, earningGrowth, true))
        {
            return;
        }
    }

    if(ui->checkBoxShowForecast_2->isChecked())
    {
        int colToUse = 1;
        int nofDataToAdd = 5;

        if(true == twf.calcLeastSqrtFit(ui->treeWidgetProfitGrowth, k, m, minX, maxX, colToUse))
        {
            leastSqrtFitIsValid = true;
            twf.addLeastSqrtFitAndGrowthRateDataToTreeWidget(ui->treeWidgetProfitGrowth, k, m, maxX, nofDataToAdd);
        }
    }

    // bool calcLeastSqrtFit(QTreeWidget *treeWidget, double &k, double &m, double &minX, double &maxX, int colToUse);
    // void addLeastSqrtFitAndGrowthRateDataToTreeWidget(QTreeWidget *treeWidget, double k, double m, double maxX, int nofDataToAdd);
}



/******************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::initSubAnalysisPlots(void)
{
    CExtendedQwtPlot eqp;


    // Hide axises
    m_qwtPlot[0] = ui->qwtPlot_0;
    m_qwtPlot[0]->enableAxis(QwtPlot::xBottom, false);
    m_qwtPlot[0]->enableAxis(QwtPlot::yLeft, false);
   // m_qwtPlot[0]->hide();

    m_qwtPlot[1] = ui->qwtPlot_1;
    m_qwtPlot[1]->enableAxis(QwtPlot::xBottom, false);
    m_qwtPlot[1]->enableAxis(QwtPlot::yLeft, false);

    m_qwtPlot[2] = ui->qwtPlot_2;
    m_qwtPlot[2]->enableAxis(QwtPlot::xBottom, false);
    m_qwtPlot[2]->enableAxis(QwtPlot::yLeft, false);

    m_qwtPlot[3] = ui->qwtPlot_3;
    m_qwtPlot[3]->enableAxis(QwtPlot::xBottom, false);
    m_qwtPlot[3]->enableAxis(QwtPlot::yLeft, false);

    m_qwtPlot[4] = ui->qwtPlot_4;
    m_qwtPlot[4]->enableAxis(QwtPlot::xBottom, false);
    m_qwtPlot[4]->enableAxis(QwtPlot::yLeft, false);

    m_qwtPlot[5] = ui->qwtPlot_5;
    m_qwtPlot[5]->enableAxis(QwtPlot::xBottom, false);
    m_qwtPlot[5]->enableAxis(QwtPlot::yLeft, false);

    m_qwtPlot[6] = ui->qwtPlot_6;
    m_qwtPlot[6]->enableAxis(QwtPlot::xBottom, false);
    m_qwtPlot[6]->enableAxis(QwtPlot::yLeft, false);

    m_qwtPlot[7] = ui->qwtPlot_7;
    m_qwtPlot[7]->enableAxis(QwtPlot::xBottom, false);
    m_qwtPlot[7]->enableAxis(QwtPlot::yLeft, false);

    m_qwtPlot[8] = ui->qwtPlot_8;
    m_qwtPlot[8]->enableAxis(QwtPlot::xBottom, false);
    m_qwtPlot[8]->enableAxis(QwtPlot::yLeft, false);

    m_qwtPlot[9] = ui->qwtPlot_9;
    m_qwtPlot[9]->enableAxis(QwtPlot::xBottom, false);
    m_qwtPlot[9]->enableAxis(QwtPlot::yLeft, false);

    // Is handle in difftent way ordinary graph
    ui->qwtCashFlowPlot->enableAxis(QwtPlot::xBottom, false);
    ui->qwtCashFlowPlot->enableAxis(QwtPlot::yLeft, false);

    eqp.initPlotPicker(ui->qwtPlot_10);
}



/******************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::initSubAnalysTables(void)
{
    QString dateHeader = QString::fromUtf8("   År    ");
    QString dataHeader;

    initCompanyTypeComboBox();



    //------------------------------------------------------------------------------
    // Dividend
    dataHeader = QString::fromUtf8("Utdel/Aktie");
    initSubAnalyseTableWidget(ui->tableWidgetDividend, dateHeader, dataHeader);
    m_nofDividendArrData = 0;


    // Dividend (IntrinsicValue tab)
    dataHeader = QString::fromUtf8("Utdel/Aktie");
    initSubAnalyseTableWidget(ui->tableWidgetIntrinsicValueDividendPerShare, dateHeader, dataHeader);
    //------------------------------------------------------------------------------

    // TotDividends ((Total utdelning)
    dataHeader = QString::fromUtf8("Total utdelning");
    initSubAnalyseTableWidget(ui->tableWidgetTotDividends, dateHeader, dataHeader);
    m_nofTotDividensData = 0;

    //------------------------------------------------------------------------------
    // Earnings
    dataHeader = QString::fromUtf8("Vinst/Aktie");
    initSubAnalyseTableWidget(ui->tableWidgetEarnings, dateHeader, dataHeader);
    m_nofEarningsArrData = 0;

    // Earning growth
    m_nofEarningsGrowthArrData = 0;

    // Earnings (IntrinsicValue tab)
    dataHeader = QString::fromUtf8("Vinst/Aktie");
    initSubAnalyseTableWidget(ui->tableWidgetIntrinsicValueEarningsPerShare, dateHeader, dataHeader);
    //------------------------------------------------------------------------------

    // Total Earnings, Net Income (Vinst)
    dataHeader = QString::fromUtf8("     Vinst      ");
    initSubAnalyseTableWidget(ui->tableWidgetNetIncome, dateHeader, dataHeader);
    m_nofTotEarningsArrData = 0;


    //------------------------------------------------------------------------------
    // TotalCurrentAssets
    dataHeader = QString::fromUtf8("Oms.tillg");
    initSubAnalyseTableWidget(ui->tableWidgetTotalCurrentAsset, dateHeader, dataHeader);
    m_nofTotalCurrentAssetsArrData = 0;

    // totalCurrentLiabilities
    dataHeader = QString::fromUtf8("Kortf.skuld");
    initSubAnalyseTableWidget(ui->tableWidgetTotalCurrentLiabilities, dateHeader, dataHeader);
    m_nofTotalCurrentLiabilitiesData = 0;

    // totalLiabilities
    dataHeader = QString::fromUtf8("Totala Skuld");
    initSubAnalyseTableWidget(ui->tableWidgetTotalLiabilities, dateHeader, dataHeader);
    m_nofTotalLiabilitiesData = 0;


    // TotalCurrentAssets/totalLiabilities (IntrinsicValue tab)
    dataHeader = QString::fromUtf8("O.tillg/T.Skuld");
    initSubAnalyseTableWidget(ui->tableWidgetIntrinsicValueTotalCurrentAssetDivTotalLiabilities, dateHeader, dataHeader);
    //------------------------------------------------------------------------------


    // Solidity
    dataHeader = QString::fromUtf8("Soliditet (%)");
    initSubAnalyseTableWidget(ui->tableWidgetSolidity, dateHeader, dataHeader);
    m_nofSolidityData = 0;


    // Coverage Ratio (Räntetäckningsgrad)
    dataHeader = QString::fromUtf8("RänteTG ggr");
    initSubAnalyseTableWidget(ui->tableWidgetCoverageRatio, dateHeader, dataHeader);
    m_nofCoverageRatioData = 0;


    // Core Tier1 Ratio (Primärkapitalrelation)
    dataHeader = QString::fromUtf8("primärkap.rel");
    initSubAnalyseTableWidget(ui->tableWidgetCoreTier1Ratio, dateHeader, dataHeader);
    m_nofCoreTier1RatioData = 0;


    // CoreCapitalRatio (Kärnprimärkapitalrelation)
    dataHeader = QString::fromUtf8("kärnprimärkap.rel");
    initSubAnalyseTableWidget(ui->tableWidgetCoreCapitalRatio, dateHeader, dataHeader);
    m_nofCoreCapitalRatioData = 0;


    // Equity (Eget kapital)
    dataHeader = QString::fromUtf8("Eget kapital");
    initSubAnalyseTableWidget(ui->tableWidgetEquity, dateHeader, dataHeader);
    m_nofTotEquityData = 0;


    // EquityPerShare (Eget kapital/Aktie)
    dataHeader = QString::fromUtf8("Eget kapital/Aktie");
    initSubAnalyseTableWidget(ui->tableWidgetEquityPerShare, dateHeader, dataHeader);
    m_nofEquityPerShareData = 0;


    // CashFlowCapex ((Kassaflöde Capex Investeringar/kapitalutgifter))
    dataHeader = QString::fromUtf8("    Capex    ");
    initSubAnalyseTableWidget(ui->tableWidgetCashFlowCapex, dateHeader, dataHeader);
    m_nofCashFlowCapexData = 0;


    // CashFlowCapex ((operativt kassaflöde)
    dataHeader = QString::fromUtf8("Operativt kflöde");
    initSubAnalyseTableWidget(ui->tableWidgetOperatingCashFlow, dateHeader, dataHeader);
    m_nofOperatingCashFlowData = 0;


    // Revenue (Försäljning)
    dataHeader = QString::fromUtf8("Försäljning");
    initSubAnalyseTableWidget(ui->tableWidgetRevenue, dateHeader, dataHeader);
    m_nofRevenueArrData = 0;
}



/******************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::initCompanyTypeComboBox(void)
{
    ui->comboBoxCompanyType->clear();

    for(int i = 0; i < NOF_COMPANY_TYPE_ARR_DATA; i++)
    {
        ui->comboBoxCompanyType->addItem(m_companyTypesArr[i]);
    }

    ui->comboBoxCompanyType->setCurrentIndex(0);

}




/******************************************************************
 *
 * Function:    on_SelStockListButton_clicked()
 *
 * Description: init Sub Analyse QTableWidget.
 *
 *              This table has 20 rows and 2 columns
 *
 * ****************************************************************/
void StockAnalysisTab::initSubAnalyseTableWidget(QTableWidget *tableWidget,
                                                 QString dateHeader,
                                                 QString dataHeader)
{

    // Set table size
    tableWidget->setRowCount(SAT_NOF_ROWS);
    tableWidget->setColumnCount(SAT_NOF_COLUMNS);

    // Init label
    QStringList labels;
    labels.clear();
    labels << dateHeader << dataHeader;
    tableWidget->setHorizontalHeaderLabels(labels);

    // Init headers
    tableWidget->horizontalHeader()->setResizeMode(SAT_COLUMN_DATE, QHeaderView::ResizeToContents);
    tableWidget->horizontalHeader()->setResizeMode(SAT_COLUMN_DATA, QHeaderView::ResizeToContents);

    // Init font
    QFont font;
    font = tableWidget->horizontalHeader()->font();
    font.setPointSize(SAT_FONT_SIZE);
    font.setFamily(SAT_FONT_NAME);
    tableWidget->horizontalHeader()->setFont( font );
    tableWidget->verticalHeader()->setFont( font );

}



/******************************************************************
 *
 * Function:    on_SelStockListButton_clicked()
 *
 * Description: Invoked when user pressing Select stock list button
 *
 *
 *
 * ****************************************************************/
void StockAnalysisTab::on_SelStockListButton_clicked()
{
    CDbHndl db;

     m_stockListName = ui->StockListComboBoxAnalysis->currentText().toUtf8();

     qDebug() << m_stockListName;

     if(true == db.findTaStockListId(m_stockListName, m_stockListId))
     {
         ui->treeWidgetStockListAnalysis->clear();
         db.addStockSymbolAndNameInTreeWidget1(ui->treeWidgetStockListAnalysis,
                                            m_stockListId,
                                            CDbHndl::SORT_TWIDGET_NAME,
                                            (char *) SQL_STR_ASC);


         ui->treeWidgetStockListAnalysis->setColumnWidth(0, 160);
         ui->treeWidgetStockListAnalysis->scrollToTop();
         ui->treeWidgetStockListAnalysis->setCurrentItem(ui->treeWidgetStockListAnalysis->topLevelItem(0));

     }

}


/******************************************************************
 *
 * Function:    on_treeWidgetStockListAnalysis_doubleClicked()
 *
 * Description: Select stock to analyse and display all date of analysis
 *
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::on_treeWidgetStockListAnalysis_doubleClicked(const QModelIndex &index)
{
    CDbHndl db;
    CUtil cu;
    QString startDate;
    QString riskStr;
    QString returnStr;
    int intMonth = -12;
    bool res;
    GuiFinanceColor gfc;
    ExtendedLineEdit ele;
    QPalette palette[10];
    QColor gColor;

    Qt::GlobalColor color = Qt::magenta;
    CDbHndl::EfficPortStockData_ST riskReturnData;
    CDbHndl::snapshotStockData_ST faData;
    CDbHndl::YahooNordnetOutputkeyData_ST yahooKeyData;
    NordnetCompanyDescription_ST companyDescripData;

    // Reset all edit ctrl i analysis graphs2 (on all tabs)
    clearAllAnalysisEditCtrls();

    cu.getCurrentDate(m_analysisDate);
    cu.getCurrentDate(startDate);

    ui->analysisDateLineEdit->setText(m_analysisDate);

    if(false == cu.addMonth(m_analysisDate, startDate, intMonth))
    {
        return;
    }

     resetGuiCtrl();
     clearGUIIntrinsicValue();

    m_gfc.getSelStockListItem(ui->treeWidgetStockListAnalysis, m_stockName, m_stockSymbol, index);
    ui->stockNameLineEdit->setText(m_stockName);
    ui->stockSymbolLineEdit->setText(m_stockSymbol);


    if(m_stockName.size() > 0 && m_stockSymbol.size() > 0)
    {
        m_stockNameIsInit = true;
        db.getStockAnalysisDate(m_stockName, m_stockSymbol, ui->treeWidgetAnalysisDate);
    }

    // Page analysdata 1
    ui->StockNameLabel_16->setText(m_stockName);
    ui->ananlysisDateLabel_32->setText(m_analysisDate);

    // Page analysdata 2
    ui->StockNameLabel_17->setText(m_stockName);
    ui->ananlysisDateLabel_33->setText(m_analysisDate);

    if(startDate.isEmpty() == false && m_analysisDate.isEmpty() == false)
    {
        if(true == db.getKeyDataUseBridge(m_stockSymbol, faData))
        {
            gfc.getColorEarningsDivDividend(faData.earningsDividedByDividend, color);
            ui->lineEditEarningsDivByDividend->setText(faData.earningsDividedByDividend);
            ele.setTxtColor(ui->lineEditEarningsDivByDividend, &palette[0], color);

            gfc.getColorNavDivStockPrice(faData.navDivLastStockPrice, color);
            ui->lineEditNavDivLastStockPrice->setText(faData.navDivLastStockPrice);
            ele.setTxtColor(ui->lineEditNavDivLastStockPrice, &palette[1], color);

            gfc.getColorPe(faData.keyValuePE, color);
            ui->lineEditPE->setText(faData.keyValuePE);
            ele.setTxtColor(ui->lineEditPE, &palette[2], color);

            gfc.getColorPs(faData.keyValuePS, color);
            ui->lineEditPs->setText(faData.keyValuePS);
            ele.setTxtColor(ui->lineEditPs, &palette[3], color);

            gColor = gfc.getColorYield(faData.keyValueYield, faData.earningsDividedByDividend);
            ui->lineEditYield->setText(faData.keyValueYield);
            ele.setTxtColor(ui->lineEditYield, &palette[4], gColor);

        }


        if(true == db.getNordnetCompanyDescription(companyDescripData, faData.companyName))
        {
            //ui->textEditMoreInfobusinessDescription->clear();

            //ui->lineEditMoreInfoCompanyName->setText(companyDescripData.assetName);
            //ui->lineEditMoreInfoexecutiveDirector->setText(companyDescripData.executiveDirector);
            //ui->lineEditMoreInfoBransch->setText(companyDescripData.bransch);
            //ui->lineEditMoreInfoSector->setText(companyDescripData.sector);
            //ui->textEditMoreInfobusinessDescription->insertPlainText(companyDescripData.businessDescription);

        }

    }

    m_riskStdDev.clear();
    m_meanReturns.clear();
    m_returnOnEquity.clear();


    riskReturnData.stockSymbol = m_stockSymbol;
    res = m_gfc.calcRiskAndReturn(startDate, m_analysisDate, riskReturnData);

    if(true == res)
    {
        color = gfc.getColorRiskReturns(riskReturnData.riskStdDev, riskReturnData.meanReturns);

        riskStr.sprintf("%.3f", riskReturnData.riskStdDev);

        returnStr.sprintf("%.3f", riskReturnData.meanReturns);

        ui->lineEditExpectedReturn->setText(returnStr);
        ui->lineEditExpectedRisk->setText(riskStr);

        m_riskStdDev = riskStr;
        m_meanReturns = returnStr;


        ele.setTxtColor(ui->lineEditExpectedReturn, &palette[5], color);
        ele.setTxtColor(ui->lineEditExpectedRisk, &palette[6], color);
    }


    if(true == db.getNordnetYahooSingleKeyData(m_stockSymbol,
                                      m_stockListId,
                                      m_stockListName,
                                      yahooKeyData))
    {

        gfc.getColorCurrentRatio(yahooKeyData.currentRatio, color);
        ui->lineEditCurrentRatio->setText(yahooKeyData.currentRatio);
        ele.setTxtColor(ui->lineEditCurrentRatio, &palette[7], color);


        gfc.getColorDebtToEquityRatio(yahooKeyData.totalDebtToEquityRatio, color);
        ui->lineEditTotDebtEquityRatio->setText(yahooKeyData.totalDebtToEquityRatio);
        ele.setTxtColor(ui->lineEditTotDebtEquityRatio, &palette[8], color);

        ui->lineEditReturnOnEquity->setText(yahooKeyData.returnOnEquity);
        m_returnOnEquity = yahooKeyData.returnOnEquity;
    }


    //====================================================================
    // Get and display sub data
    //====================================================================


    updateTableWithSubAnalysData(ui->tableWidgetNetIncome,
                                 SAD_TOT_EARNINGS,
                                 m_totEarningsDataArr,
                                 m_nofTotEarningsArrData);


    updateTableWithSubAnalysData(ui->tableWidgetRevenue,
                                 SAD_REVENUE,
                                 m_revenueDataArr,
                                 m_nofRevenueArrData);

    //------------------------------------------------------

    updateTableWithSubAnalysData(ui->tableWidgetIntrinsicValueDividendPerShare,
                                 SAD_DIVIDEND,
                                 m_dividendDataArr,
                                 m_nofDividendArrData);

    // Update intrinsic Line edit ctrl
    if(m_nofDividendArrData > 1)
    {
        ui->lineEditEstimateYearlyDividend->setText(m_dividendDataArr[m_nofDividendArrData-1].data);
    }

    updateTableWithSubAnalysData(ui->tableWidgetDividend,
                                 SAD_DIVIDEND,
                                 m_dividendDataArr,
                                 m_nofDividendArrData);
    //------------------------------------------------------


    //------------------------------------------------------

    updateTableWithSubAnalysData(ui->tableWidgetIntrinsicValueEarningsPerShare,
                                 SAD_EARNINGS,
                                 m_earningsDataArr,
                                 m_nofEarningsArrData);

    updateTableWithSubAnalysData(ui->tableWidgetEarnings,
                                 SAD_EARNINGS,
                                 m_earningsDataArr,
                                 m_nofEarningsArrData);


     //------------------------------------------------------

    updateTableWithSubAnalysData(ui->tableWidgetTotalCurrentAsset,
                                 SAD_TOTAL_CURRENT_ASSETS,
                                 m_totalCurrentAssetsArr,
                                 m_nofTotalCurrentAssetsArrData);

    updateTableWithSubAnalysData(ui->tableWidgetTotalCurrentLiabilities,
                                 SAD_TOTAL_CURRENT_LIABILITIES,
                                 m_totalCurrentLiabilitiesArr,
                                 m_nofTotalCurrentLiabilitiesData);

    updateTableWithSubAnalysData(ui->tableWidgetTotalLiabilities,
                                 SAD_TOTAL_LIABILITIES,
                                 m_totalLiabilitiesArr,
                                 m_nofTotalLiabilitiesData);

    insertTableWidgetIntrinsicValueTotCurrAssetDivTotLiabilities(ui->tableWidgetIntrinsicValueTotalCurrentAssetDivTotalLiabilities,
                                                                 m_totalCurrentAssetsArr,
                                                                 m_nofTotalCurrentAssetsArrData,
                                                                 m_totalLiabilitiesArr,
                                                                 m_nofTotalLiabilitiesData);


    //---------------------------------------------------------------------------


    updateTableWithSubAnalysData(ui->tableWidgetSolidity,
                                 SAD_SOLIDITY,
                                 m_solidityArr,
                                 m_nofSolidityData);


    updateTableWithSubAnalysData(ui->tableWidgetCoverageRatio,
                                 SAD_COVERAGE_RATIO,
                                 m_coverageRatioArr,
                                 m_nofCoverageRatioData);

    updateTableWithSubAnalysData(ui->tableWidgetCoreTier1Ratio,
                                 SAD_CORE_TIER_1_RATIO,
                                 m_coreTier1RatioArr,
                                 m_nofCoreTier1RatioData);

    updateTableWithSubAnalysData(ui->tableWidgetCoreCapitalRatio,
                                 SAD_CORE_CAPITAL_RATIO,
                                 m_coreCapitalRatioArr,
                                 m_nofCoreCapitalRatioData);

    updateTableWithSubAnalysData(ui->tableWidgetEquity,
                                 SAD_EQUITY,
                                 m_totEquityArr,
                                 m_nofTotEquityData);

    updateTableWithSubAnalysData(ui->tableWidgetEquityPerShare,
                                 SAD_EQUITY_PER_SHARE,
                                 m_equityPerShareArr,
                                 m_nofEquityPerShareData);

    if(m_nofEquityPerShareData > 1)
    {
        // Update Intrinsic value line edit ctrl
        ui->lineEditCurrEquityPerShare->setText(m_equityPerShareArr[m_nofEquityPerShareData-1].data);
    }



    if(m_nofEquityPerShareData > 1)
    {
        plotEquityPerShareData(m_equityPerShareArr, m_nofEquityPerShareData);
    }




    updateTableWithSubAnalysData(ui->tableWidgetCashFlowCapex,
                                 SAD_CASH_FLOW_CAPEX,
                                 m_cashFlowCapexArr,
                                 m_nofCashFlowCapexData);

    updateTableWithSubAnalysData(ui->tableWidgetOperatingCashFlow,
                                 SAD_OPERATING_CASH_FLOW,
                                 m_operatingCashFlowArr,
                                 m_nofOperatingCashFlowData);

    updateTableWithSubAnalysData(ui->tableWidgetTotDividends,
                                 SAD_TOTAL_DIVIDENT,
                                 m_totDividensArr,
                                 m_nofTotDividensData);


    m_saDisply.subAnalysisClearAllGraphs(m_qwtPlot);
    plotCashflowData();

    calcTotSubdataForIntrinsicValue();
    on_pushButtonCalcYearlyIntrestRateOnEquity_clicked();

    int mainAnalysisId;
    int companyType = 0;

    // Reset Company type
    ui->labelCompanyType->setText(m_companyTypesArr[companyType]);
    ui->labelMainPglabelCompanyType->setText(m_companyTypesArr[companyType]);

    ui->comboBoxCompanyType->setCurrentIndex(companyType);

    if(true == db.mainAnalysisDataExists(m_stockName, m_stockSymbol, mainAnalysisId))
    {
        if(true ==  db.getSubAnalysisCompanyType(mainAnalysisId, companyType))
        {
            if(companyType < NOF_COMPANY_TYPE_ARR_DATA)
            {
                ui->labelCompanyType->setText(m_companyTypesArr[companyType]);
                ui->labelMainPglabelCompanyType->setText(m_companyTypesArr[companyType]);
                ui->comboBoxCompanyType->setCurrentIndex(companyType);
            }
        }
    }

    // Display all analysis graphs
    displayAllAnalysisPlots();


}


/******************************************************************
 *
 * Function:    on_pushButton_clicked()
 *
 * Description: This function updates table Widget with
 *              Sub Analys Data from the database.
 *
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::updateTableWithSubAnalysData(QTableWidget *tableWidget,
                                                    SubAnalyseDataTypeET analyseType,
                                                    SubAnalysDataST *subAnalysDataArr,
                                                    int &nofArrData)
{
    CDbHndl db;
    bool res = false;
    SubAnalysDataST data;
    QFont font;

    // Set table widget font
    font.setPointSize(SAT_FONT_SIZE);
    font.setFamily(SAT_FONT_NAME);

    // Clear table widget
    tableWidget->clearContents();



    switch(analyseType)
    {
    case SAD_TOT_EARNINGS:
        res = db.getSubAnalysisTotEarningsData(m_stockName, m_stockSymbol, subAnalysDataArr, nofArrData);
        if(nofArrData > MAX_NOF_TOT_EARNINGS)
        {
            QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Error 1: Too many array data"));
        }
        break;


    case SAD_REVENUE:
        res = db.getSubAnalysisRevenueData(m_stockName, m_stockSymbol, subAnalysDataArr, nofArrData);
        if(nofArrData > MAX_NOF_REVENUE)
        {
            QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Error 1: Too many array data"));
        }
        break;

    case SAD_DIVIDEND:
        res = db.getSubAnalysisDividendData(m_stockName, m_stockSymbol, subAnalysDataArr, nofArrData);
        if(nofArrData > MAX_NOF_DIVIDEND_ARR_DATA)
        {
            QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Error 1: Too many array data"));
        }
        break;

    case SAD_EARNINGS:
        res = db.getSubAnalysisEarningsPerShareData(m_stockName, m_stockSymbol, subAnalysDataArr, nofArrData);
        if(nofArrData > MAX_NOF_EARNINGS_ARR_DATA)
        {
            QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Error 2: Too many array data"));
        }
        break;

    case SAD_TOTAL_CURRENT_ASSETS:
        res = db.getSubAnalysisTotalCurrentAssetsData(m_stockName, m_stockSymbol, subAnalysDataArr, nofArrData);
        if(nofArrData > MAX_NOF_TOTAL_CURRENT_ASSETS_ARR_DATA)
        {
            QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Error 3: Too many array data"));
        }
        break;

    case SAD_TOTAL_CURRENT_LIABILITIES:
        res = db.getSubAnalysisTotalCurrentLiabilitiesData(m_stockName, m_stockSymbol, subAnalysDataArr, nofArrData);
        if(nofArrData > MAX_NOF_TOTAL_CURRENT_LIABILITIES)
        {
            QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Error 4: Too many array data"));
        }
        break;

    case SAD_TOTAL_LIABILITIES:
        res = db.getSubAnalysisTotalLiabilitiesData(m_stockName, m_stockSymbol, subAnalysDataArr, nofArrData);
        if(nofArrData > MAX_NOF_TOTAL_LIABILITIES)
        {
            QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Error 5: Too many array data"));
        }
        break;

   case SAD_SOLIDITY:
        res = db.getSubAnalysisSolidityData(m_stockName, m_stockSymbol, subAnalysDataArr, nofArrData);
        if(nofArrData > MAX_NOF_SOLIDITY)
        {
            QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Error 6: Too many array data"));
        }
        break;

    case SAD_COVERAGE_RATIO:
        res = db.getSubAnalysisCoverageRatioData(m_stockName, m_stockSymbol, subAnalysDataArr, nofArrData);
        if(nofArrData > MAX_NOF_COVERAGE_RATIO)
        {
            QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Error 7: Too many array data"));
        }
        break;

    case SAD_CORE_TIER_1_RATIO:
        res = db.getSubAnalysisCoreTier1RatioData(m_stockName, m_stockSymbol, subAnalysDataArr, nofArrData);
        if(nofArrData > MAX_NOF_CORE_TIER_1_RATIO)
        {
            QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Error 7: Too many array data"));
        }
        break;

    case SAD_CORE_CAPITAL_RATIO:
        res = db.getSubAnalysisCoreCapitalRatioData(m_stockName, m_stockSymbol, subAnalysDataArr, nofArrData);
        if(nofArrData > MAX_NOF_CORE_CAPITAL_RATIO)
        {
            QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Error 7: Too many array data"));
        }
        break;

   case SAD_EQUITY:
        res = db.getSubAnalysisEquityData(m_stockName, m_stockSymbol, subAnalysDataArr, nofArrData);
        if(nofArrData > MAX_NOF_EQUITY)
        {
            QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Error 8: Too many array data"));
        }
        break;

    case SAD_EQUITY_PER_SHARE:
         res = db.getSubAnalysisEquityPerShareData(m_stockName, m_stockSymbol, subAnalysDataArr, nofArrData);
         if(nofArrData > MAX_NOF_EQUITY_PER_SHARE)
         {
             QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Error 8: Too many array data"));
         }
         break;

    case SAD_CASH_FLOW_CAPEX:
         res = db.getSubAnalysisCashFlowCapexData(m_stockName, m_stockSymbol, subAnalysDataArr, nofArrData);
         if(nofArrData > MAX_NOF_CASH_FLOW_CAPEX)
         {
             QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Error 9: Too many array data"));
         }
         break;


    case SAD_OPERATING_CASH_FLOW:
         res = db.getSubAnalysisOperatingCashFlowData(m_stockName, m_stockSymbol, subAnalysDataArr, nofArrData);
         if(nofArrData > MAX_NOF_OPERATING_CASH_FLOW)
         {
             QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Error 10: Too many array data"));
         }
         break;

    case SAD_TOTAL_DIVIDENT:
         res = db.getSubAnalysisTotDividendsData(m_stockName, m_stockSymbol, subAnalysDataArr, nofArrData);
         if(nofArrData > MAX_NOF_TOT_DIVIDENT)
         {
             QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Error 11: Too many array data"));
         }
         break;



    default:
        QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Error: invalid subAnalyseType"));
        return;
    }


    if(true == res)
    {
        for(int i = 0; i < nofArrData; i++)
        {
            data = subAnalysDataArr[i];
            tableWidget->insertRow(i);

            // Insert Year in table widget
            QTableWidgetItem *itemDate = new QTableWidgetItem( data.date);
            itemDate->setFont(font);
            tableWidget->setItem( i, SAT_COLUMN_DATE, itemDate);

            // Insert Data in table widget
            QTableWidgetItem *itemData = new QTableWidgetItem(data.data);
            itemData->setFont(font);
            tableWidget->setItem( i, SAT_COLUMN_DATA, itemData);
        }
    }
}



/******************************************************************
 *
 * Function:    insertTableWidgetIntrinsicValueTotCurrAssetDivTotLiabilities()
 *
 * Description: This function insert data in:
 *
 *              TableWidgetIntrinsicValueTotalCurrentAssetDivTotalLiabilities[]
 *
 *
 *
 *
 *****************************************************************/
bool StockAnalysisTab::
insertTableWidgetIntrinsicValueTotCurrAssetDivTotLiabilities(QTableWidget *tableWidget,
                                                             SubAnalysDataST *totalCurrentAssetArr,
                                                             int nofTotalCurrentAssetArrData,
                                                             SubAnalysDataST *totalLiabilities,
                                                             int nofTotalLiabilitiesArrData)
{
    int i;
    QString dataString;

    QFont font;

    // Set table widget font
    font.setPointSize(SAT_FONT_SIZE);
    font.setFamily(SAT_FONT_NAME);

    // Clear table widget
    tableWidget->clearContents();


    // Do both arrays have equal amount of data?
    if(nofTotalCurrentAssetArrData != nofTotalLiabilitiesArrData)
    {
        return false;
    }


    // Is data from same years?
    for(i = 0; i < nofTotalCurrentAssetArrData; i++)
    {
        if(totalCurrentAssetArr[i].date.toInt() != totalLiabilities[i].date.toInt())
        {
            return false;
        }

    }


    // Is data from same years?
    for(i = 0; i < nofTotalCurrentAssetArrData; i++)
    {
        if(totalLiabilities[i].data.toDouble() != 0)
        {
           dataString.sprintf("%.2f", totalCurrentAssetArr[i].data.toDouble()/totalLiabilities[i].data.toDouble());
        }
        else
        {
            // Do not divide by zero
            dataString.sprintf("%.2f", totalCurrentAssetArr[i].data.toDouble()/0.001);
        }


        // data = subAnalysDataArr[i];
        tableWidget->insertRow(i);

        // Insert Year in table widget
        QTableWidgetItem *itemDate = new QTableWidgetItem(totalCurrentAssetArr[i].date);
        itemDate->setFont(font);
        tableWidget->setItem( i, SAT_COLUMN_DATE, itemDate);

        // Insert Data in table widget
        QTableWidgetItem *itemData = new QTableWidgetItem(dataString);
        itemData->setFont(font);
        tableWidget->setItem( i, SAT_COLUMN_DATA, itemData);
    }

    return true;

}


/******************************************************************
 *
 * Function:    on_pushButton_clicked()
 *
 * Description: This function saves data to db
 *
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::on_pushButton_clicked()
{

//    CExtendedQwtPlot ceqp;
    CDbHndl db;
    bool res;
    int mainAnalysisId;
    int analysisDateId;
    QString str;
    HtmlStockAnalysPageDataST hSAPData;

    str = (QString::fromUtf8("Vill du lägga till data?\n"));
    str = str + m_stockName;
    str = str + ", ";
    str = str + m_stockSymbol;
    str = str + ", ";
    str = str + m_analysisDate;


    if(QMessageBox::No == QMessageBox::question(this, QString::fromUtf8("Uppdatera databas"), str, QMessageBox::Yes|QMessageBox::No))
    {
        return;
    }

    // Create directory so we are able to save anaysis images
    if(true == makeAnalysPlotDirectory(m_stockName, m_analysisDate))
    {
       saveAnalysisPlotAsImages();
    }

    // Check if this stocksymbol and stockname is already added
    // If not add it
    res = db.mainAnalysisDataExists(m_stockName,
                                m_stockSymbol,
                                mainAnalysisId);

    hSAPData.stockName = m_stockName;
    hSAPData.stockSymbol = m_stockSymbol;
    hSAPData.analysisDate = m_analysisDate;


    if(false == res)
    {
        res = db.insertMainAnalysisData(m_stockName,
                                     m_stockSymbol,
                                     mainAnalysisId);
    }


    if(true == res)
    {
        res = db.mainAnalysisDateExists(m_analysisDate,
                                     mainAnalysisId,
                                     analysisDateId);

    }

    // Exist anaysis date?
    if(false == res)
    {
        res = db.insertMainAnalysisDate(m_analysisDate,
                                     mainAnalysisId,
                                     analysisDateId);

    }


    if(true == res)
    {
        hSAPData.companyDescription = ui->textEditCompDescription->toPlainText();

        hSAPData.bigEnoughAnswer = ui->lineEditBigEnoughAnswer->text();
        hSAPData.bigEnoughComment = ui->textEditBigEnoughText->toPlainText();

        hSAPData.strongFinancialPositionAnswer = ui->lineEditFinancialStrongAnswer->text();
        hSAPData.strongFinancialPositionComment = ui->textEditFinancialStrongText->toPlainText();

        hSAPData.earningStabilityAnswer = ui->lineEditErningStabilityAnswer->text();
        hSAPData.earningStabilityComment = ui->textEditErningStabilityText->toPlainText();

        hSAPData.dividendStabilityAnswer = ui->lineEditDividentStabilityAnswer->text();
        hSAPData.dividendStabilityComment =ui->textEditDividentStabilityText->toPlainText();

        hSAPData.earningGrowthAnswer = ui->lineEditErningGrowthAnswer->text();
        hSAPData.earningGrowthComment = ui->textEditErningGrowthText->toPlainText();


        hSAPData.keyValuePe = ui->lineEditPE->text();
        hSAPData.keyValuePs = ui->lineEditPs->text();
        hSAPData.keyValueNavPriceRatio = ui->lineEditNavDivLastStockPrice->text();
        hSAPData.keyValueYield = ui->lineEditYield->text();
        hSAPData.keyValueCurrentRatio = ui->lineEditCurrentRatio->text();
        hSAPData.keyValueTotalDebtEquityRatio = ui->lineEditTotDebtEquityRatio->text();

        // Saknas m_keyValuePriceJEKRatio;
        hSAPData.keyValueErningsDividentRatio = ui->lineEditEarningsDivByDividend->text();

        hSAPData.trustworthyLeadershipAnswer = ui->lineEditTrustworthyManagementAnswer->text();
        hSAPData.trustworthyLeadershipComment =  ui->textEditTrustworthyManagementText->toPlainText();

        hSAPData.goodOwnershipAnswer = ui->lineEditBeneficialOwnershipAnswer->text();
        hSAPData.goodOwnershipComment = ui->textEditBeneficialOwnershipText->toPlainText();

        hSAPData.otherInformation = ui->textEditOtherInfo->toPlainText();

        hSAPData.riskStdDev = m_riskStdDev;
        hSAPData.meanReturns = m_meanReturns;

        // IntrinsicValue
        hSAPData.tenYearNoteYield = ui->lineEditTenYearNoteYield->text();
        hSAPData.currEquityPerShare = ui->lineEditCurrEquityPerShare->text();
        hSAPData.estimateYearlyDividend = ui->lineEditEstimateYearlyDividend->text();
        hSAPData.calcIntrinsicValue = ui->lineEditIntrinsicValueResult->text();
        hSAPData.intrinsicValueYearSpan = m_equityPerShareArr[0].date;
        hSAPData.intrinsicValueYearSpan += " - ";
        hSAPData.intrinsicValueYearSpan += m_equityPerShareArr[m_nofEquityPerShareData-1].date;
        hSAPData.historicalYearlyInterestOnEquity = ui->lineEditHistoricalInterestOnEquity->text();


        int inputAnalysisDataId;
        int analysisDataId;
        bool inputAnalysisDataIdIsValid = false;

        if( true == db.getAnalysisDataId(mainAnalysisId, analysisDateId, inputAnalysisDataId))
        {
           //QString responsStr;
           inputAnalysisDataIdIsValid = true;

           //responsStr.sprintf("Data finns redan analysisDataId = %d", inputAnalysisDataId);
           //QMessageBox::information(NULL, QString::fromUtf8("Uppdatera databas"), responsStr);
           //return;

        }


        res = db.insertAnalysisData(analysisDateId,
                                    mainAnalysisId,
                                    inputAnalysisDataId,
                                    inputAnalysisDataIdIsValid,
                           hSAPData.companyDescription,
                           hSAPData.bigEnoughAnswer,
                           hSAPData.bigEnoughComment,
                           hSAPData.strongFinancialPositionAnswer,
                           hSAPData.strongFinancialPositionComment,
                           hSAPData.earningStabilityAnswer,
                           hSAPData.earningStabilityComment,
                           hSAPData.dividendStabilityAnswer,
                           hSAPData.dividendStabilityComment,
                           hSAPData.earningGrowthAnswer,
                           hSAPData.earningGrowthComment,
                           hSAPData.keyValuePe,
                           hSAPData.keyValuePs,
                           hSAPData.keyValueNavPriceRatio,
                           hSAPData.keyValueYield,
                           hSAPData.keyValuePriceJEKRatio,
                           hSAPData.keyValueErningsDividentRatio,
                           hSAPData.keyValueTotalDebtEquityRatio,
                           hSAPData.keyValueCurrentRatio,
                           hSAPData.trustworthyLeadershipAnswer,
                           hSAPData.trustworthyLeadershipComment,
                           hSAPData.goodOwnershipAnswer,
                           hSAPData.goodOwnershipComment,
                           hSAPData.otherInformation,

                           hSAPData.tenYearNoteYield,
                           hSAPData.currEquityPerShare,
                           hSAPData.estimateYearlyDividend,
                           hSAPData.calcIntrinsicValue,
                           hSAPData.intrinsicValueYearSpan,
                           hSAPData.historicalYearlyInterestOnEquity,

                           analysisDataId);
    }

    // Save excel data to file
    writeHtmlArrDataToTxtFile();

    // Display all analysis graphs
    displayAllAnalysisPlots();


}



/******************************************************************
 *
 * Function:        writeHtmlArrDataToTxtFile()
 *
 * Description:     This function writes table data displayed
 *                  in html report to text file. So it can
 *                  be displayed as graph in Excel.
 *
 *
 *****************************************************************/
void StockAnalysisTab::writeHtmlArrDataToTxtFile(void)
{
    CUtil cu;
    QString str;
    QString filename;
    double tmpRes;


    m_fildata.clear();

    // Create header in file and part of filename
    str = str + m_stockName;
    str = str + "_";
    str = str + m_stockSymbol;
    str = str + "_";
    str = str + m_analysisDate;

    m_fildata = str;
    m_fildata = "|\n";

    // Create file extention
    filename = str + ".txt";



    //-------------------------------------------------------------------
    // Stark finansiell ställning:
    //-------------------------------------------------------------------


    //=======================================================================
    // Omsättningstillgångarna / Kortfristiga skulder > 2
    //=======================================================================
    if((m_nofTotalCurrentAssetsArrData == m_nofTotalCurrentLiabilitiesData) &&
       (m_nofTotalCurrentAssetsArrData > 0))
    {
        m_fildata += QString::fromUtf8("Omsättningstillgångarna / Kortfristiga skulder > 2||'O/KS'|\n");

        for(int ii = 0; ii < m_nofTotalCurrentAssetsArrData; ii++)
        {
            if(m_totalCurrentAssetsArr[ii].date.toInt() == m_totalCurrentLiabilitiesArr[ii].date.toInt())
            {
                // Add Year
                m_fildata +=  "|";
                m_fildata += m_totalCurrentAssetsArr[ii].date;
                m_fildata +=  "|";

                if(m_totalCurrentLiabilitiesArr[ii].data.toDouble() != 0)
                {
                    //tmpRes = (m_totalCurrentAssetsArr[ii].data.toDouble() - m_totalCurrentLiabilitiesArr[ii].data.toDouble())/m_totalCurrentLiabilitiesArr[ii].data.toDouble();
                    tmpRes = (m_totalCurrentAssetsArr[ii].data.toDouble() / m_totalCurrentLiabilitiesArr[ii].data.toDouble());
                }
                else
                {
                    tmpRes = m_totalCurrentAssetsArr[ii].data.toDouble() / 0.001;
                }

                // Add data
                //tmpRes = 1.0 + tmpRes;
                str = str.sprintf("%.2f", tmpRes);
                m_fildata +=  str;
                m_fildata +=  "|\n";
           }
        }
        // Make space for graph in excel file
        for(int kk = 0; kk < 15; kk++)
        {
            m_fildata +=  "|\n";
        }
    }


    //=======================================================================
    // Omsättningstillgångarna	/ Totala skulder >= 1
    //=======================================================================
    if((m_nofTotalCurrentAssetsArrData == m_nofTotalLiabilitiesData) &&
       (m_nofTotalCurrentAssetsArrData > 0))
    {
        m_fildata += QString::fromUtf8("Omsättningstillgångarna / Totala skulder >= 1||'O/TS'|\n");

        for(int ii = 0; ii < m_nofTotalCurrentAssetsArrData; ii++)
        {
            if(m_totalCurrentAssetsArr[ii].date.toInt() == m_totalLiabilitiesArr[ii].date.toInt())
            {
                // Add Year
                m_fildata +=  "|";
                m_fildata += m_totalCurrentAssetsArr[ii].date;
                m_fildata +=  "|";

                if(m_totalLiabilitiesArr[ii].data.toDouble() != 0)
                {
                    // tmpRes = (m_totalCurrentAssetsArr[ii].data.toDouble() - m_totalLiabilitiesArr[ii].data.toDouble()) / m_totalCurrentLiabilitiesArr[ii].data.toDouble();
                    tmpRes = (m_totalCurrentAssetsArr[ii].data.toDouble() / m_totalLiabilitiesArr[ii].data.toDouble());
                }
                else
                {
                    tmpRes = m_totalCurrentAssetsArr[ii].data.toDouble() / 0.001;
                }

                // tmpRes = 1.0 + tmpRes;

                QString tmpDb;
                tmpDb = tmpDb.sprintf("%.2f", tmpRes);

                // Add data to file
                m_fildata += tmpDb;
                m_fildata +=  "|\n";

            }
        }
        // Make space for graph in excel file
        for(int kk = 0; kk < 15; kk++)
        {
            m_fildata +=  "|\n";
        }
    }




    //====================================================================
    // Totala skulder / Eget kapital < 2
    //====================================================================
    if(m_nofTotalLiabilitiesData > 0 && m_nofTotalLiabilitiesData ==  m_nofTotEquityData)
    {
        m_fildata += QString::fromUtf8("Totala skulder / Eget kapital< 2||'TS/EK'|\n");
        for(int ii = 0; ii < m_nofTotalLiabilitiesData; ii++)
        {
            if((m_totEquityArr[ii].date.toInt() == m_totalLiabilitiesArr[ii].date.toInt()))
            {
                // Add Year
                m_fildata +=  "|";
                m_fildata += m_totEquityArr[ii].date;
                m_fildata +=  "|";


                QString tmpStr;
                double realEquity = m_totEquityArr[ii].data.toDouble();
                double realTotalLiabilities = m_totalLiabilitiesArr[ii].data.toDouble();
                double ratio;
                if(realEquity == 0)
                {
                    realEquity = 0.001;
                    ratio = realTotalLiabilities / realEquity;
                }
                if(realEquity < 0)
                {
                    ratio = realTotalLiabilities / realEquity;
                    if(ratio > 0)
                    {
                        ratio = -ratio;
                    }
                }
                else
                {
                    ratio = realTotalLiabilities / realEquity;
                }

                tmpStr = tmpStr.sprintf("%.2f", ratio);

                // Add data
                m_fildata += tmpStr;
                m_fildata +=  "|\n";

            }
        }

        // Make space for graph in excel file
        for(int kk = 0; kk < 15; kk++)
        {
            m_fildata +=  "|\n";
        }
    }



#if 0
    //====================================================================
    // Soliditet >= 40%
    //====================================================================
    if(m_nofSolidityData > 0)
    {
        m_fildata += QString::fromUtf8("Soliditet >= 40%||S|\n");


        for(int ii = 0; ii < m_nofSolidityData; ii++)
        {
            // Add Year
            m_fildata +=  "|";
            m_fildata += m_solidityArr[ii].date;
            m_fildata +=  "|";

            // Add data
            QString tmpDb;
            tmpDb.sprintf("%.2f", (m_solidityArr[ii].data.toDouble() * 100.0));
            m_fildata += tmpDb;
            m_fildata +=  "|\n";
        }

        // Make space for graph in excel file
        for(int kk = 0; kk < 15; kk++)
        {
            m_fildata +=  "|\n";
        }

    }
#endif




    //====================================================================
    // Soliditet >= 40%
    //====================================================================
    if(m_nofSolidityData > 9)
    {
        m_fildata += QString::fromUtf8("Soliditet >= 40%||S|\n");

        for(int ii = 0; ii < m_nofSolidityData; ii++)
        {
            // Add Year
            m_fildata +=  "|";
            m_fildata += m_solidityArr[ii].date;
            m_fildata +=  "|";

            // Add data
            QString tmpDb;
            tmpDb.sprintf("%.2f", (m_solidityArr[ii].data.toDouble() * 100.0));
            m_fildata += tmpDb;
            m_fildata +=  "|\n";
        }

        // Make space for graph in excel file
        for(int kk = 0; kk < 15; kk++)
        {
            m_fildata +=  "|\n";
        }

    }


    //====================================================================
    // Räntetäckningsgraden >= 3
    //====================================================================
    if(m_nofCoverageRatioData > 0)
    {
        m_fildata += QString::fromUtf8("Räntetäckningsgraden >= 3||R|\n");

        for(int ii = 0; ii < m_nofCoverageRatioData; ii++)
        {
            // Add Year
            m_fildata +=  "|";
            m_fildata += m_coverageRatioArr[ii].date;
            m_fildata +=  "|";

            // Add data
            m_fildata += m_coverageRatioArr[ii].data;
            m_fildata +=  "|\n";
        }

        // Make space for graph in excel file
        for(int kk = 0; kk < 15; kk++)
        {
            m_fildata +=  "|\n";
        }

    }


#if 0
    //====================================================================
    // Totala skulder / Eget kapital< 2
    //====================================================================
    if(m_nofTotalLiabilitiesData > 0 && m_nofTotalLiabilitiesData ==  m_nofTotEquityData)
    {
        m_fildata += QString::fromUtf8("Totala skulder / Eget kapital< 2||'TS/EK'|\n");
        for(int ii = 0; ii < m_nofTotalLiabilitiesData; ii++)
        {
            if((m_totEquityArr[ii].date.toInt() == m_totalLiabilitiesArr[ii].date.toInt()))
            {
                // Add Year
                m_fildata +=  "|";
                m_fildata += m_totEquityArr[ii].date;
                m_fildata +=  "|";


                QString tmpStr;
                double realEquity = m_totEquityArr[ii].data.toDouble();
                double realTotalLiabilities = m_totalLiabilitiesArr[ii].data.toDouble();
                double ratio;
                if(realEquity == 0)
                {
                    realEquity = 0.001;
                    ratio = realTotalLiabilities / realEquity;
                }
                if(realEquity < 0)
                {
                    ratio = realTotalLiabilities / realEquity;
                    if(ratio > 0)
                    {
                        ratio = -ratio;
                    }
                }
                else
                {
                    ratio = realTotalLiabilities / realEquity;
                }

                tmpStr = tmpStr.sprintf("%.2f", ratio);

                // Add data
                m_fildata += tmpStr;
                m_fildata +=  "|\n";

            }
        }

        // Make space for graph in excel file
        for(int kk = 0; kk < 15; kk++)
        {
            m_fildata +=  "|\n";
        }

    }
#endif

    //====================================================================
    // Kärnprimärkapitalrelation > 15%
    //====================================================================
    if(m_nofCoreCapitalRatioData > 0)
    {
        m_fildata += QString::fromUtf8("Kärnprimärkapitalrelation > 15%||K|\n");

        for(int ii = 0; ii < m_nofCoreCapitalRatioData; ii++)
        {
            // Add Year
            m_fildata +=  "|";
            m_fildata += m_coreCapitalRatioArr[ii].date;
            m_fildata +=  "|";

            // Add data to file
            m_fildata += m_coreCapitalRatioArr[ii].data;
            m_fildata +=  "|\n";
        }

        // Make space for graph in excel file
        for(int kk = 0; kk < 15; kk++)
        {
            m_fildata +=  "|\n";
        }
    }



    //-------------------------------------------------------------------
    // Intjäningsstabilitet:
    //-------------------------------------------------------------------

    //====================================================================
    // Vinst/Aktie,
    //====================================================================
    if(m_nofEarningsArrData > 0)
    {
        m_fildata += QString::fromUtf8("Vinst/Aktie||V|\n");
        for(int ii = 0; ii < m_nofEarningsArrData; ii++)
        {

            // Add Year
            m_fildata +=  "|";
            m_fildata += m_earningsDataArr[ii].date;
            m_fildata +=  "|";

            // Add data
            m_fildata += m_earningsDataArr[ii].data;
            m_fildata +=  "|\n";
        }

        // Make space for graph in excel file
        for(int kk = 0; kk < 15; kk++)
        {
            m_fildata +=  "|\n";
        }
    }


    //====================================================================
    //  Omsättning, Vinst efter skatt
    //====================================================================
    if(m_nofTotEarningsArrData > 0 && (m_nofRevenueArrData == m_nofTotEarningsArrData))
    {
        m_fildata += QString::fromUtf8("Omsättning, Vinst efter skatt|År|O|V|\n");
        for(int ii = 0; ii < m_nofTotEarningsArrData; ii++)
        {
            if((m_revenueDataArr[ii].date.toInt() == m_totEarningsDataArr[ii].date.toInt()))
            {
                // Add Year
                m_fildata +=  "|";
                m_fildata += m_revenueDataArr[ii].date;
                m_fildata +=  "|";

                // Add data
                QString tmpStr;
                tmpStr = tmpStr.sprintf("%.2f", m_revenueDataArr[ii].data.toDouble());
                m_fildata += tmpStr;
                m_fildata +=  "|";

                tmpStr = tmpStr.sprintf("%.2f", m_totEarningsDataArr[ii].data.toDouble());
                m_fildata += tmpStr;
                m_fildata +=  "|\n";
            }
        }

        // Make space for graph in excel file
        for(int kk = 0; kk < 15; kk++)
        {
            m_fildata +=  "|\n";
        }

    }


    //-------------------------------------------------------------------
    // Utdelningsstabilitet:
    //-------------------------------------------------------------------


    //====================================================================
    // Utdelning
    //====================================================================
    if((m_nofDividendArrData > 0))
    {
        m_fildata += QString::fromUtf8("Utdelning||U|\n");

        for(int ii = 0; ii < m_nofDividendArrData; ii++)
        {
            // Add date
            m_fildata +=  "|";
            m_fildata += m_dividendDataArr[ii].date;
            m_fildata +=  "|";

            // Add data
            QString div;
            div.sprintf("%.2f", m_dividendDataArr[ii].data.toDouble());

            m_fildata += div;
            m_fildata +=  "|\n";
        }
        // Make space for graph in excel file
        for(int kk = 0; kk < 15; kk++)
        {
            m_fildata +=  "|\n";
        }
    }



    //====================================================================
    // Vinst/Utdelning
    //====================================================================
    if((m_nofEarningsArrData > 0) && (m_nofDividendArrData > 0))
    {
        m_fildata += QString::fromUtf8("Vinst/Utdelning||'V/U'|\n");

        for(int ii = 0; ii < m_nofDividendArrData; ii++)
        {
	        for(int jj = 0; jj < m_nofEarningsArrData; jj++)
	        {
	            if(m_dividendDataArr[ii].date.toInt() == m_earningsDataArr[jj].date.toInt())
	            {
	                // Add date
	                m_fildata +=  "|";
	                m_fildata += m_dividendDataArr[ii].date;
	                m_fildata +=  "|";

	                // Add data
	                QString res;
	                res.sprintf("%.2f", (m_earningsDataArr[jj].data.toDouble() / m_dividendDataArr[ii].data.toDouble()));

	                m_fildata += res;
	                m_fildata +=  "|\n";
	                break;
	            }
	        }
        }

        // Make space for graph in excel file
        for(int kk = 0; kk < 15; kk++)
        {
            m_fildata +=  "|\n";
        }
    }

    //==================================================================
    //  Operativt kassaflöde - CAPEX - Utdelning > 0
    //==================================================================
    if((m_nofTotDividensData > 0) &&
       (m_nofTotDividensData == m_nofCashFlowCapexData) &&
       (m_nofTotDividensData == m_nofCashFlowCapexData))
    {
        double res;
        QString resStr;

        m_fildata += QString::fromUtf8("Operativt kassaflöde - CAPEX - Utdelning > 0||OK-CA-UT|\n");


        for(int i = 0; i < m_nofTotDividensData; i++)
        {
            // Add Year
            m_fildata +=  "|";
            m_fildata += m_cashFlowCapexArr[i].date;;
            m_fildata +=  "|";

            // cashFlowCapexArr totDividensArr is stored as negative numbers
            res = m_operatingCashFlowArr[i].data.toDouble() +
                  m_cashFlowCapexArr[i].data.toDouble() +
                  m_totDividensArr[i].data.toDouble();

            resStr.sprintf("%.2f", res);

            // Add data to file
            m_fildata += resStr;
            m_fildata +=  "|\n";
        }

        // Make space for graph in excel file
        for(int kk = 0; kk < 15; kk++)
        {
            m_fildata +=  "|\n";
        }
    }



    //==================================================================
    //  Operativt kassaflöde| CAPEX| Utdelning|
    //==================================================================
    if((m_nofTotDividensData > 0) &&
       (m_nofTotDividensData == m_nofCashFlowCapexData) &&
       (m_nofTotDividensData == m_nofCashFlowCapexData))
    {

        m_fildata += QString::fromUtf8("Operativt kassaflöde, CAPEX, Utdelning||OK|CA|UT|\n");


        for(int i = 0; i < m_nofTotDividensData; i++)
        {
            // Add Year
            m_fildata +=  "| ";
            m_fildata += m_cashFlowCapexArr[i].date;;
            m_fildata +=  "|";

            // Add data
            str = str.sprintf("%.0f", m_operatingCashFlowArr[i].data.toDouble());
            m_fildata += str;
            m_fildata +=  "|";

            // Add data
            if(m_cashFlowCapexArr[i].data.toDouble() < 0)
            {
                str = str.sprintf("%.0f", -m_cashFlowCapexArr[i].data.toDouble());
            }
            else
            {
                str =  m_cashFlowCapexArr[i].data;
            }

            m_fildata += str;
            m_fildata +=  "|";

            // Add data
            if(m_totDividensArr[i].data.toDouble() < 0)
            {
                str = str.sprintf("%.0f", -m_totDividensArr[i].data.toDouble());
            }
            else
            {
                str = m_totDividensArr[i].data;
            }

            m_fildata += str;
            m_fildata +=  "|\n";
        }

        // Make space for graph in excel file
        for(int kk = 0; kk < 15; kk++)
        {
            m_fildata +=  "|\n";
        }
    }





    //-------------------------------------------------------------------
    // Intjäningstillväxt:
    //-------------------------------------------------------------------

    //====================================================================
    // Vinsttillväxt
    //====================================================================
    QString earningsGrowthStr;
    double earningsGrowth = 0;
    bool negativeSign = false;
    double currEarnings;
    double lastEarnings;

    if(m_nofEarningsArrData > 0)
    {
        m_fildata += QString::fromUtf8("Vinsttillväxt (%)||Vt|\n");

        for(int ii = 0; ii < m_nofEarningsArrData; ii++)
        {

            // Add Year
            m_fildata +=  "|";
            m_fildata += m_earningsDataArr[ii].date;
            m_fildata +=  "|";

            negativeSign = false;
            if(ii > 0)
            {
                currEarnings = m_earningsDataArr[ii].data.toDouble();
                lastEarnings = m_earningsDataArr[ii-1].data.toDouble();

                if(currEarnings < 0)
                {
                    negativeSign = true;
                }


                // Handle divide by zero
                if(lastEarnings == 0)
                {
                    earningsGrowth = currEarnings / 0.001;
                }
                else
                {
                    earningsGrowth = (currEarnings - lastEarnings) / lastEarnings;
                }


                // Change sign if negative growth
                if( ((negativeSign == true) && (earningsGrowth > 0)) || ((currEarnings > 0) && (lastEarnings < 0)) )
                {
                    earningsGrowth = -earningsGrowth;
                }

                earningsGrowth = earningsGrowth * 100;
                earningsGrowthStr.sprintf("%.2f",  earningsGrowth);
            }
            else
            {
                earningsGrowthStr.clear();
            }

            // Add data
            m_fildata += earningsGrowthStr;
            m_fildata +=  "|\n";
        }

        // Make space for graph in excel file
        for(int kk = 0; kk < 15; kk++)
        {
            m_fildata +=  "|\n";
        }
    }



    //====================================================================
    // Vinstmarginal (Vinst efter skatt / Omsättning)
    //====================================================================
    if(m_nofTotEarningsArrData > 0 && (m_nofRevenueArrData == m_nofTotEarningsArrData))
    {
        m_fildata += QString::fromUtf8("Vinstmarginal (%) (Vinst efter skatt / Omsättning)||'V/O'|\n");
        for(int ii = 0; ii < m_nofTotEarningsArrData; ii++)
        {
            if((m_revenueDataArr[ii].date.toInt() == m_totEarningsDataArr[ii].date.toInt()))
            {
                // Add Year
                m_fildata +=  "|";
                m_fildata += m_revenueDataArr[ii].date;
                m_fildata +=  "|";


                QString tmpStr;
                double realRevenue = m_revenueDataArr[ii].data.toDouble();
                double realEarnings = m_totEarningsDataArr[ii].data.toDouble();
                double ratio;

                if(realRevenue == 0)
                {
                    realRevenue = 0.001;
                    ratio = realEarnings / realRevenue * 100.0;
                }

                if(realRevenue < 0 || (realEarnings < 0))
                {
                    ratio = realEarnings / realRevenue * 100.0;
                    if(ratio > 0)
                    {
                        ratio = -ratio;
                    }
                }
                else
                {
                    ratio = realEarnings/ realRevenue * 100.0;
                }

                tmpStr = tmpStr.sprintf("%.2f", ratio);

                // Add data
                m_fildata += tmpStr;
                m_fildata +=  "|\n";
            }
        }

        // Make space for graph in excel file
        for(int kk = 0; kk < 15; kk++)
        {
            m_fildata +=  "|\n";
        }
    }




    //====================================================================
    // Avkastning på det egna kapitalet (Vinst efter skatt / Eget kapital)
    //====================================================================
    if(m_nofTotEarningsArrData > 0 && (m_nofTotEarningsArrData == m_nofTotEquityData))
    {
        m_fildata += QString::fromUtf8("Avkastning på det egna kapitalet (%) [Vinst/Eget kapital]||'V/EK'|\n");
        for(int ii = 0; ii < m_nofTotEarningsArrData; ii++)
        {
            if((m_totEquityArr[ii].date.toInt() == m_totEarningsDataArr[ii].date.toInt()))
            {
                // Add Year
                m_fildata +=  "|";
                m_fildata += m_totEquityArr[ii].date;
                m_fildata +=  "|";


                QString tmpStr;
                double realEquity = m_totEquityArr[ii].data.toDouble();
                double realEarnings = m_totEarningsDataArr[ii].data.toDouble();
                double ratio;

                if(realEquity == 0)
                {
                    realEquity = 0.001;
                    ratio = realEarnings / realEquity * 100.0;
                }

                if(realEquity < 0 || (realEarnings < 0))
                {
                    ratio = realEarnings / realEquity * 100.0;
                    if(ratio > 0)
                    {
                        ratio = -ratio;
                    }
                }
                else
                {
                    ratio = realEarnings/ realEquity * 100.0;
                }


                tmpStr = tmpStr.sprintf("%.2f", ratio);

                // Add data
                m_fildata += tmpStr;
                m_fildata +=  "|\n";
            }
        }

        // Make space for graph in excel file
        for(int kk = 0; kk < 15; kk++)
        {
            m_fildata +=  "|\n";
        }
    }


    //====================================================================
    // Totala skulder Eget kapital, Vinst, Utdelning
    //====================================================================
    if((m_nofEarningsArrData > 0) && (m_nofTotDividensData > 0))
    {
        bool earningsIsFound;
        bool totalLiabilitiesIsFound;
        bool equityIsFound;

        int jj;
        int kk;
        int nn;

        qDebug() << QString::fromUtf8("Totala skulder Eget kapital, Vinst, Utdelning|");;
        m_fildata += QString::fromUtf8("Totala Skulder, Eget kapital, Vinst, Utdelning|År|TS|EK|V|UT|\n");

        // Loop through all dividend
        for(int ii = 0; ii < m_nofTotDividensData; ii++)
        {
            earningsIsFound = false;
            totalLiabilitiesIsFound = false;
            equityIsFound = false;


            // Check if we have earnings from same year
            for(jj = 0; jj < m_nofTotEarningsArrData; jj++)
            {
                if(m_totDividensArr[ii].date.toInt() == m_totEarningsDataArr[jj].date.toInt())
                {
                    qDebug() << "divYear" << m_totDividensArr[ii].date;
                    qDebug() << "earYear" << m_totEarningsDataArr[jj].date;

                    qDebug() << ii << "div" << m_totDividensArr[ii].data;
                    qDebug() << jj << "ear" << m_totEarningsDataArr[jj].data;

                    earningsIsFound = true;
                    break;
                }
            }


            // Check if we have Equity from same year
            for(kk = 0; kk < m_nofTotEquityData; kk++)
            {
                if(m_totDividensArr[ii].date.toInt() == m_totEquityArr[kk].date.toInt())
                {
                    qDebug() << "divYear" << m_totDividensArr[ii].date;
                    qDebug() << "euqYear" << m_totEquityArr[kk].date;


                    qDebug() << kk << "equ" << m_totEquityArr[kk].data;
                    equityIsFound = true;
                    break;
                }
            }

            // Check if we have Total Liabilities from same year
            for(nn = 0; nn < m_nofTotalLiabilitiesData; nn++)
            {
                if(m_totDividensArr[ii].date.toInt() == m_totalLiabilitiesArr[nn].date.toInt())
                {
                    qDebug() << "divYear" << m_totDividensArr[ii].date;
                    qDebug() << "totalLiaYear" << m_totalLiabilitiesArr[kk].date;


                    qDebug() << nn << "totLia" << m_totalLiabilitiesArr[nn].data;
                    totalLiabilitiesIsFound = true;
                    break;
                }
            }


            // If all data are found for this year then wite to file.
            if((earningsIsFound == true) && (equityIsFound == true) && (totalLiabilitiesIsFound == true))
            {
                QString res;

                qDebug() << ii << "divYear" << m_totDividensArr[ii].date;
                qDebug() << nn << "totalLia" << m_totalLiabilitiesArr[nn].data;
                qDebug() << kk << "equYear" << m_totEquityArr[kk].data;
                qDebug() << jj << "totEar" << m_totEarningsDataArr[jj].data;
                qDebug() << ii << "totDiv" << m_totDividensArr[ii].data;

                // Add Year
                m_fildata +=  "|";
                m_fildata += m_totDividensArr[ii].date;
                m_fildata +=  "|";

                // Add total liabilities
                res = res.sprintf("%.2f", m_totalLiabilitiesArr[nn].data.toDouble());
                m_fildata += res;
                m_fildata +=  "|";

                // Add equity
                res = res.sprintf("%.2f", m_totEquityArr[kk].data.toDouble());
                m_fildata += res;
                m_fildata +=  "|";


                // Add earnings
                res = res.sprintf("%.2f", m_totEarningsDataArr[jj].data.toDouble());
                m_fildata += res;
                m_fildata +=  "|";

                // Add divident
                res = res.sprintf("%.2f", -m_totDividensArr[ii].data.toDouble());
                m_fildata += res;
                m_fildata +=  "|\n";
            }
        }
    }





    // Save data
    if(m_fildata.length() > 0)
    {
        QString path = qApp->applicationDirPath();
        path += "/database/AnalysDoc/htmlReports/Excel";

        cu.saveTextFile(m_fildata, filename, path);
        // m_fildata.clear();
    }
}


/******************************************************************
 *
 * Function:    resetStockAnalysisData()
 *
 * Description:
 *
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::resetStockAnalysisData(HtmlStockAnalysPageDataST &hSAPData)
{
    hSAPData.companyDescription.clear();
    //m_BigCompDescription.clear();
    hSAPData.bigEnoughAnswer.clear();
    hSAPData.bigEnoughComment.clear();
    hSAPData.strongFinancialPositionAnswer.clear();
    hSAPData.strongFinancialPositionComment.clear();
    hSAPData.earningStabilityAnswer.clear();
    hSAPData.earningStabilityComment.clear();
    hSAPData.dividendStabilityAnswer.clear();
    hSAPData.dividendStabilityComment.clear();
    hSAPData.earningGrowthAnswer.clear();
    hSAPData.earningGrowthComment.clear();
    hSAPData.keyValuePe.clear();
    hSAPData.keyValuePs.clear();
    hSAPData.keyValueNavPriceRatio.clear();
    hSAPData.keyValueYield.clear();
    hSAPData.keyValuePriceJEKRatio.clear();
    hSAPData.keyValueErningsDividentRatio.clear();
    hSAPData.keyValueTotalDebtEquityRatio.clear();
    hSAPData.keyValueCurrentRatio.clear();
    hSAPData.trustworthyLeadershipAnswer.clear();
    hSAPData.trustworthyLeadershipComment.clear();
    hSAPData.goodOwnershipAnswer.clear();
    hSAPData.goodOwnershipComment.clear();
    hSAPData.otherInformation.clear();
    hSAPData.riskStdDev.clear();
    hSAPData.meanReturns.clear();
    hSAPData.meanReturns.clear();

    hSAPData.nofCoreCapitalRatioData = 0;
    hSAPData.nofCoreTier1RatioData = 0;
    hSAPData.nofCoverageRatioData = 0;
    hSAPData.nofDividendArrData = 0;
    hSAPData.nofEarningsArrData = 0;
    hSAPData.nofEquityData = 0;
    hSAPData.nofSolidityData = 0;
    hSAPData.nofTotalCurrentAssetsArrData = 0;
    hSAPData.nofTotalCurrentLiabilitiesData = 0;
    hSAPData.nofTotalLiabilitiesData = 0;

    // Intrinsic value
    hSAPData.tenYearNoteYield.clear();
    hSAPData.currEquityPerShare.clear();
    hSAPData.estimateYearlyDividend.clear();
    hSAPData.calcIntrinsicValue.clear();
    hSAPData.intrinsicValueYearSpan.clear();
    hSAPData.historicalYearlyInterestOnEquity.clear();

}


/******************************************************************
 *
 * Function:    on_treeWidgetAnalysisDate_doubleClicked()
 *
 * Description: This function use date to get stock analysis data
 *              from the database.
 *
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::on_treeWidgetAnalysisDate_doubleClicked(const QModelIndex &index)
{

    createStockAnalysisHtmlPage csaHtmlPg;
    QString analysisDate;
    CDbHndl db;
    HtmlStockAnalysPageDataST hSAPData;



    m_gfc.getSelStockAnalysisDateItem(ui->treeWidgetAnalysisDate,
                                      analysisDate,
                                      index);

    if(analysisDate.size() < 1)
    {
        return;
    }

    // Main analys page
    m_analysisDate = analysisDate;
    hSAPData.analysisDate = analysisDate;
    ui->analysisDateLineEdit->setText(m_analysisDate);

    // Page analysdata 1
    ui->ananlysisDateLabel_32->setText(m_analysisDate);

    // Page analysdata 2
    ui->ananlysisDateLabel_33->setText(m_analysisDate);

    resetStockAnalysisData(hSAPData);
    resetGuiCtrl();
    clearGUIIntrinsicValue();


    hSAPData.stockName = m_stockName;
    hSAPData.stockSymbol = m_stockSymbol;
    hSAPData.analysisDate = m_analysisDate;


    db.getStockAnalysisData(hSAPData.stockName,
                         hSAPData.stockSymbol,
                         hSAPData.analysisDate,
                         hSAPData.companyDescription,
                         hSAPData.bigEnoughAnswer,
                         hSAPData.bigEnoughComment,
                         hSAPData.strongFinancialPositionAnswer,
                         hSAPData.strongFinancialPositionComment,
                         hSAPData.earningStabilityAnswer,
                         hSAPData.earningStabilityComment,
                         hSAPData.dividendStabilityAnswer,
                         hSAPData.dividendStabilityComment,
                         hSAPData.earningGrowthAnswer,
                         hSAPData.earningGrowthComment,
                         hSAPData.keyValuePe,
                         hSAPData.keyValuePs,
                         hSAPData.keyValueNavPriceRatio,
                         hSAPData.keyValueYield,
                         hSAPData.keyValuePriceJEKRatio,
                         hSAPData.keyValueErningsDividentRatio,
                         hSAPData.keyValueTotalDebtEquityRatio,
                         hSAPData.keyValueCurrentRatio,
                         hSAPData.trustworthyLeadershipAnswer,
                         hSAPData.trustworthyLeadershipComment,
                         hSAPData.goodOwnershipAnswer,
                         hSAPData.goodOwnershipComment,
                         hSAPData.otherInformation,

                         hSAPData.tenYearNoteYield,
                         hSAPData.currEquityPerShare,
                         hSAPData.estimateYearlyDividend,
                         hSAPData.calcIntrinsicValue,
                         hSAPData.intrinsicValueYearSpan,
                         hSAPData.historicalYearlyInterestOnEquity);


    //m_companyDescription = hSAPData.companyDescription;


    hSAPData.dividendDataArr = m_dividendDataArr;
    hSAPData.nofDividendArrData = m_nofDividendArrData;

    hSAPData.earningsDataArr = &m_earningsDataArr[0];
    hSAPData.nofEarningsArrData = m_nofEarningsArrData;

    hSAPData.totalCurrentAssetsArr = m_totalCurrentAssetsArr;
    hSAPData.nofTotalCurrentAssetsArrData = m_nofTotalCurrentAssetsArrData;
    hSAPData.totalCurrentLiabilitiesArr = m_totalCurrentLiabilitiesArr;
    hSAPData.nofTotalCurrentLiabilitiesData = m_nofTotalCurrentLiabilitiesData;
    hSAPData.totalLiabilitiesArr = m_totalLiabilitiesArr;
    hSAPData.nofTotalLiabilitiesData = m_nofTotalLiabilitiesData;
    hSAPData.solidityArr = m_solidityArr;
    hSAPData.nofSolidityData = m_nofSolidityData;
    hSAPData.coverageRatioArr = m_coverageRatioArr;
    hSAPData.nofCoverageRatioData = m_nofCoverageRatioData;

    hSAPData.equityArr = m_totEquityArr;
    hSAPData.nofEquityData = m_nofTotEquityData;

    hSAPData.coreTier1RatioArr = m_coreTier1RatioArr;
    hSAPData.nofCoreTier1RatioData = m_nofCoreTier1RatioData;

    hSAPData.coreCapitalRatioArr = m_coreCapitalRatioArr;
    hSAPData.nofCoreCapitalRatioData = m_nofCoreCapitalRatioData;

    hSAPData.cashFlowCapexArr = m_cashFlowCapexArr;
    hSAPData.nofCashFlowCapexData = m_nofCashFlowCapexData;
    hSAPData.operatingCashFlowArr = m_operatingCashFlowArr;
    hSAPData.nofOperatingCashFlowData = m_nofOperatingCashFlowData;

    hSAPData.totDividensArr = m_totDividensArr;
    hSAPData.nofTotDividensData = m_nofTotDividensData;

    hSAPData.riskStdDev = m_riskStdDev;
    hSAPData.meanReturns = m_meanReturns;
    hSAPData.returnOnEquity = m_returnOnEquity;


    csaHtmlPg.createHtmlPage(hSAPData);
     m_saDisply.subAnalysisShowDataInGraphs(hSAPData, m_qwtPlot);

    ui->webView->setHtml(hSAPData.htmlStr);


    // Write html report to disk
    ///home/ajn/Documents/OldPC/swProj/MyQtProj/JackStockProj/JackStock/database/AnalysDoc/htmlReports

    qDebug() << "App path : " << qApp->applicationDirPath();
    // path =  QCoreApplication::applicationDirPath();
    QString path = qApp->applicationDirPath();
    path += "/database/AnalysDoc/htmlReports";
    QString filname;

    filname = m_stockName;
    filname.trimmed();
    filname += "_";
    filname += m_analysisDate;
    filname.trimmed();
    filname += ".html";

    CUtil cu;
    cu.saveTextFile(hSAPData.htmlStr, filname, path);

    ui->textEditCompDescription->insertPlainText(hSAPData.companyDescription);

    // Big enought
    ui->lineEditBigEnoughAnswer->setText(hSAPData.bigEnoughAnswer);
    ui->textEditBigEnoughText->insertPlainText(hSAPData.bigEnoughComment);

    // Financial strong enought
    ui->lineEditFinancialStrongAnswer->setText(hSAPData.strongFinancialPositionAnswer);
    ui->textEditFinancialStrongText->insertPlainText(hSAPData.strongFinancialPositionComment);

    // Earning Stability
    ui->lineEditErningStabilityAnswer->setText(hSAPData.earningStabilityAnswer);
    ui->textEditErningStabilityText->insertPlainText(hSAPData.earningStabilityComment);

    // Dividend Stability
    ui->lineEditDividentStabilityAnswer->setText(hSAPData.dividendStabilityAnswer);
    ui->textEditDividentStabilityText->insertPlainText(hSAPData.dividendStabilityComment);

    // Erning Growth
    ui->lineEditErningGrowthAnswer->setText(hSAPData.earningGrowthAnswer);
    ui->textEditErningGrowthText->insertPlainText(hSAPData.earningGrowthComment);

    // Pe-value
    ui->lineEditPE->setText(hSAPData.keyValuePe);
    ui->lineEditPs->setText(hSAPData.keyValuePs);
    ui->lineEditNavDivLastStockPrice->setText(hSAPData.keyValueNavPriceRatio);
    ui->lineEditYield->setText(hSAPData.keyValueYield);

    // m_keyValuePriceJEKRatio, saknas
    ui->lineEditTotDebtEquityRatio->setText(hSAPData.keyValueTotalDebtEquityRatio);
    ui->lineEditEarningsDivByDividend->setText(hSAPData.keyValueErningsDividentRatio);
    ui->lineEditCurrentRatio->setText(hSAPData.keyValueCurrentRatio);

    // Trustworthy Leadership
    ui->lineEditTrustworthyManagementAnswer->setText(hSAPData.trustworthyLeadershipAnswer);
    ui->textEditTrustworthyManagementText->setText(hSAPData.trustworthyLeadershipComment);

    // Good Ownership
    ui->lineEditBeneficialOwnershipAnswer->setText(hSAPData.goodOwnershipAnswer);
    ui->textEditBeneficialOwnershipText->insertPlainText(hSAPData.goodOwnershipComment);

    ui->textEditOtherInfo->insertPlainText(hSAPData.otherInformation);

    // Intrinsic value
    ui->lineEditTenYearNoteYield->setText(hSAPData.tenYearNoteYield);
    ui->lineEditCurrEquityPerShare->setText(hSAPData.currEquityPerShare);
    ui->lineEditEstimateYearlyDividend->setText(hSAPData.estimateYearlyDividend);
    ui->lineEditIntrinsicValueResult->setText(hSAPData.calcIntrinsicValue);
    ui->lineEditHistoricalInterestOnEquity->setText(hSAPData.historicalYearlyInterestOnEquity);



    if(m_nofEquityPerShareData > 1)
    {
        plotEquityPerShareData(m_equityPerShareArr, m_nofEquityPerShareData);
    }

    calcTotSubdataForIntrinsicValue();
    on_pushButtonCalcYearlyIntrestRateOnEquity_clicked();

    // Display all analysis graphs
    displayAllAnalysisPlots();

}



/******************************************************************
 *
 * Function:    resetGuiCtrl()
 *
 * Description:
 *
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::resetGuiCtrl(void)
{
    ui->textEditCompDescription->clear();

    // Big enought
    ui->lineEditBigEnoughAnswer->clear();
    ui->textEditBigEnoughText->clear();

    // Financial strong enought
    ui->lineEditFinancialStrongAnswer->clear();
    ui->textEditFinancialStrongText->clear();

    // Earning Stability
    ui->lineEditErningStabilityAnswer->clear();
    ui->textEditErningStabilityText->clear();

    // Dividend Stability
    ui->lineEditDividentStabilityAnswer->clear();
    ui->textEditDividentStabilityText->clear();

    // Erning Growth
    ui->lineEditErningGrowthAnswer->clear();
    ui->textEditErningGrowthText->clear();

    // Pe-value
    ui->lineEditPE->clear();
    ui->lineEditPs->clear();
    ui->lineEditNavDivLastStockPrice->clear();
    ui->lineEditYield->clear();

    // m_keyValuePriceJEKRatio, saknas
    ui->lineEditTotDebtEquityRatio->clear();
    ui->lineEditEarningsDivByDividend->clear();
    ui->lineEditCurrentRatio->clear();

    // Trustworthy Leadership
    ui->lineEditTrustworthyManagementAnswer->clear();
    ui->textEditTrustworthyManagementText->clear();

    // Good Ownership
    ui->lineEditBeneficialOwnershipAnswer->clear();
    ui->textEditBeneficialOwnershipText->clear();

    ui->textEditOtherInfo->clear();

}


/******************************************************************
 *
 * Function:    on_pushButtonRemoveRecord_clicked()
 *
 * Description:
 *
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::on_pushButtonRemoveRecord_clicked()
{
    QString str;
    CDbHndl db;


    str = QString::fromUtf8("Vill du ta bort data?\n");
    str = str + m_stockName;
    str = str + ", ";
    str = str + m_stockSymbol;
    str = str + ", ";
    str = str + m_analysisDate;


    if( false == QMessageBox::information(NULL, QString::fromUtf8("Uppdatera databas"), str))
    {
        return;
    }


    // Remove all data from data table in db
    if( false == db.deleteStockAnalysisDataRecord(m_stockName, m_stockSymbol, m_analysisDate))
    {
        str = (QString::fromUtf8("Fel: Data gick ej att ta bort\n"));
        QMessageBox::information(NULL, QString::fromUtf8("Uppdatera databas"), str);
        return;
    }

    // Delete data from date table in db
    if( false == db.deleteStockAnalysisDateRecord(m_stockName, m_stockSymbol, m_analysisDate))
    {
        str = (QString::fromUtf8("Fel: Data gick ej att ta bort\n"));
        QMessageBox::information(NULL, QString::fromUtf8("Uppdatera databas"), str);
        return;
    }


    // Find date in treeWidget and remove it
    QTreeWidgetItem *item;
    int index = -1;
    int numberOfTopLevelItems = ui->treeWidgetAnalysisDate->topLevelItemCount();
    for (int topLevelindex = 0 ; topLevelindex < numberOfTopLevelItems ; topLevelindex++)
    {
        item = ui->treeWidgetAnalysisDate->topLevelItem(topLevelindex);
        if(item != NULL)
        {
            if(m_analysisDate.compare(item->text(0)) == 0)
            {
                index = topLevelindex;
                break;
            }
        }
    }




    if(index > -1)
    {
        item  = ui->treeWidgetAnalysisDate->takeTopLevelItem(index);
        delete item; // do not forget to delete the item if it is not owned by any other widget.
    }

}




/******************************************************************
 *
 * Function:    on_pushSaveDividend_clicked()
 *
 * Description: This function saves dividend data to db
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::on_pushSaveDividend_clicked()
{
    CDbHndl db;
    bool res;
    int mainAnalysisId;
    QString str;

    bool isValid;
    int dividendDateId;
    int inputDividendDataId;
    int dividendDataId;
    bool dividendDataIdIsValid = false;

    int nofData;
    QString dividendDate;
    QString dividendData;

    if((m_stockName.length() < 1) || m_stockSymbol.length() < 1)
    {
        QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Välj aktie"));
        return;
    }


    str = (QString::fromUtf8("Vill du lägga till sub data?\n"));
    str = str + m_stockName;
    str = str + ", ";
    str = str + m_stockSymbol;
    str = str + ", ";
    str = str + m_analysisDate;





    if(QMessageBox::No == QMessageBox::question(this, QString::fromUtf8("Uppdatera databas"), str, QMessageBox::Yes|QMessageBox::No))
    {
        return;
    }

    // Check if this stocksymbol and stockname is already added, if not add it
    res = db.mainAnalysisDataExists(m_stockName,
                                m_stockSymbol,
                                mainAnalysisId);
    if(false == res)
    {
        res = db.insertMainAnalysisData(m_stockName,
                                     m_stockSymbol,
                                     mainAnalysisId);
    }



    nofData = ui->tableWidgetDividend->rowCount();

    for(int row = 0; row < nofData; row++)
    {
       if(NULL != ui->tableWidgetDividend->item(row, 0))
       {
            dividendDate = ui->tableWidgetDividend->item(row, 0)->text();
            dividendData = ui->tableWidgetDividend->item(row, 1)->text();

            dividendDate.toInt(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Year is not a number"));
                continue;
            }

            CUtil cu;
            cu.number2double(dividendData, dividendData);
            dividendData.toDouble(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Dividend is not a number"));
                continue;
            }
       }
       else
       {
           break;
       }


        res = db.subAnalysisDividendDateExists(dividendDate,
                                                   mainAnalysisId,
                                                   dividendDateId);

        // Exist anaysis date?
        if(false == res)
        {
            res = db.insertSubAnalysisDividendDate(dividendDate,
                                                   mainAnalysisId,
                                                   dividendDateId);
        }


        if(true == res)
        {
           dividendDataIdIsValid = false;

           if( true == db.getSubAnalysisDividendId(mainAnalysisId, dividendDateId, inputDividendDataId))
           {
               dividendDataIdIsValid = true;
           }

            res = db.insertSubAnalysisDividend(dividendDateId,
                                               mainAnalysisId,
                                               inputDividendDataId,
                                               dividendDataIdIsValid,
                                               dividendData,
                                               dividendDataId);
        }
    }

}



/******************************************************************
 *
 * Function:    on_pushSaveEarnings_clicked()
 *
 * Description: This function saves earnings data to db
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::on_pushSaveEarnings_clicked()
{
    CDbHndl db;
    bool res;
    int mainAnalysisId;
    QString str;

    bool isValid;
    int earningsDateId;
    int inputEarningsDataId;
    int earningsDataId;
    bool earningsDataIdIsValid = false;

    int nofData;
    QString earningsDate;
    QString earningsData;

    if((m_stockName.length() < 1) || m_stockSymbol.length() < 1)
    {
        QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Välj aktie"));
        return;
    }

    str = (QString::fromUtf8("Vill du lägga till sub data?\n"));
    str = str + m_stockName;
    str = str + ", ";
    str = str + m_stockSymbol;
    str = str + ", ";
    str = str + m_analysisDate;





    if(QMessageBox::No == QMessageBox::question(this, QString::fromUtf8("Uppdatera databas"), str, QMessageBox::Yes|QMessageBox::No))
    {
        return;
    }

    // Check if this stocksymbol and stockname is already added, if not add it
    res = db.mainAnalysisDataExists(m_stockName,
                                m_stockSymbol,
                                mainAnalysisId);
    if(false == res)
    {
        res = db.insertMainAnalysisData(m_stockName,
                                     m_stockSymbol,
                                     mainAnalysisId);
    }



    nofData = ui->tableWidgetEarnings->rowCount();

    for(int row = 0; row < nofData; row++)
    {
       if(NULL != ui->tableWidgetEarnings->item(row, 0))
       {
            earningsDate = ui->tableWidgetEarnings->item(row, 0)->text();
            earningsData = ui->tableWidgetEarnings->item(row, 1)->text();

            earningsDate.toInt(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Year is not a number"));
                continue;
            }

            CUtil cu;
            cu.number2double(earningsData, earningsData);
            earningsData.toDouble(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Earnings is not a number"));
                continue;
            }
       }
       else
       {
           break;
       }


        res = db.subAnalysisEarningsPerShareDateExists(earningsDate,
                                               mainAnalysisId,
                                               earningsDateId);

        // Exist anaysis date?
        if(false == res)
        {
            res = db.insertSubAnalysisEarningsPerShareDate(earningsDate,
                                                   mainAnalysisId,
                                                   earningsDateId);
        }


        if(true == res)
        {
           earningsDataIdIsValid = false;

           if( true == db.getSubAnalysisEarningsPerShareId(mainAnalysisId, earningsDateId, inputEarningsDataId))
           {
               earningsDataIdIsValid = true;
           }

            res = db.insertSubAnalysisEarningsPerShare(earningsDateId,
                                               mainAnalysisId,
                                               inputEarningsDataId,
                                               earningsDataIdIsValid,
                                               earningsData,
                                               earningsDataId);
        }
    }

}


/******************************************************************
 *
 * Function:    on_pushSaveEarnings_clicked()
 *
 * Description: This function saves earnings data to db
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::on_pushButtonSaveTotalCurrentAsset_clicked()
{
    CDbHndl db;
    bool res;
    int mainAnalysisId;
    QString str;

    bool isValid;
    int totalCurrentAssetsDateId;
    int inputTotalCurrentAssetsDataId;
    int totalCurrentAssetsDataId;
    bool totalCurrentAssetsDataIdIsValid = false;

    int nofData;
    QString totalCurrentAssetsDate;
    QString totalCurrentAssetsData;

    if((m_stockName.length() < 1) || m_stockSymbol.length() < 1)
    {
        QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Välj aktie"));
        return;
    }


    str = (QString::fromUtf8("Vill du lägga till sub data?\n"));
    str = str + m_stockName;
    str = str + ", ";
    str = str + m_stockSymbol;
    str = str + ", ";
    str = str + m_analysisDate;





    if(QMessageBox::No == QMessageBox::question(this, QString::fromUtf8("Uppdatera databas"), str, QMessageBox::Yes|QMessageBox::No))
    {
        return;
    }

    // Check if this stocksymbol and stockname is already added, if not add it
    res = db.mainAnalysisDataExists(m_stockName,
                                m_stockSymbol,
                                mainAnalysisId);
    if(false == res)
    {
        res = db.insertMainAnalysisData(m_stockName,
                                     m_stockSymbol,
                                     mainAnalysisId);
    }



    nofData = ui->tableWidgetTotalCurrentAsset->rowCount();

    for(int row = 0; row < nofData; row++)
    {
       if(NULL != ui->tableWidgetTotalCurrentAsset->item(row, 0))
       {
            totalCurrentAssetsDate = ui->tableWidgetTotalCurrentAsset->item(row, 0)->text();
            totalCurrentAssetsData = ui->tableWidgetTotalCurrentAsset->item(row, 1)->text();

            totalCurrentAssetsDate.toInt(&isValid);
            if (false == isValid)
            {
               QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Year is not a number"));
               continue;
            }

            CUtil cu;
            cu.number2double(totalCurrentAssetsData, totalCurrentAssetsData);
            totalCurrentAssetsData.toDouble(&isValid);
            if (false == isValid)
            {
               QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Earnings is not a number"));
               continue;
            }
       }
       else
       {
           break;
       }


        res = db.subAnalysisTotalCurrentAssetsDateExists(totalCurrentAssetsDate,
                                                        mainAnalysisId,
                                                        totalCurrentAssetsDateId);

        // Exist anaysis date?
        if(false == res)
        {
            res = db.insertSubAnalysisTotalCurrentAssetsDate(totalCurrentAssetsDate,
                                                            mainAnalysisId,
                                                            totalCurrentAssetsDateId);
        }


        if(true == res)
        {
           totalCurrentAssetsDataIdIsValid = false;

           if( true == db.getSubAnalysisTotalCurrentAssetsDataId(mainAnalysisId, totalCurrentAssetsDateId, inputTotalCurrentAssetsDataId))
           {
               totalCurrentAssetsDataIdIsValid = true;
           }

            res = db.insertSubAnalysisTotalCurrentAssets(totalCurrentAssetsDateId,
                                                        mainAnalysisId,
                                                        inputTotalCurrentAssetsDataId,
                                                        totalCurrentAssetsDataIdIsValid,
                                                        totalCurrentAssetsData,
                                                        totalCurrentAssetsDataId);
        }
    }
}


/******************************************************************
 *
 * Function:    on_pushButtonSaveTotalCurrentLiabilities_clicked()
 *
 * Description: This function saves Total Current Liabilities data to db
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::on_pushButtonSaveTotalCurrentLiabilities_clicked()
{
    CDbHndl db;
    bool res;
    int mainAnalysisId;
    QString str;

    bool isValid;
    int dateId;
    int inputDataId;
    int dataId;
    bool dataIdIsValid = false;

    int nofData;
    QString date;
    QString data;

    if((m_stockName.length() < 1) || m_stockSymbol.length() < 1)
    {
        QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Välj aktie"));
        return;
    }


    str = (QString::fromUtf8("Vill du lägga till sub data?\n"));
    str = str + m_stockName;
    str = str + ", ";
    str = str + m_stockSymbol;
    str = str + ", ";
    str = str + m_analysisDate;





    if(QMessageBox::No == QMessageBox::question(this, QString::fromUtf8("Uppdatera databas"), str, QMessageBox::Yes|QMessageBox::No))
    {
        return;
    }

    // Check if this stocksymbol and stockname is already added, if not add it
    res = db.mainAnalysisDataExists(m_stockName,
                                m_stockSymbol,
                                mainAnalysisId);
    if(false == res)
    {
        res = db.insertMainAnalysisData(m_stockName,
                                     m_stockSymbol,
                                     mainAnalysisId);
    }



    nofData = ui->tableWidgetTotalCurrentLiabilities->rowCount();

    for(int row = 0; row < nofData; row++)
    {
       if(NULL != ui->tableWidgetTotalCurrentLiabilities->item(row, 0))
       {
            date = ui->tableWidgetTotalCurrentLiabilities->item(row, 0)->text();
            data = ui->tableWidgetTotalCurrentLiabilities->item(row, 1)->text();

            date.toInt(&isValid);
            if (false == isValid)
            {
               QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Year is not a number"));
               continue;
            }

            CUtil cu;
            cu.number2double(data, data);
            data.toDouble(&isValid);
            if (false == isValid)
            {
               QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Earnings is not a number"));
               continue;
            }
       }
       else
       {
           break;
       }


        res = db.subAnalysisTotalCurrentLiabilitiesDateExists(date,
                                                        mainAnalysisId,
                                                        dateId);

        // Exist anaysis date?
        if(false == res)
        {
            res = db.insertSubAnalysisTotalCurrentLiabilitiesDate(date,
                                                            mainAnalysisId,
                                                            dateId);
        }


        if(true == res)
        {
           dataIdIsValid = false;

           if( true == db.getSubAnalysisTotalCurrentLiabilitiesDataId(mainAnalysisId, dateId, inputDataId))
           {
               dataIdIsValid = true;
           }

            res = db.insertSubAnalysisTotalCurrentLiabilities(dateId,
                                                              mainAnalysisId,
                                                              inputDataId,
                                                              dataIdIsValid,
                                                              data,
                                                              dataId);
        }
    }

}



/******************************************************************
 *
 * Function:    on_pushButtonSaveTotalLiabilities_clicked()
 *
 * Description: This function saves Total Liabilities data to db
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::on_pushButtonSaveTotalLiabilities_clicked()
{
    CDbHndl db;
    bool res;
    int mainAnalysisId;
    QString str;

    bool isValid;
    int dateId;
    int inputDataId;
    int dataId;
    bool dataIdIsValid = false;

    int nofData;
    QString date;
    QString data;


    if((m_stockName.length() < 1) || m_stockSymbol.length() < 1)
    {
        QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Välj aktie"));
        return;
    }


    str = (QString::fromUtf8("Vill du lägga till sub data?\n"));
    str = str + m_stockName;
    str = str + ", ";
    str = str + m_stockSymbol;
    str = str + ", ";
    str = str + m_analysisDate;




    if(QMessageBox::No == QMessageBox::question(this, QString::fromUtf8("Uppdatera databas"), str, QMessageBox::Yes|QMessageBox::No))
    {
        return;
    }

    // Check if this stocksymbol and stockname is already added, if not add it
    res = db.mainAnalysisDataExists(m_stockName,
                                m_stockSymbol,
                                mainAnalysisId);
    if(false == res)
    {
        res = db.insertMainAnalysisData(m_stockName,
                                     m_stockSymbol,
                                     mainAnalysisId);
    }



    nofData = ui->tableWidgetTotalLiabilities->rowCount();

    for(int row = 0; row < nofData; row++)
    {
       if(NULL != ui->tableWidgetTotalLiabilities->item(row, 0))
       {
            date = ui->tableWidgetTotalLiabilities->item(row, 0)->text();
            data = ui->tableWidgetTotalLiabilities->item(row, 1)->text();

            date.toInt(&isValid);
            if (false == isValid)
            {
               QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Year is not a number"));
               continue;
            }

            CUtil cu;
            cu.number2double(data, data);
            data.toDouble(&isValid);
            if (false == isValid)
            {
               QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Earnings is not a number"));
               continue;
            }
       }
       else
       {
           break;
       }


        res = db.subAnalysisTotalLiabilitiesDateExists(date,
                                                       mainAnalysisId,
                                                       dateId);

        // Exist anaysis date?
        if(false == res)
        {
            res = db.insertSubAnalysisTotalLiabilitiesDate(date,
                                                           mainAnalysisId,
                                                           dateId);
        }


        if(true == res)
        {
           dataIdIsValid = false;

           if( true == db.getSubAnalysisTotalLiabilitiesDataId(mainAnalysisId, dateId, inputDataId))
           {
               dataIdIsValid = true;
           }

            res = db.insertSubAnalysisTotalLiabilities(dateId,
                                                       mainAnalysisId,
                                                       inputDataId,
                                                       dataIdIsValid,
                                                       data,
                                                       dataId);
        }
    }

}



/******************************************************************
 *
 * Function:    on_pushButtonSaveTotalLiabilities_clicked()
 *
 * Description: This function set company type and save it in db
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::on_pushButtonSelectSaveCompanyType_clicked()
{
    CDbHndl db;
    bool res;
    int mainAnalysisId;
    int companyType;
    int companyTypeId;


    if((m_stockName.length() < 1) || m_stockSymbol.length() < 1)
    {
        QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Välj aktie"));
        return;
    }

    QString str;

    str = (QString::fromUtf8("Vill du lägga till bolagstyp?\n"));
    str = str + m_stockName;
    str = str + ", ";
    str = str + m_stockSymbol;


    if(m_stockNameIsInit == false)
    {
        return;
    }

    if(QMessageBox::No == QMessageBox::question(this, QString::fromUtf8("Uppdatera databas"), str, QMessageBox::Yes|QMessageBox::No))
    {
        return;
    }

    // Check if this stocksymbol and stockname is already added, if not add it
    res = db.mainAnalysisDataExists(m_stockName,
                                m_stockSymbol,
                                mainAnalysisId);
    if(false == res)
    {
        res = db.insertMainAnalysisData(m_stockName,
                                     m_stockSymbol,
                                     mainAnalysisId);
    }


    companyType = ui->comboBoxCompanyType->currentIndex();

    db.deleteCompanyType(mainAnalysisId);

    res = db.insertSubAnalysisCompanyType(companyType,
                                          mainAnalysisId,
                                          companyTypeId);

    if(companyType < NOF_COMPANY_TYPE_ARR_DATA)
    {
        ui->labelCompanyType->setText(m_companyTypesArr[companyType]);
        ui->labelMainPglabelCompanyType->setText(m_companyTypesArr[companyType]);

        ui->comboBoxCompanyType->setCurrentIndex(companyType);
    }

}


/******************************************************************
 *
 * Function:    on_pushButtonSaveSolidity_clicked()
 *
 * Description: This function saves Solidity data to db
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::on_pushButtonSaveSolidity_clicked()
{
    CDbHndl db;
    bool res;
    int mainAnalysisId;
    QString str;

    bool isValid;
    int dateId;
    int inputDataId;
    int dataId;
    bool dataIdIsValid = false;

    int nofData;
    QString date;
    QString data;

    if((m_stockName.length() < 1) || m_stockSymbol.length() < 1)
    {
        QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Välj aktie"));
        return;
    }


    str = (QString::fromUtf8("Vill du lägga till sub data?\n"));
    str = str + m_stockName;
    str = str + ", ";
    str = str + m_stockSymbol;
    str = str + ", ";
    str = str + m_analysisDate;





    if(QMessageBox::No == QMessageBox::question(this, QString::fromUtf8("Uppdatera databas"), str, QMessageBox::Yes|QMessageBox::No))
    {
        return;
    }

    // Check if this stocksymbol and stockname is already added, if not add it
    res = db.mainAnalysisDataExists(m_stockName,
                                m_stockSymbol,
                                mainAnalysisId);
    if(false == res)
    {
        res = db.insertMainAnalysisData(m_stockName,
                                     m_stockSymbol,
                                     mainAnalysisId);
    }



    nofData = ui->tableWidgetSolidity->rowCount();

    for(int row = 0; row < nofData; row++)
    {
       if(NULL != ui->tableWidgetSolidity->item(row, 0))
       {
            date = ui->tableWidgetSolidity->item(row, 0)->text();
            data = ui->tableWidgetSolidity->item(row, 1)->text();

            date.toInt(&isValid);
            if (false == isValid)
            {
               QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Year is not a number"));
               continue;
            }

            CUtil cu;
            cu.number2double(data, data);
            data.toDouble(&isValid);
            if (false == isValid)
            {
               QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Solidity is not a number"));
               continue;
            }
       }
       else
       {
           break;
       }


        res = db.subAnalysisSolidityDateExists(date,
                                               mainAnalysisId,
                                               dateId);

        // Exist anaysis date?
        if(false == res)
        {
            res = db.insertSubAnalysisSolidityDate(date,
                                                   mainAnalysisId,
                                                   dateId);
        }


        if(true == res)
        {
           dataIdIsValid = false;

           if( true == db.getSubAnalysisSolidityDataId(mainAnalysisId, dateId, inputDataId))
           {
               dataIdIsValid = true;
           }

            res = db.insertSubAnalysisSolidityData(dateId,
                                               mainAnalysisId,
                                               inputDataId,
                                               dataIdIsValid,
                                               data,
                                               dataId);
        }
    }

}

/******************************************************************
 *
 * Function:    on_pushButtonSaveCoverageRatio_clicked()
 *
 * Description: This function saves Coverage Ratio data to db
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::on_pushButtonSaveCoverageRatio_clicked()
{
    CDbHndl db;
    bool res;
    int mainAnalysisId;
    QString str;

    bool isValid;
    int dateId;
    int inputDataId;
    int dataId;
    bool dataIdIsValid = false;

    int nofData;
    QString date;
    QString data;


    if((m_stockName.length() < 1) || m_stockSymbol.length() < 1)
    {
        QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Välj aktie"));
        return;
    }


    str = (QString::fromUtf8("Vill du lägga till sub data?\n"));
    str = str + m_stockName;
    str = str + ", ";
    str = str + m_stockSymbol;
    str = str + ", ";
    str = str + m_analysisDate;


    if(QMessageBox::No == QMessageBox::question(this, QString::fromUtf8("Uppdatera databas"), str, QMessageBox::Yes|QMessageBox::No))
    {
        return;
    }

    // Check if this stocksymbol and stockname is already added, if not add it
    res = db.mainAnalysisDataExists(m_stockName, m_stockSymbol, mainAnalysisId);

    if(false == res)
    {
        res = db.insertMainAnalysisData(m_stockName, m_stockSymbol, mainAnalysisId);
    }


    nofData = ui->tableWidgetCoverageRatio->rowCount();

    for(int row = 0; row < nofData; row++)
    {
        if(NULL != ui->tableWidgetCoverageRatio->item(row, 0))
        {
            date = ui->tableWidgetCoverageRatio->item(row, 0)->text();
            data = ui->tableWidgetCoverageRatio->item(row, 1)->text();

            date.toInt(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Year is not a number"));
                continue;
            }

            CUtil cu;
            cu.number2double(data, data);
            data.toDouble(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("CoverageRatio is not a number"));
                continue;
            }
        }
        else
        {
           break;
        }


        res = db.subAnalysisCoverageRatioDateExists(date, mainAnalysisId, dateId);

        // Exist anaysis date?
        if(false == res)
        {
            res = db.insertSubAnalysisCoverageRatioDate(date, mainAnalysisId, dateId);
        }


        if(true == res)
        {
           dataIdIsValid = false;

           if( true == db.getSubAnalysisCoverageRatioDataId(mainAnalysisId, dateId, inputDataId))
           {
               dataIdIsValid = true;
           }

           res = db.insertSubAnalysisCoverageRatioData(dateId, mainAnalysisId, inputDataId, dataIdIsValid,
                                                       data, dataId);
        }
    }
}


/******************************************************************
 *
 * Function:    on_pushButtonSaveCoreTier1Ratio_clicked()
 *
 * Description: This function saves CoreTier1Ratio data to db
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::on_pushButtonSaveCoreTier1Ratio_clicked()
{
    CDbHndl db;
    bool res;
    int mainAnalysisId;
    QString str;

    bool isValid;
    int dateId;
    int inputDataId;
    int dataId;
    bool dataIdIsValid = false;

    int nofData;
    QString date;
    QString data;


    if((m_stockName.length() < 1) || m_stockSymbol.length() < 1)
    {
        QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Välj aktie"));
        return;
    }


    str = (QString::fromUtf8("Vill du lägga till sub data?\n"));
    str = str + m_stockName;
    str = str + ", ";
    str = str + m_stockSymbol;
    str = str + ", ";
    str = str + m_analysisDate;


    if(QMessageBox::No == QMessageBox::question(this, QString::fromUtf8("Uppdatera databas"), str, QMessageBox::Yes|QMessageBox::No))
    {
        return;
    }

    // Check if this stocksymbol and stockname is already added, if not add it
    res = db.mainAnalysisDataExists(m_stockName, m_stockSymbol, mainAnalysisId);

    if(false == res)
    {
        res = db.insertMainAnalysisData(m_stockName, m_stockSymbol, mainAnalysisId);
    }


    nofData = ui->tableWidgetCoreTier1Ratio->rowCount();

    for(int row = 0; row < nofData; row++)
    {
        if(NULL != ui->tableWidgetCoreTier1Ratio->item(row, 0))
        {
            date = ui->tableWidgetCoreTier1Ratio->item(row, 0)->text();
            data = ui->tableWidgetCoreTier1Ratio->item(row, 1)->text();

            date.toInt(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Year is not a number"));
                continue;
            }

            CUtil cu;
            cu.number2double(data, data);
            data.toDouble(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("CoreTier1Ratio is not a number"));
                continue;
            }
        }
        else
        {
           break;
        }


        res = db.subAnalysisCoreTier1RatioDateExists(date, mainAnalysisId, dateId);

        // Exist anaysis date?
        if(false == res)
        {
            res = db.insertSubAnalysisCoreTier1RatioDate(date, mainAnalysisId, dateId);
        }


        if(true == res)
        {
           dataIdIsValid = false;

           if( true == db.getSubAnalysisCoreTier1RatioDataId(mainAnalysisId, dateId, inputDataId))
           {
               dataIdIsValid = true;
           }

           res = db.insertSubAnalysisCoreTier1RatioData(dateId, mainAnalysisId, inputDataId, dataIdIsValid,
                                                       data, dataId);
        }
    }
}



/******************************************************************
 *
 * Function:    on_pushButtonSaveCoreCapitalRatio_clicked()
 *
 * Description: This function saves CoreCapitalRatio data to db
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::on_pushButtonSaveCoreCapitalRatio_clicked()
{
    CDbHndl db;
    bool res;
    int mainAnalysisId;
    QString str;

    bool isValid;
    int dateId;
    int inputDataId;
    int dataId;
    bool dataIdIsValid = false;

    int nofData;
    QString date;
    QString data;


    if((m_stockName.length() < 1) || m_stockSymbol.length() < 1)
    {
        QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Välj aktie"));
        return;
    }


    str = (QString::fromUtf8("Vill du lägga till sub data?\n"));
    str = str + m_stockName;
    str = str + ", ";
    str = str + m_stockSymbol;
    str = str + ", ";
    str = str + m_analysisDate;


    if(QMessageBox::No == QMessageBox::question(this, QString::fromUtf8("Uppdatera databas"), str, QMessageBox::Yes|QMessageBox::No))
    {
        return;
    }

    // Check if this stocksymbol and stockname is already added, if not add it
    res = db.mainAnalysisDataExists(m_stockName, m_stockSymbol, mainAnalysisId);

    if(false == res)
    {
        res = db.insertMainAnalysisData(m_stockName, m_stockSymbol, mainAnalysisId);
    }


    nofData = ui->tableWidgetCoreCapitalRatio->rowCount();

    for(int row = 0; row < nofData; row++)
    {
        if(NULL != ui->tableWidgetCoreCapitalRatio->item(row, 0))
        {
            date = ui->tableWidgetCoreCapitalRatio->item(row, 0)->text();
            data = ui->tableWidgetCoreCapitalRatio->item(row, 1)->text();

            date.toInt(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Year is not a number"));
                continue;
            }

            CUtil cu;
            cu.number2double(data, data);
            data.toDouble(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("CoreCapitalRatio is not a number"));
                continue;
            }
        }
        else
        {
           break;
        }


        res = db.subAnalysisCoreCapitalRatioDateExists(date, mainAnalysisId, dateId);

        // Exist anaysis date?
        if(false == res)
        {
            res = db.insertSubAnalysisCoreCapitalRatioDate(date, mainAnalysisId, dateId);
        }


        if(true == res)
        {
           dataIdIsValid = false;

           if( true == db.getSubAnalysisCoreCapitalRatioDataId(mainAnalysisId, dateId, inputDataId))
           {
               dataIdIsValid = true;
           }

           res = db.insertSubAnalysisCoreCapitalRatioData(dateId, mainAnalysisId, inputDataId, dataIdIsValid,
                                                       data, dataId);
        }
    }

}




/******************************************************************
 *
 * Function:    on_pushButtonSaveEquity_clicked()
 *
 * Description: This function saves Equity data to db
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::on_pushButtonSaveEquity_clicked()
{
    CDbHndl db;
    bool res;
    int mainAnalysisId;
    QString str;

    bool isValid;
    int dateId;
    int inputDataId;
    int dataId;
    bool dataIdIsValid = false;

    int nofData;
    QString date;
    QString data;


    if((m_stockName.length() < 1) || m_stockSymbol.length() < 1)
    {
        QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Välj aktie"));
        return;
    }


    str = (QString::fromUtf8("Vill du lägga till sub data?\n"));
    str = str + m_stockName;
    str = str + ", ";
    str = str + m_stockSymbol;
    str = str + ", ";
    str = str + m_analysisDate;


    if(QMessageBox::No == QMessageBox::question(this, QString::fromUtf8("Uppdatera databas"), str, QMessageBox::Yes|QMessageBox::No))
    {
        return;
    }

    // Check if this stocksymbol and stockname is already added, if not add it
    res = db.mainAnalysisDataExists(m_stockName, m_stockSymbol, mainAnalysisId);

    if(false == res)
    {
        res = db.insertMainAnalysisData(m_stockName, m_stockSymbol, mainAnalysisId);
    }


    nofData = ui->tableWidgetEquity->rowCount();

    for(int row = 0; row < nofData; row++)
    {
        if(NULL != ui->tableWidgetEquity->item(row, 0))
        {
            date = ui->tableWidgetEquity->item(row, 0)->text();
            data = ui->tableWidgetEquity->item(row, 1)->text();

            date.toInt(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Year is not a number"));
                continue;
            }

            CUtil cu;
            cu.number2double(data, data);
            data.toDouble(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Equity is not a number"));
                continue;
            }
        }
        else
        {
           break;
        }


        res = db.subAnalysisEquityDateExists(date, mainAnalysisId, dateId);

        // Exist anaysis date?
        if(false == res)
        {
            res = db.insertSubAnalysisEquityDate(date, mainAnalysisId, dateId);
        }


        if(true == res)
        {
           dataIdIsValid = false;

           if( true == db.getSubAnalysisEquityDataId(mainAnalysisId, dateId, inputDataId))
           {
               dataIdIsValid = true;
           }

           res = db.insertSubAnalysisEquityData(dateId, mainAnalysisId, inputDataId, dataIdIsValid,
                                                       data, dataId);
        }
    }
}


/******************************************************************
 *
 * Function:    on_pushButtonSaveEquity_clicked()
 *
 * Description: This function saves CashFlowCapex data to db
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::on_pushButtonSaveCashFlowCapex_clicked()
{
    CDbHndl db;
    bool res;
    int mainAnalysisId;
    QString str;

    bool isValid;
    int dateId;
    int inputDataId;
    int dataId;
    bool dataIdIsValid = false;

    int nofData;
    QString date;
    QString data;


    if((m_stockName.length() < 1) || m_stockSymbol.length() < 1)
    {
        QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Välj aktie"));
        return;
    }


    str = (QString::fromUtf8("Vill du lägga till sub data?\n"));
    str = str + m_stockName;
    str = str + ", ";
    str = str + m_stockSymbol;
    str = str + ", ";
    str = str + m_analysisDate;


    if(QMessageBox::No == QMessageBox::question(this, QString::fromUtf8("Uppdatera databas"), str, QMessageBox::Yes|QMessageBox::No))
    {
        return;
    }

    // Check if this stocksymbol and stockname is already added, if not add it
    res = db.mainAnalysisDataExists(m_stockName, m_stockSymbol, mainAnalysisId);

    if(false == res)
    {
        res = db.insertMainAnalysisData(m_stockName, m_stockSymbol, mainAnalysisId);
    }


    nofData = ui->tableWidgetCashFlowCapex->rowCount();

    for(int row = 0; row < nofData; row++)
    {
        if(NULL != ui->tableWidgetCashFlowCapex->item(row, 0))
        {
            date = ui->tableWidgetCashFlowCapex->item(row, 0)->text();
            data = ui->tableWidgetCashFlowCapex->item(row, 1)->text();

            date.toInt(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Year is not a number"));
                continue;
            }

            CUtil cu;
            cu.number2double(data, data);
            data.toDouble(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("CashFlowCapex is not a number"));
                continue;
            }
        }
        else
        {
           break;
        }


        res = db.subAnalysisCashFlowCapexDateExists(date, mainAnalysisId, dateId);

        // Exist anaysis date?
        if(false == res)
        {
            res = db.insertSubAnalysisCashFlowCapexDate(date, mainAnalysisId, dateId);
        }


        if(true == res)
        {
           dataIdIsValid = false;

           if( true == db.getSubAnalysisCashFlowCapexDataId(mainAnalysisId, dateId, inputDataId))
           {
               dataIdIsValid = true;
           }

           res = db.insertSubAnalysisCashFlowCapexData(dateId, mainAnalysisId, inputDataId, dataIdIsValid,
                                                       data, dataId);
        }
    }

}


/******************************************************************
 *
 * Function:    on_pushButtonSaveEquity_clicked()
 *
 * Description: This function saves Operating Cash Flow data to db
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::on_pushButtonSaveOperatingCashFlow_clicked()
{

    CDbHndl db;
    bool res;
    int mainAnalysisId;
    QString str;

    bool isValid;
    int dateId;
    int inputDataId;
    int dataId;
    bool dataIdIsValid = false;

    int nofData;
    QString date;
    QString data;


    if((m_stockName.length() < 1) || m_stockSymbol.length() < 1)
    {
        QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Välj aktie"));
        return;
    }


    str = (QString::fromUtf8("Vill du lägga till sub data?\n"));
    str = str + m_stockName;
    str = str + ", ";
    str = str + m_stockSymbol;
    str = str + ", ";
    str = str + m_analysisDate;


    if(QMessageBox::No == QMessageBox::question(this, QString::fromUtf8("Uppdatera databas"), str, QMessageBox::Yes|QMessageBox::No))
    {
        return;
    }

    // Check if this stocksymbol and stockname is already added, if not add it
    res = db.mainAnalysisDataExists(m_stockName, m_stockSymbol, mainAnalysisId);

    if(false == res)
    {
        res = db.insertMainAnalysisData(m_stockName, m_stockSymbol, mainAnalysisId);
    }


    nofData = ui->tableWidgetOperatingCashFlow->rowCount();

    for(int row = 0; row < nofData; row++)
    {
        if(NULL != ui->tableWidgetOperatingCashFlow->item(row, 0))
        {
            date = ui->tableWidgetOperatingCashFlow->item(row, 0)->text();
            data = ui->tableWidgetOperatingCashFlow->item(row, 1)->text();

            date.toInt(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Year is not a number"));
                continue;
            }

            CUtil cu;
            cu.number2double(data, data);
            data.toDouble(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("OperatingCashFlow is not a number"));
                continue;
            }
        }
        else
        {
           break;
        }


        res = db.subAnalysisOperatingCashFlowDateExists(date, mainAnalysisId, dateId);

        // Exist anaysis date?
        if(false == res)
        {
            res = db.insertSubAnalysisOperatingCashFlowDate(date, mainAnalysisId, dateId);
        }


        if(true == res)
        {
           dataIdIsValid = false;

           if( true == db.getSubAnalysisOperatingCashFlowDataId(mainAnalysisId, dateId, inputDataId))
           {
               dataIdIsValid = true;
           }

           res = db.insertSubAnalysisOperatingCashFlowData(dateId, mainAnalysisId, inputDataId, dataIdIsValid,
                                                       data, dataId);
        }
    }
}



/******************************************************************
 *
 * Function:    plotCashflowData()
 *
 * Description:
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::plotCashflowData(void)
{
    int i;
    CDbHndl db;
    std::vector<double> data_y;
    std::vector<double> data_x;
    double minX;
    double maxX;
    double minY;
    double maxY;


    if((m_nofOperatingCashFlowData < 2) && (m_nofOperatingCashFlowData != m_nofCashFlowCapexData))
    {
        return;
    }


    minX = m_operatingCashFlowArr[0].date.toDouble();
    maxX = m_operatingCashFlowArr[0].date.toDouble();
    minY = m_operatingCashFlowArr[0].data.toDouble();
    maxY = m_operatingCashFlowArr[0].data.toDouble();


    for(i = 0; i < m_nofOperatingCashFlowData; i++)
    {

        data_x.push_back(m_operatingCashFlowArr[i].date.toDouble());
        data_y.push_back(m_operatingCashFlowArr[i].data.toDouble());

        // Check x
        if(minX > m_operatingCashFlowArr[i].date.toDouble())
        {
            minX = m_operatingCashFlowArr[i].date.toDouble();
        }

        if(maxX < m_operatingCashFlowArr[i].date.toDouble())
        {
            maxX = m_operatingCashFlowArr[i].date.toDouble();
        }

        // Check y
        if(minY > m_operatingCashFlowArr[i].data.toDouble())
        {
            minY = m_operatingCashFlowArr[i].data.toDouble();
        }

        if(maxY < m_operatingCashFlowArr[i].data.toDouble())
        {
            maxY = m_operatingCashFlowArr[i].data.toDouble();
        }
    }


    m_qwtcashFlowPlotData.stock[0].data.detach();
    m_qwtcashFlowPlotData.stock[0].data.setData(NULL);


    m_qwtcashFlowPlotData.stock[0].data.setSamples(data_x.data(),data_y.data(),data_y.size());
    m_qwtcashFlowPlotData.stock[0].data.setPen(QPen(Qt::blue, 2));
    m_qwtcashFlowPlotData.stock[0].data.setSymbol( new QwtSymbol(QwtSymbol::Ellipse,                                                            Qt::blue,
                                                                 QPen( Qt::blue ),
                                                                  QSize( 7, 7 ) ) );

    data_x.clear();
    data_y.clear();


    for(i = 0; i < m_nofCashFlowCapexData; i++)
    {

        data_x.push_back(m_cashFlowCapexArr[i].date.toDouble());
        data_y.push_back(-m_cashFlowCapexArr[i].data.toDouble());

        // Check x
        if(minX > m_cashFlowCapexArr[i].date.toDouble())
        {
            minX = m_cashFlowCapexArr[i].date.toDouble();
        }

        if(maxX < m_cashFlowCapexArr[i].date.toDouble())
        {
            maxX = m_cashFlowCapexArr[i].date.toDouble();
        }

        // Check y
        if(minY > -m_cashFlowCapexArr[i].data.toDouble())
        {
            minY = -m_cashFlowCapexArr[i].data.toDouble();
        }

        if(maxY < -m_cashFlowCapexArr[i].data.toDouble())
        {
            maxY = -m_cashFlowCapexArr[i].data.toDouble();
        }
    }

    // ui->qwtCashFlowPlot->setAxisScale(QwtPlot::xBottom, minX, maxX);
    // ui->qwtCashFlowPlot->setAxisScale(QwtPlot::yLeft, minY, maxY); // Max av % satser

    m_qwtcashFlowPlotData.stock[1].data.detach();
    m_qwtcashFlowPlotData.stock[1].data.setData(NULL);


    m_qwtcashFlowPlotData.stock[1].data.setSamples(data_x.data(),data_y.data(),data_y.size());
    m_qwtcashFlowPlotData.stock[1].data.setPen(QPen(Qt::red, 2));
    m_qwtcashFlowPlotData.stock[1].data.setSymbol( new QwtSymbol(QwtSymbol::Ellipse,
                                                                 Qt::red,
                                                                 QPen( Qt::red ),
                                                                  QSize( 7, 7 ) ) );


    //================
    data_x.clear();
    data_y.clear();


    for(i = 0; i < m_nofTotDividensData; i++)
    {

        data_x.push_back(m_totDividensArr[i].date.toDouble());
        data_y.push_back(-m_totDividensArr[i].data.toDouble());

        // Check x
        if(minX > m_totDividensArr[i].date.toDouble())
        {
            minX = m_totDividensArr[i].date.toDouble();
        }

        if(maxX < m_totDividensArr[i].date.toDouble())
        {
            maxX = m_totDividensArr[i].date.toDouble();
        }

        // Check y
        if(minY > -m_totDividensArr[i].data.toDouble())
        {
            minY = -m_totDividensArr[i].data.toDouble();
        }

        if(maxY < -m_totDividensArr[i].data.toDouble())
        {
            maxY = -m_totDividensArr[i].data.toDouble();
        }
    }

    ui->qwtCashFlowPlot->setAxisScale(QwtPlot::xBottom, minX, maxX);
    ui->qwtCashFlowPlot->setAxisScale(QwtPlot::yLeft, minY, maxY); // Max av % satser

    m_qwtcashFlowPlotData.stock[2].data.detach();
    m_qwtcashFlowPlotData.stock[2].data.setData(NULL);


    m_qwtcashFlowPlotData.stock[2].data.setSamples(data_x.data(),data_y.data(),data_y.size());
    m_qwtcashFlowPlotData.stock[2].data.setPen(QPen(Qt::green, 2));
    m_qwtcashFlowPlotData.stock[2].data.setSymbol( new QwtSymbol(QwtSymbol::Ellipse,
                                                                 Qt::green,
                                                                 QPen( Qt::red ),
                                                                  QSize( 7, 7 ) ) );


    //================

    // 1:st curve
    m_qwtcashFlowPlotData.stock[0].data.attach(ui->qwtCashFlowPlot);
    ui->qwtCashFlowPlot->replot();

    // 2:st curve
    m_qwtcashFlowPlotData.stock[1].data.attach(ui->qwtCashFlowPlot);
    ui->qwtCashFlowPlot->replot();

    // 3:th curve
    m_qwtcashFlowPlotData.stock[2].data.attach(ui->qwtCashFlowPlot);
    ui->qwtCashFlowPlot->replot();

    data_x.clear();
    data_y.clear();

}



/******************************************************************
 *
 * Function:    on_pushButtonSaveTotDividends_clicked()
 *
 * Description:
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::on_pushButtonSaveTotDividends_clicked()
{

    CDbHndl db;
    bool res;
    int mainAnalysisId;
    QString str;

    bool isValid;
    int dateId;
    int inputDataId;
    int dataId;
    bool dataIdIsValid = false;

    int nofData;
    QString date;
    QString data;


    if((m_stockName.length() < 1) || m_stockSymbol.length() < 1)
    {
        QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Välj aktie"));
        return;
    }


    str = (QString::fromUtf8("Vill du lägga till sub data?\n"));
    str = str + m_stockName;
    str = str + ", ";
    str = str + m_stockSymbol;
    str = str + ", ";
    str = str + m_analysisDate;


    if(QMessageBox::No == QMessageBox::question(this, QString::fromUtf8("Uppdatera databas"), str, QMessageBox::Yes|QMessageBox::No))
    {
        return;
    }

    // Check if this stocksymbol and stockname is already added, if not add it
    res = db.mainAnalysisDataExists(m_stockName, m_stockSymbol, mainAnalysisId);

    if(false == res)
    {
        res = db.insertMainAnalysisData(m_stockName, m_stockSymbol, mainAnalysisId);
    }


    nofData = ui->tableWidgetTotDividends->rowCount();

    for(int row = 0; row < nofData; row++)
    {
        if(NULL != ui->tableWidgetTotDividends->item(row, 0))
        {
            date = ui->tableWidgetTotDividends->item(row, 0)->text();
            data = ui->tableWidgetTotDividends->item(row, 1)->text();

            date.toInt(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Year is not a number"));
                continue;
            }

            CUtil cu;
            cu.number2double(data, data);
            data.toDouble(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("TotDividends is not a number"));
                continue;
            }
        }
        else
        {
           break;
        }


        res = db.subAnalysisTotDividendsDateExists(date, mainAnalysisId, dateId);

        // Exist anaysis date?
        if(false == res)
        {
            res = db.insertSubAnalysisTotDividendsDate(date, mainAnalysisId, dateId);
        }


        if(true == res)
        {
           dataIdIsValid = false;

           if( true == db.getSubAnalysisTotDividendsDataId(mainAnalysisId, dateId, inputDataId))
           {
               dataIdIsValid = true;
           }

           res = db.insertSubAnalysisTotDividendsData(dateId, mainAnalysisId, inputDataId, dataIdIsValid,
                                                       data, dataId);
        }
    }

}


/******************************************************************
 *
 * Function:    on_pushButtonSaveRevenue_clicked()
 *
 * Description:
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::on_pushButtonSaveRevenue_clicked()
{

    CDbHndl db;
    bool res;
    int mainAnalysisId;
    QString str;

    bool isValid;
    int dateId;
    int inputDataId;
    int dataId;
    bool dataIdIsValid = false;

    int nofData;
    QString date;
    QString data;


    if((m_stockName.length() < 1) || m_stockSymbol.length() < 1)
    {
        QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Välj aktie"));
        return;
    }


    str = (QString::fromUtf8("Vill du lägga till sub data?\n"));
    str = str + m_stockName;
    str = str + ", ";
    str = str + m_stockSymbol;
    str = str + ", ";
    str = str + m_analysisDate;


    if(QMessageBox::No == QMessageBox::question(this, QString::fromUtf8("Uppdatera databas"), str, QMessageBox::Yes|QMessageBox::No))
    {
        return;
    }

    // Check if this stocksymbol and stockname is already added, if not add it
    res = db.mainAnalysisDataExists(m_stockName, m_stockSymbol, mainAnalysisId);

    if(false == res)
    {
        res = db.insertMainAnalysisData(m_stockName, m_stockSymbol, mainAnalysisId);
    }


    nofData = ui->tableWidgetRevenue->rowCount();

    for(int row = 0; row < nofData; row++)
    {
        if(NULL != ui->tableWidgetRevenue->item(row, 0))
        {
            date = ui->tableWidgetRevenue->item(row, 0)->text();
            data = ui->tableWidgetRevenue->item(row, 1)->text();

            date.toInt(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Year is not a number"));
                continue;
            }

            CUtil cu;
            cu.number2double(data, data);
            data.toDouble(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("TotDividends is not a number"));
                continue;
            }
        }
        else
        {
           break;
        }


        res = db.subAnalysisRevenueDateExists(date, mainAnalysisId, dateId);

        // Exist anaysis date?
        if(false == res)
        {
            res = db.insertSubAnalysisRevenueDate(date, mainAnalysisId, dateId);
        }


        if(true == res)
        {
           dataIdIsValid = false;

           if( true == db.getSubAnalysisRevenueDataId(mainAnalysisId, dateId, inputDataId))
           {
               dataIdIsValid = true;
           }

           res = db.insertSubAnalysisRevenueData(dateId, mainAnalysisId, inputDataId, dataIdIsValid,
                                                       data, dataId);
        }
    }
}



/******************************************************************
 *
 * Function:    on_pushButtonSaveNetIncome_clicked()
 *
 * Description:
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::on_pushButtonSaveNetIncome_clicked()
{

    CDbHndl db;
    bool res;
    int mainAnalysisId;
    QString str;

    bool isValid;
    int dateId;
    int inputDataId;
    int dataId;
    bool dataIdIsValid = false;

    int nofData;
    QString date;
    QString data;


    if((m_stockName.length() < 1) || m_stockSymbol.length() < 1)
    {
        QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Välj aktie"));
        return;
    }


    str = (QString::fromUtf8("Vill du lägga till sub data?\n"));
    str = str + m_stockName;
    str = str + ", ";
    str = str + m_stockSymbol;
    str = str + ", ";
    str = str + m_analysisDate;


    if(QMessageBox::No == QMessageBox::question(this, QString::fromUtf8("Uppdatera databas"), str, QMessageBox::Yes|QMessageBox::No))
    {
        return;
    }

    // Check if this stocksymbol and stockname is already added, if not add it
    res = db.mainAnalysisDataExists(m_stockName, m_stockSymbol, mainAnalysisId);

    if(false == res)
    {
        res = db.insertMainAnalysisData(m_stockName, m_stockSymbol, mainAnalysisId);
    }


    nofData = ui->tableWidgetNetIncome->rowCount();

    for(int row = 0; row < nofData; row++)
    {
        if(NULL != ui->tableWidgetNetIncome->item(row, 0))
        {
            date = ui->tableWidgetNetIncome->item(row, 0)->text();
            data = ui->tableWidgetNetIncome->item(row, 1)->text();

            date.toInt(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Year is not a number"));
                continue;
            }

            CUtil cu;
            cu.number2double(data, data);
            data.toDouble(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("TotDividends is not a number"));
                continue;
            }
        }
        else
        {
           break;
        }


        res = db.subAnalysisTotEarningsDateExists(date, mainAnalysisId, dateId);

        // Exist anaysis date?
        if(false == res)
        {
            res = db.insertSubAnalysisTotEarningsDate(date, mainAnalysisId, dateId);
        }


        if(true == res)
        {
           dataIdIsValid = false;

           if( true == db.getSubAnalysisTotEarningsDataId(mainAnalysisId, dateId, inputDataId))
           {
               dataIdIsValid = true;
           }

           res = db.insertSubAnalysisTotEarningsData(dateId, mainAnalysisId, inputDataId, dataIdIsValid,
                                                       data, dataId);
        }
    }
}



/******************************************************************
 *
 * Function:    on_pushButtonSaveEquityPerShare_clicked()
 *
 * Description:
 *
 *
 *
 *****************************************************************/
void StockAnalysisTab::on_pushButtonSaveEquityPerShare_clicked()
{
    CDbHndl db;
    bool res;
    int mainAnalysisId;
    QString str;

    bool isValid;
    int dateId;
    int inputDataId;
    int dataId;
    bool dataIdIsValid = false;

    int nofData;
    QString date;
    QString data;


    if((m_stockName.length() < 1) || m_stockSymbol.length() < 1)
    {
        QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Välj aktie"));
        return;
    }


    str = (QString::fromUtf8("Vill du lägga till sub data?\n"));
    str = str + m_stockName;
    str = str + ", ";
    str = str + m_stockSymbol;
    str = str + ", ";
    str = str + m_analysisDate;


    if(QMessageBox::No == QMessageBox::question(this, QString::fromUtf8("Uppdatera databas"), str, QMessageBox::Yes|QMessageBox::No))
    {
        return;
    }

    // Check if this stocksymbol and stockname is already added, if not add it
    res = db.mainAnalysisDataExists(m_stockName, m_stockSymbol, mainAnalysisId);

    if(false == res)
    {
        res = db.insertMainAnalysisData(m_stockName, m_stockSymbol, mainAnalysisId);
    }


    nofData = ui->tableWidgetEquityPerShare->rowCount();

    for(int row = 0; row < nofData; row++)
    {
        if(NULL != ui->tableWidgetEquityPerShare->item(row, 0))
        {
            date = ui->tableWidgetEquityPerShare->item(row, 0)->text();
            data = ui->tableWidgetEquityPerShare->item(row, 1)->text();

            date.toInt(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Year is not a number"));
                continue;
            }

            CUtil cu;
            cu.number2double(data, data);
            data.toDouble(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Equity is not a number"));
                continue;
            }
        }
        else
        {
           break;
        }


        res = db.subAnalysisEquityPerShareDateExists(date, mainAnalysisId, dateId);

        // Exist anaysis date?
        if(false == res)
        {
            res = db.insertSubAnalysisEquityPerShareDate(date, mainAnalysisId, dateId);
        }


        if(true == res)
        {
           dataIdIsValid = false;

           if( true == db.getSubAnalysisEquityPerShareDataId(mainAnalysisId, dateId, inputDataId))
           {
               dataIdIsValid = true;
           }

           res = db.insertSubAnalysisEquityPerShareData(dateId, mainAnalysisId, inputDataId, dataIdIsValid,
                                                       data, dataId);
        }
    }
}


/******************************************************************
 *
 * Function:    on_pushButtonSaveEquityPerShare_clicked()
 *
 * Description:
 *
 *  i   = is the discount rate.
    FBV = Future Book Value = Present Book Value * (1+g)^n
    The future book value is
        then discounted at rate i (e.g. 10 year t-bill return)
        to find the present value of what the book value will grow to.

    If you input that into the formula you get:
    IV = DIV * [ (1 - (1/( (1+i)^n) ) )/i] + PBV*((1+g)/(1+i))^n

 *
 *
 *****************************************************************/
void StockAnalysisTab::on_pushButtonCalcIntrinsicValue_clicked()
{
    double intrinsicValue, tenYearNoteYield, yearlyInterestOnEquity, presentEquityPerShare, dividendPerYear;
    double nofYears = 10.0;
    double tmpDivRes;
    double tmpEquityRes;

    double powXData;
    double tmpPow1Res;
    double oneDivPow1;
    double oneMinusoneDivPow1;

    double powYData1;
    double powYData2;
    double powYData3;
    double tmpPow2Res;


    //intrinsicValue = pow(1.05, nofYears);
    //QString str1;
    //str1.sprintf("%.2f", intrinsicValue);
    //ui->lineEditIntrinsicValueResult->setText(str1);

    if(ui->lineEditTenYearNoteYield->text().isEmpty() == true)
    {
        QMessageBox::information(this, QString::fromUtf8("Data saknas"), QString::fromUtf8("Ange räntan för 10 års stadsobligation"));
        return;
    }

    if(ui->lineEditHistoricalInterestOnEquity->text().isEmpty() == true)
    {
        QMessageBox::information(this, QString::fromUtf8("Data saknas"), QString::fromUtf8("Ange förväntad ånge räntan på eget kapital"));
        return;
    }

    if(ui->lineEditCurrEquityPerShare->text().isEmpty() == true)
    {
        QMessageBox::information(this, QString::fromUtf8("Data saknas"), QString::fromUtf8("Ange senast värde på eget kapital/Aktie"));
        return;
    }

    if(ui->lineEditEstimateYearlyDividend->text().isEmpty() == true)
    {
        QMessageBox::information(this, QString::fromUtf8("Data saknas"), QString::fromUtf8("Ange uppskattad årlig utdelning/Aktie"));
        return;
    }

    tenYearNoteYield = ui->lineEditTenYearNoteYield->text().toDouble();
    tenYearNoteYield = tenYearNoteYield / 100.0;
    yearlyInterestOnEquity = ui->lineEditHistoricalInterestOnEquity->text().toDouble();
    yearlyInterestOnEquity = yearlyInterestOnEquity / 100.0;
    presentEquityPerShare = ui->lineEditCurrEquityPerShare->text().toDouble();
    dividendPerYear = ui->lineEditEstimateYearlyDividend->text().toDouble();

    // IV = C * [1-[1/(1+I)^n]]/I + M/(1+I)^n

    // IV = DIV * [ (1 - (1/( (1+i)^n) ) )/i] + PBV*((1+g)/(1+i))^n

    //              DIV         * [ (1   - (1   / (    (1  + i              )^n       ) ) )/ i]
    powXData = (1.0 + tenYearNoteYield);
    tmpPow1Res = pow(powXData,nofYears);
    oneDivPow1 = (1.0 / tmpPow1Res );
    oneMinusoneDivPow1 = (1.0 - oneDivPow1);
    tmpDivRes = oneMinusoneDivPow1 / tenYearNoteYield;
    tmpDivRes = dividendPerYear * tmpDivRes;


    powYData1 = (1.0 + yearlyInterestOnEquity);
    powYData2 = (1.0 + tenYearNoteYield);
    powYData3 = powYData1 / powYData2;

    QString str1;
    str1.sprintf("%.2f", powYData3);
    ui->lineEditIntrinsicValueResult->setText(str1);


    tmpPow2Res = pow(powYData3, nofYears);
    tmpEquityRes = presentEquityPerShare * tmpPow2Res;


    //                PBV                *((1   + g                     ) /    (1   + i               ))^n
    intrinsicValue = tmpDivRes + tmpEquityRes;
    QString str;
    str.sprintf("%.2f", intrinsicValue);
    ui->lineEditIntrinsicValueResult->setText(str);

}





/*******************************************************************
 *
 * Function:    calcLeastSqrtFit()
 *
 * Description:
 *
 *******************************************************************/
bool StockAnalysisTab::
calcLeastSqrtFit(SubAnalysDataST *dataArr,
                 int nofArrData,
                 double &k,
                 double &m,
                 double &minX,
                 double &maxX)
{
    CUtil cu;
    CDbHndl db;
    int nofData = 0;
    double meanXSum = 0;
    double meanYSum = 0;
    double prodXXSum = 0;
    double prodYYSum = 0;
    double prodXYSum = 0;
    double x;
    double y;
    bool minMaxXIsInit = false;


    db.init1dLeastSrq(nofData, meanXSum, meanYSum, prodXXSum, prodYYSum, prodXYSum);


    if(nofArrData < 2)
    {
        return false;
    }

    for(int i = 0 ; i < nofArrData ; i++)
    {
        if(true == cu.number2double(dataArr[i].date, x) && true == cu.number2double(dataArr[i].data, y))
        {
            //x = (double) (nofTopItems - i);
            qDebug() << "xx" << x << "yy" << y;
            db.gather1dLeastSrqData(x,
                                    y,
                                    nofData,
                                    meanXSum,
                                    meanYSum,
                                    prodXXSum,
                                    prodYYSum,
                                    prodXYSum);

            if(minMaxXIsInit == false)
            {
                minMaxXIsInit = true;
                maxX = x;
                minX = x;
            }
            else
            {
                if(minX > x)
                    minX = x;

                if(maxX < x)
                    maxX = x;
            }
        }
    }


    if(nofData < 2)
    {
        return false;
    }


    if(false == db.calc1dLeastSrqFitParams(nofData, meanXSum, meanYSum, prodXXSum, prodYYSum, prodXYSum, m, k))
    {
       return false;
    }

    qDebug() << "k" << k << "m" << m;

    return true;
}


/*******************************************************************
 *
 * Function:    calcTotSubdataForIntrinsicValue()
 *
 * Description:
 *
 *******************************************************************/
void StockAnalysisTab::calcTotSubdataForIntrinsicValue(void)
{
    int i = 0;
    double diff = 0.0;
    double totDividend = 0.0;
    double totEarning = 0.0;
    bool tooMuchDebt = false;



    if(m_nofEquityPerShareData > 1)
    {
        for(i = 1; i < m_nofEquityPerShareData; i++)
        {
            diff += (m_equityPerShareArr[i].data.toDouble() - m_equityPerShareArr[i-1].data.toDouble());
        }
    }

    if(m_nofDividendArrData > 1)
    {
        for(i = 1; i < m_nofDividendArrData; i++)
        {
            totDividend += m_dividendDataArr[i].data.toDouble();
        }
    }

    if(m_nofEarningsArrData > 1)
    {
        for(i = 1; i < m_nofEarningsArrData; i++)
        {
            totEarning += m_earningsDataArr[i].data.toDouble();
        }
    }

    QString str;
    str.sprintf("%.2f", totEarning);
    ui->lineEditTotErningsPerShare->setText(str);

    str.sprintf("%.2f", diff);
    ui->lineEditTotEquityIncPerShare->setText(str);

    str.sprintf("%.2f", totDividend);
    ui->lineEditTotDividendPerShare->setText(str);


    if(m_nofEquityPerShareData > 1 && m_nofDividendArrData > 1 && m_nofEarningsArrData > 1)
    {
        QString str;
        if(totEarning > (diff + totDividend))
        {
            ui->labelWarningEps->setPalette(*m_blue_palette);
            str = str.sprintf("OK uttag av vinst OK %.2f > %.2f + %2.f", totEarning, diff, totDividend);
            ui->labelWarningEps->setText(str);
        }
        else
        {
            QString str1;
            ui->labelWarningEps->setPalette(*m_red_palette);
            str = QString::fromUtf8("Högt uttag av vinst: ");
            str1.sprintf("%.2f < %.2f + %.2f", totEarning, diff, totDividend);
            str += str1;
            ui->labelWarningEps->setText(str);
        }
    }


    if(m_nofTotalCurrentAssetsArrData == m_nofTotalLiabilitiesData && m_nofTotalLiabilitiesData > 1)
    {
        for(int i = 1; i < m_nofTotalLiabilitiesData; i++)
        {
            if((m_totalCurrentAssetsArr[i].data.toDouble() / m_totalLiabilitiesArr[i].data.toDouble()) < 1.0 )
            {
                tooMuchDebt = true;
                break;
            }
        }


        if(tooMuchDebt == true)
        {
            ui->labelWarningDebt->setPalette(*m_red_palette);
            str = QString::fromUtf8("Höga Skulder: Kortfrisktiga tillgångar/Tot skulder < 1");
            ui->labelWarningDebt->setText(str);
        }
        else
        {
            ui->labelWarningDebt->setPalette(*m_blue_palette);
            str = QString::fromUtf8("Skulder OK: Kortfrisktiga tillgångar/skulder >= 1");
            ui->labelWarningDebt->setText(str);
        }
    }
 }



/*******************************************************************
 *
 * Function:    on_pushButtonCalcYearlyIntrestRateOnEquity_clicked()
 *
 * Description:
 *
 *******************************************************************/
void StockAnalysisTab::on_pushButtonCalcYearlyIntrestRateOnEquity_clicked()
{
    CUtil cu;
    double k, m, minX, maxX, minY, maxY, growthRate = 0;
    double nofYears;
    QString trendLineGrowth;

    if(m_nofEquityPerShareData <  1)
    {
        return;
    }

    nofYears = (double) (m_nofEquityPerShareData -1);

    calcLeastSqrtFit(m_equityPerShareArr, m_nofEquityPerShareData, k, m, minX, maxX);


    minY = k * minX + m;
    maxY = k * maxX + m;

    double x;
    double y;
    SubAnalysDataST pdataArr[2000];

    x = minX;
    for(int cnt = 0; cnt < m_nofEquityPerShareData + 10; cnt++)
    {
        y = k * x + m;
        pdataArr[cnt].date.sprintf("%.2f", x);
        pdataArr[cnt].data.sprintf("%.2f", y);
        x = x + 1;
    }

   plotEquityPerShareData(pdataArr, (m_nofEquityPerShareData + 5), true);


    if(true == cu.annualGrowthRate(minY, maxY, (nofYears), growthRate))
    {
        growthRate = growthRate - 1;
        growthRate = growthRate * 100;
        qDebug() << growthRate;


        trendLineGrowth.sprintf("%.2f", growthRate);
        ui->lineEditHistoricalInterestOnEquity->setText(trendLineGrowth);
    }
    else
    {
        trendLineGrowth.sprintf("  ");
        ui->lineEditHistoricalInterestOnEquity->setText(trendLineGrowth);
    }
}





/*******************************************************************
 *
 * Function:
 *
 * Description:
 *
 *
 *
 *******************************************************************/
void StockAnalysisTab::plotEquityPerShareData(SubAnalysDataST *dataArr, int nofData, bool plotPrediction)
{
    CExtendedQwtPlot eqp;
    CYahooStockPlotUtil cyspu;
    int index = 0;


    if(plotPrediction == true)
    {
        index = 1;
    }
    else
    {
        m_qwtEquityPlotData.stock[1].data.detach();
        m_qwtEquityPlotData.stock[1].data.setData(NULL);
    }

    m_qwtEquityPlotData.stock[index].data.detach();
    m_qwtEquityPlotData.stock[index].data.setData(NULL);

    cyspu.removePlotData(m_qwtEquityPlotData, index, ui->qwtPlot_10);

    QString title;
    title = QString::fromUtf8("Eget kapital/Aktie (Historiskt & beräknat)");
    eqp.setPlotTitle(ui->qwtPlot_10, title);


    m_qwtEquityPlotData.axis.minMaxIsInit = false;

    for(int i = 0; i < nofData; i++)
    {
        m_x[i] = dataArr[i].date.toDouble();
        m_y[i] = dataArr[i].data.toDouble();
        cyspu.updateMinMaxAxis(m_qwtEquityPlotData.axis, m_x[i], m_y[i]);
    }

    int nofXTicks;
    m_qwtEquityPlotData.stock[index].data.setSamples(m_x, m_y, nofData);
    ui->qwtPlot_10->setAxisMaxMinor(QwtPlot::xBottom, 2);
    nofXTicks = nofData - 1;
    if(nofData - 1 >= 10)
    {
        nofXTicks = (nofData / 2) + 1;
    }
    ui->qwtPlot_10->setAxisMaxMajor(QwtPlot::xBottom, nofXTicks);

    if(index == 1)
    {
        m_qwtEquityPlotData.stock[index].data.setPen(QPen(Qt::blue, 2));
        m_qwtEquityPlotData.stock[index].data.setSymbol( new QwtSymbol(QwtSymbol::Ellipse,                                                            Qt::blue,
                                                                     QPen( Qt::blue ),
                                                                      QSize( 7, 7 ) ) );
    }
    else
    {
        m_qwtEquityPlotData.stock[index].data.setPen(QPen(Qt::black, 2));
        m_qwtEquityPlotData.stock[index].data.setSymbol( new QwtSymbol(QwtSymbol::Ellipse,                                                            Qt::blue,
                                                                     QPen( Qt::black ),
                                                                      QSize( 7, 7 ) ) );
    }

    cyspu.plotData(m_qwtEquityPlotData, ui->qwtPlot_10, index, true);
    m_qwtEquityPlotData.stock[index].data.attach(ui->qwtPlot_10);
    ui->qwtPlot_10->replot();
}





#define STOCK_ANALYSIS_IMAGE_BASE_PATH QString::fromUtf8("database/AnalysDoc/htmlReports/MinaAnalyser/Images/")

/*******************************************************************
 *
 * Function:        makeAnalysPlotDirectory()
 *
 * Description:     Create directory where plot images is stored.
 *
 *
 *
 *******************************************************************/
bool StockAnalysisTab::makeAnalysPlotDirectory(QString stockname, QString date)
{
    CUtil cu;
    QString propStockname;
    QString propDate;
    QString path = STOCK_ANALYSIS_IMAGE_BASE_PATH;


    // Remove improper characters
    cu.createProperFilname(stockname, propStockname, false);
    cu.createProperFilname(date, propDate, false);

    // Make stockname directory name
    m_stocknameImgDir = path;
    m_stocknameImgDir += propStockname;

    // Make date directory name
    m_dateImgDir = m_stocknameImgDir;
    m_dateImgDir += "/";
    m_dateImgDir += propDate;


    if(false == cu.createFileDriectory(m_stocknameImgDir))
    {
        QMessageBox::information(NULL,
                                 QString::fromUtf8("Error"),
                                 QString::fromUtf8("Fail to create stockname directory"));
        return false;
    }


    if(false == cu.createFileDriectory(m_dateImgDir))
    {
        QMessageBox::information(NULL,
                                 QString::fromUtf8("Error"),
                                 QString::fromUtf8("Fail to create date directory"));
        return false;
    }

    return true;
}


/*******************************************************************
 *
 * Function:        clearGUIIntrinsicValue()
 *
 * Description:
 *
 *
 *
 *******************************************************************/
void StockAnalysisTab::saveAnalysisPlotAsImages(void)
{
    QString filename;
    CExtendedQwtPlot ceqp;

    if(m_stocknameImgDir.length() < 5 || m_dateImgDir.length() < 5)
    {
        QMessageBox::information(NULL,
                                 QString::fromUtf8("Error"),
                                 QString::fromUtf8("Missing path to analysis image map"));
        return;

    }


    //----------------------------------------------------------------------------------
    // Omsättningstillgångarna / Kortfristiga skulder > 2
    //-----------------------------------------------------------------------------------
    filename = m_dateImgDir;
    filename += "/";
    filename += "image_1.png";
    ceqp.saveQwtPlotAsImage(filename, ui->qwtPlotCurrAssLiab);


    //-----------------------------------------------------------------------------------
    // Omsättningstillgångarna / Totala skulder >= 1
    //-----------------------------------------------------------------------------------
    filename = m_dateImgDir;
    filename += "/";
    filename += "image_2.png";
    ceqp.saveQwtPlotAsImage(filename, ui->qwtPlotCurrAssTotLiab_12);


    //-----------------------------------------------------------------------------------
    // Vinst/Aktie
    //-----------------------------------------------------------------------------------
    filename = m_dateImgDir;
    filename += "/";
    filename += "image_3.png";
    ceqp.saveQwtPlotAsImage(filename, ui->qwtPlotEarningsPerShare_13);


    //-----------------------------------------------------------------------------------
    // Omsättning, Vinst efter skatt
    //-----------------------------------------------------------------------------------
    filename = m_dateImgDir;
    filename += "/";
    filename += "image_4.png";
    ceqp.saveQwtPlotAsImage(filename, ui->qwtPlotRevenueEarnings_17);

    //-----------------------------------------------------------------------------------
    // Utdelning
    //-----------------------------------------------------------------------------------
    filename = m_dateImgDir;
    filename += "/";
    filename += "image_5.png";
    ceqp.saveQwtPlotAsImage(filename, ui->qwtPlotDiv_14);


    //-----------------------------------------------------------------------------------
    // Vinst/Utdelning
    //-----------------------------------------------------------------------------------
    filename = m_dateImgDir;
    filename += "/";
    filename += "image_6.png";
    ceqp.saveQwtPlotAsImage(filename, ui->qwtPlotEarningDiv_16);


    //-----------------------------------------------------------------------------------
    // Operativt kassaflöde - CAPEX - Utdelning > 0
    //-----------------------------------------------------------------------------------
    filename = m_dateImgDir;
    filename += "/";
    filename += "image_7.png";
    ceqp.saveQwtPlotAsImage(filename, ui->qwtPlotCashflow1_15);


    //-----------------------------------------------------------------------------------
    // Operativt kassaflöde, CAPEX, Utdelning
    //-----------------------------------------------------------------------------------
    filename = m_dateImgDir;
    filename += "/";
    filename += "image_8.png";
    ceqp.saveQwtPlotAsImage(filename, ui->qwtPlotCashFlow2_18);


    //-----------------------------------------------------------------------------------
    // Vinsttillväxt (%)
    //-----------------------------------------------------------------------------------
    filename = m_dateImgDir;
    filename += "/";
    filename += "image_9.png";
    ceqp.saveQwtPlotAsImage(filename, ui->qwtPlot_19);



    //-----------------------------------------------------------------------------------
    // Vinstmarginal (%) (Vinst efter skatt / Omsättning)
    //-----------------------------------------------------------------------------------
    filename = m_dateImgDir;
    filename += "/";
    filename += "image_10.png";
    ceqp.saveQwtPlotAsImage(filename, ui->qwtPlotProfitMargin);

    //-----------------------------------------------------------------------------------
    // Avkastning på det egna kapitalet (%) [Vinst/Eget kapital]
    //-----------------------------------------------------------------------------------
    filename = m_dateImgDir;
    filename += "/";
    filename += "image_11.png";
    ceqp.saveQwtPlotAsImage(filename, ui->qwtPlotEquityMargin_22);


    //-----------------------------------------------------------------------------------
    // Totala Skulder, Eget kapital, Vinst, Utdelning
    //-----------------------------------------------------------------------------------
    filename = m_dateImgDir;
    filename += "/";
    filename += "image_12.png";
    ceqp.saveQwtPlotAsImage(filename, ui->qwtPlotLiabEquityEarningDiv_21);

    //-----------------------------------------------------------------------------------
    // (Intrinsic value) Eget kapital / Aktie
    //-----------------------------------------------------------------------------------
    filename = m_dateImgDir;
    filename += "/";
    filename += "image_13.png";
    ceqp.saveQwtPlotAsImage(filename, ui->qwtPlot_10);

}





/*******************************************************************
 *
 * Function:        clearGUIIntrinsicValue()
 *
 * Description:
 *
 *
 *
 *******************************************************************/
void StockAnalysisTab::initAllAnalysisPlots(void)
{
    QString plotHeader;
    QString legendText;
    QColor canvasColor = Qt::white;
    QwtSymbol legendSymbol = QwtSymbol::Rect;
    QColor legendColor = Qt::blue;
    QwtPlot::LegendPosition location = QwtPlot::BottomLegend; //QwtPlot::TopLegend;
    int legendSize = 10;
    CExtendedQwtPlot eqp;





    //===================================================================================
    // Trading company industrial companies
    //===================================================================================

    //----------------------------------------------------------------------------------
    // Omsättningstillgångarna / Kortfristiga skulder > 2
    //-----------------------------------------------------------------------------------

    plotHeader = QString::fromUtf8("Omsättningstillgångarna / Kortfristiga skulder > 2");
    legendText = QString::fromUtf8("OT/KS");

    initAnalysisPlot(ui->qwtPlotCurrAssLiab, plotHeader, canvasColor, legendText, legendSymbol, legendColor,
                             location, legendSize);

    //-----------------------------------------------------------------------------------
    // Omsättningstillgångarna / Totala skulder >= 1
    //-----------------------------------------------------------------------------------
    plotHeader = QString::fromUtf8("Omsättningstillgångarna / Totala skulder >= 1");
    legendText = QString::fromUtf8("OT/TS");
    legendSymbol = QwtSymbol::Ellipse;

    initAnalysisPlot(ui->qwtPlotCurrAssTotLiab_12, plotHeader, canvasColor, legendText, legendSymbol, legendColor,
                             location, legendSize);


    //-----------------------------------------------------------------------------------
    // Vinst/Aktie
    //-----------------------------------------------------------------------------------
    plotHeader = QString::fromUtf8("Vinst/Aktie");
    legendText = QString::fromUtf8("V/A");

    initAnalysisPlot(ui->qwtPlotEarningsPerShare_13, plotHeader, canvasColor, legendText, legendSymbol, legendColor,
                             location, legendSize);


    //-----------------------------------------------------------------------------------
    // Omsättning, Vinst efter skatt
    //-----------------------------------------------------------------------------------
    plotHeader = QString::fromUtf8("Omsättning, Vinst efter skatt");
    legendText = QString::fromUtf8("O, V");

    initAnalysisPlot(ui->qwtPlotRevenueEarnings_17, plotHeader, canvasColor, legendText, legendSymbol, legendColor,
                             location, legendSize);


    //-----------------------------------------------------------------------------------
    // Utdelning
    //-----------------------------------------------------------------------------------
    plotHeader = QString::fromUtf8("Utdelning");
    legendText = QString::fromUtf8("UT");

    initAnalysisPlot(ui->qwtPlotDiv_14, plotHeader, canvasColor, legendText, legendSymbol, legendColor,
                             location, legendSize);




    //-----------------------------------------------------------------------------------
    // Vinst/Utdelning
    //-----------------------------------------------------------------------------------
    plotHeader = QString::fromUtf8("Vinst/Utdelning");
    legendText = QString::fromUtf8("V/UT");

    initAnalysisPlot(ui->qwtPlotEarningDiv_16, plotHeader, canvasColor, legendText, legendSymbol, legendColor,
                             location, legendSize);


    //-----------------------------------------------------------------------------------
    // Operativt kassaflöde - CAPEX - Utdelning > 0  1(2)
    //-----------------------------------------------------------------------------------
    plotHeader = QString::fromUtf8("Operativt kassaflöde - CAPEX - Utdelning > 0");
    legendText = QString::fromUtf8("Op kassaflöde");

    initAnalysisPlot(ui->qwtPlotCashflow1_15, plotHeader, canvasColor, legendText, legendSymbol, legendColor,
                             location, legendSize);


    //-----------------------------------------------------------------------------------
    // Operativt kassaflöde, CAPEX, Utdelning   2(2)
    //-----------------------------------------------------------------------------------
    legendColor = Qt::blue;
    plotHeader = QString::fromUtf8("Operativt kassaflöde - CAPEX - Utdelning > 0");
    legendText = QString::fromUtf8("Op kassaflöde");
    eqp.setRightLegend(ui->qwtPlotCashFlow2_18);


    initAnalysisPlot(ui->qwtPlotCashFlow2_18, plotHeader, canvasColor, legendText, legendSymbol, legendColor,
                     location, legendSize);

    legendText = legendText.fromUtf8("CAPEX");
    legendSymbol = QwtSymbol::Rect;
    legendColor = Qt::magenta;
    legendSize = 10;

    eqp.setLegendSymbol(ui->qwtPlotCashFlow2_18, legendText, legendSymbol, legendColor, legendSize);


    legendText = legendText.fromUtf8("Utdelning");
    legendSymbol = QwtSymbol::Rect;
    legendColor = Qt::red;
    legendSize = 10;

    eqp.setLegendSymbol(ui->qwtPlotCashFlow2_18, legendText, legendSymbol, legendColor, legendSize);
    legendColor = Qt::blue;


    //-----------------------------------------------------------------------------------
    // Vinsttillväxt (%)
    //-----------------------------------------------------------------------------------
    plotHeader = QString::fromUtf8("Vinsttillväxt (%)");
    legendText = QString::fromUtf8("Vt");

    initAnalysisPlot(ui->qwtPlot_19, plotHeader, canvasColor, legendText, legendSymbol, legendColor,
                             location, legendSize);



    //-----------------------------------------------------------------------------------
    // Vinstmarginal (%) (Vinst efter skatt / Omsättning)
    //-----------------------------------------------------------------------------------

    plotHeader = QString::fromUtf8("Vinstmarginal (%) (Vinst efter skatt / Omsättning)");
    legendText = QString::fromUtf8("Vm");

    initAnalysisPlot(ui->qwtPlotProfitMargin, plotHeader, canvasColor, legendText, legendSymbol, legendColor,
                             location, legendSize);



    //-----------------------------------------------------------------------------------
    // Avkastning på det egna kapitalet (%) [Vinst/Eget kapital]
    //-----------------------------------------------------------------------------------
    plotHeader = QString::fromUtf8("Avkastning på det egna kapitalet (%) [Vinst/Eget kapital]");
    legendText = QString::fromUtf8("Ak/Ek");

    initAnalysisPlot(ui->qwtPlotEquityMargin_22, plotHeader, canvasColor, legendText, legendSymbol, legendColor,
                             location, legendSize);


    //-----------------------------------------------------------------------------------
    // Totala Skulder, Eget kapital, Vinst, Utdelning
    //-----------------------------------------------------------------------------------
    plotHeader = QString::fromUtf8("Totala Skulder, Eget kapital, Vinst, Utdelning");
    legendText = QString::fromUtf8("Totala Skulder");
    legendColor = Qt::red;
    legendSize = 10;

    ui->qwtPlotLiabEquityEarningDiv_21->insertLegend(NULL);

    initAnalysisPlot(ui->qwtPlotLiabEquityEarningDiv_21, plotHeader, canvasColor, legendText, legendSymbol, legendColor,
                             location, legendSize);

  //  ui->qwtPlotLiabEquityEarningDiv_21->insertLegend(NULL);




    //-----------------------------------------------------------------------------------
    // Totala Skulder, Eget kapital, Vinst, Utdelning
    //-----------------------------------------------------------------------------------

    //ui->qwtPlotLiabEquityEarningDiv_21->insertLegend(NULL);

    //eqp.setRightLegend(ui->qwtPlotLiabEquityEarningDiv_21);


    legendText = legendText.fromUtf8("Eget kapital");
    legendSymbol = QwtSymbol::Rect;
    legendColor = Qt::blue;
    legendSize = 10;


    eqp.setLegendSymbol(ui->qwtPlotLiabEquityEarningDiv_21, legendText, legendSymbol, legendColor, legendSize);

    legendText = legendText.fromUtf8("Vinst");
    legendSymbol = QwtSymbol::Rect;
    legendColor = Qt::magenta;
    legendSize = 10;


    eqp.setLegendSymbol(ui->qwtPlotLiabEquityEarningDiv_21, legendText, legendSymbol, legendColor, legendSize);

    legendText = legendText.fromUtf8("Utdelning");
    legendSymbol = QwtSymbol::Rect;
    legendColor = Qt::black;
    legendSize = 10;

    eqp.setLegendSymbol(ui->qwtPlotLiabEquityEarningDiv_21, legendText, legendSymbol, legendColor, legendSize);

}


/*******************************************************************
 *
 * Function:        displayAllAnalysisPlots()
 *
 * Description:
 *
 *
 *
 *******************************************************************/
void StockAnalysisTab::clearAllAnalysisEditCtrls(void)
{
    ui->lineEdiMaxtCurrAssLiab->clear();
    ui->lineEditMinCurrAssLiab->clear();
    ui->lineEditAvgCurrAssLiab->clear();

    //============================
    ui->lineEditMaxtEarningDiv->clear();
    ui->lineEditMintEarningDiv->clear();
    ui->lineEditAvgtEarningDiv->clear();

    // =================
    ui->labelResEarningDiv->clear();

    ui->lineEditAnualEarningGrowth->clear();

    //===================
     ui->lineEditMaxProfitMargin->clear();
     ui->lineEditMinProfitMargin->clear();
     ui->lineEditAvgProfitMargin->clear();

    //==================
    ui->lineEditMaxEquityMargin->clear();
    ui->lineEditMinEquityMargin->clear();
    ui->lineEditAvgEquityMargin->clear();
}


/*******************************************************************
 *
 * Function:        displayAllAnalysisPlots()
 *
 * Description:
 *
 *
 *
 *******************************************************************/
void StockAnalysisTab::displayAllAnalysisPlots(void)
{
    CUtil cu;
    SubAnalysDataST resultArr[MAX_NOF_TOTAL_CURRENT_ASSETS_ARR_DATA];
    int nofDataResultArr;
    int indexToPlot;
    int nofPlotToClear;
    QColor lineColor;
    bool useAutoScale;
    bool skipDenominatorEqZero = true;
    bool convToProcent;
    bool resetMinMaxScale;
    int xScaleStep;
    bool clearLegend = true;
    double min;
    double max;
    double average;
    QString str;
    bool resetMinMaxValue = true;
    double growthRate;




    CYahooStockPlotUtil cyspu;
    m_qwtAllAnalysisPlotData.nofStocksToPlot = CYahooStockPlotUtil::MAX_NOF_PLOT_COLORS;
    cyspu.emtypPlotData(m_qwtAllAnalysisPlotData, clearLegend);

    m_qwtAnalysisPlotDataArr2.nofStocksToPlot = CYahooStockPlotUtil::MAX_NOF_PLOT_COLORS;
    cyspu.emtypPlotData(m_qwtAnalysisPlotDataArr2, clearLegend);




    //===================================================================================
    // Trading company industrial companies
    //===================================================================================

    //-----------------------------------------------------------------------------------
    // Omsättningstillgångarna / Kortfristiga skulder > 2
    //-----------------------------------------------------------------------------------
    // Beräkna kvoten
    if(true == subAnalysisCalcQuotient(resultArr,
                            nofDataResultArr,
                            m_totalCurrentAssetsArr,
                            m_nofTotalCurrentAssetsArrData,
                            m_totalCurrentLiabilitiesArr,
                            m_nofTotalCurrentLiabilitiesData,
                            min,
                            max,
                            average))
    {
        str.clear();
        str.sprintf("%.2f", max);
        ui->lineEdiMaxtCurrAssLiab->clear();
        ui->lineEdiMaxtCurrAssLiab->insert(str);

        str.clear();
        str.sprintf("%.2f", min);
        ui->lineEditMinCurrAssLiab->clear();
        ui->lineEditMinCurrAssLiab->insert(str);

        str.clear();
        str.sprintf("%.2f", average);
        ui->lineEditAvgCurrAssLiab->clear();
        ui->lineEditAvgCurrAssLiab->insert(str);

        str.clear();
        if(min >= (double) 2.0)
        {
            ui->labelResCurrAssetLiab->setPalette(*m_blue_palette);
            str.clear();
            str = str.fromUtf8("Godkänd >= 2");
            ui->labelResCurrAssetLiab->setText(str);
        }
        else
        {
            ui->labelResCurrAssetLiab->setPalette(*m_red_palette);
            str.clear();
            str = QString::fromUtf8("Underkänd < 2");
            ui->labelResCurrAssetLiab->setText(str);
        }
    }




    indexToPlot = 0;
    nofPlotToClear = 0;
    lineColor = Qt::blue;
    useAutoScale = false;
    resetMinMaxValue = true;
    plotLinearReportData(ui->qwtPlotCurrAssLiab,
                         m_qwtAllAnalysisPlotData,
                         useAutoScale,
                         resetMinMaxValue,
                         resultArr,
                         nofDataResultArr,
                         indexToPlot,
                         nofPlotToClear,
                         lineColor);

    //-----------------------------------------------------------------------------------
    // Omsättningstillgångarna / Totala skulder >= 1
    //-----------------------------------------------------------------------------------
    // Beräkna kvoten
    if(true == subAnalysisCalcQuotient(resultArr,
                            nofDataResultArr,
                            m_totalCurrentAssetsArr,
                            m_nofTotalCurrentAssetsArrData,
                            m_totalLiabilitiesArr,
                            m_nofTotalLiabilitiesData,
                            min,
                            max,
                            average))
    {
        QString str;
        str.clear();
        str.sprintf("%.2f", max);
        ui->lineEditMaxCurrAssTotLiab->clear();
        ui->lineEditMaxCurrAssTotLiab->insert(str);
        str.clear();
        str.sprintf("%.2f", min);
        ui->lineEditMinCurrAssTotLiab->clear();
        ui->lineEditMinCurrAssTotLiab->insert(str);
        str.clear();
        str.sprintf("%.2f", average);
        ui->lineEditAvgCurrAssTotLiab->clear();
        ui->lineEditAvgCurrAssTotLiab->insert(str);


        if(min >= (double) 1.0)
        {
            ui->labelResCurrAssetTotLiab->setPalette(*m_blue_palette);
            str.clear();
            str = str.fromUtf8("Godkänd >= 1");
            ui->labelResCurrAssetTotLiab->setText(str);
            str.clear();
        }
        else
        {
            ui->labelResCurrAssetTotLiab->setPalette(*m_red_palette);
            str.clear();
            str = QString::fromUtf8("Underkänd < 1");
            ui->labelResCurrAssetTotLiab->setText(str);
        }
    }


    indexToPlot = 1;
    nofPlotToClear = 0;
    lineColor = Qt::blue;
    useAutoScale = false;
    resetMinMaxValue = true;
    plotLinearReportData(ui->qwtPlotCurrAssTotLiab_12,
                         m_qwtAllAnalysisPlotData,
                         useAutoScale,
                         resetMinMaxValue,
                         resultArr,
                         nofDataResultArr,
                         indexToPlot,
                         nofPlotToClear,
                         lineColor);


    //-----------------------------------------------------------------------------------
    // Vinst/Aktie
    //-----------------------------------------------------------------------------------

    bool leastSqrtFitIsValid = false;
    double k;
    double m;
    double minX;
    double maxX;
    int nofYears;
    QString str1;


    // Calc and display least square fit earning data
    addEarningAndGrowsToTreeWidget(leastSqrtFitIsValid,
                                   k,
                                   m,
                                   minX,
                                   maxX);


    ui->lineEditAvgEarningPerShare->clear();
    ui->lineEditAnualEarningGrowthRate->clear();
    ui->lineEditGrahamsIntrinsicValue->clear();
    if(leastSqrtFitIsValid == true)
    {
        SubAnalysDataST tmpArr[MAX_NOF_EARNINGS_ARR_DATA];
        int nofTmpArrData = 2;

        //----------------------------------------------------------------
        // Calc predicted earning/share plot-line based on least square fit data
        //----------------------------------------------------------------
        double startYear = minX;
        double startY = k * minX + m;
        tmpArr[0].date.sprintf("%.2f", startYear);
        tmpArr[0].data.sprintf("%.2f", startY);

        // Calc end Y-value predicted 5 year in the feature
        double endYear = maxX + 5;
        double endY = k * (maxX + 5) + m;
        tmpArr[1].date.sprintf("%.2f", endYear);
        tmpArr[1].data.sprintf("%.2f", endY);


        //----------------------------------------------------------------
        // Calc annual earning growth rate based on least square fit data
        //----------------------------------------------------------------

        // Calc end Y-value between actual data
        nofYears = (int)(maxX - minX);
        endY = k * maxX + m;

        if(true == cu.annualGrowthRate(startY, endY, nofYears, growthRate))
        {
            growthRate = growthRate - 1;
            growthRate = growthRate * 100;
            qDebug() << growthRate;

            str1.sprintf("%.2f", growthRate);
            ui->lineEditAnualEarningGrowthRate->setText(str1);

            //----------------------------------------------------------------
            // Calc average Earning Per Share and Grahams intrinsic value
            //----------------------------------------------------------------

            if(m_nofEarningsArrData > 0)
            {
                double averageEarningPerShare = 0;

                for(int i = 0; i < m_nofEarningsArrData; i++)
                {
                    averageEarningPerShare += m_earningsDataArr[i].data.toDouble();
                }
                averageEarningPerShare = averageEarningPerShare / (double) m_nofEarningsArrData;

                str1.sprintf("%.2f", averageEarningPerShare);
                ui->lineEditAvgEarningPerShare->setText(str1);

                if((averageEarningPerShare > 0) && (growthRate > 0))
                {
                    double intrinsicValue = averageEarningPerShare * (8.5 + (growthRate * 2));
                    str1.sprintf("%.2f", intrinsicValue);
                    ui->lineEditGrahamsIntrinsicValue->setText(str1);
                }
            }
        }



        indexToPlot = 8;
        nofPlotToClear = 0;
        lineColor = Qt::red;
        useAutoScale = false;
        resetMinMaxValue = true;
        bool hideDataSample = true;
        plotLinearReportData(ui->qwtPlotEarningsPerShare_13,
                             m_qwtAnalysisPlotDataArr2,
                             useAutoScale,
                             resetMinMaxValue,
                             tmpArr,
                             nofTmpArrData,
                             indexToPlot,
                             nofPlotToClear,
                             lineColor,
                             hideDataSample);

    }


    // Plot vinst
    indexToPlot = 7;
    nofPlotToClear = 0;
    lineColor = Qt::blue;
    useAutoScale = false;
    resetMinMaxValue = false;
    plotLinearReportData(ui->qwtPlotEarningsPerShare_13,
                         m_qwtAnalysisPlotDataArr2,
                         useAutoScale,
                         resetMinMaxValue,
                         m_earningsDataArr,
                         m_nofEarningsArrData,
                         indexToPlot,
                         nofPlotToClear,
                         lineColor);




    //-----------------------------------------------------------------------------------
    // Omsättning, Vinst efter skatt 1,(2)
    //-----------------------------------------------------------------------------------
    indexToPlot = 9;
    resetMinMaxScale = true;
    xScaleStep = 1;
    plotYAxisLogData(m_qwtAllAnalysisPlotData, resetMinMaxScale,
                     m_revenueDataArr, m_nofRevenueArrData,
                     ui->qwtPlotRevenueEarnings_17, indexToPlot, Qt::blue, xScaleStep);


    //-----------------------------------------------------------------------------------
    // Omsättning, Vinst efter skatt 2,(2)
    //-----------------------------------------------------------------------------------
    indexToPlot = 10;
    resetMinMaxScale = false;
    xScaleStep = 1;
    plotYAxisLogData(m_qwtAllAnalysisPlotData, resetMinMaxScale,
                     m_totEarningsDataArr, m_nofTotEarningsArrData,
                     ui->qwtPlotRevenueEarnings_17, indexToPlot, Qt::red, xScaleStep);



    //-----------------------------------------------------------------------------------
    // Utdelning
    //-----------------------------------------------------------------------------------
    indexToPlot = 3;
    nofPlotToClear = 0;
    lineColor = Qt::blue;
    useAutoScale = false;

    plotBarGraphReportData(ui->qwtPlotDiv_14,
                                useAutoScale,
                                m_dividendDataArr,
                                m_nofDividendArrData,
                                indexToPlot,
                                nofPlotToClear,
                                lineColor);


    //-----------------------------------------------------------------------------------
    // Vinst/Utdelning
    //-----------------------------------------------------------------------------------

    // Beräkna kvoten
    if(true == subAnalysisCalcQuotient(resultArr,
                            nofDataResultArr,
                            m_earningsDataArr,
                            m_nofEarningsArrData,
                            m_dividendDataArr,
                            m_nofDividendArrData,
                            min,
                            max,
                            average))
    {
        QString str;
        str.clear();
        str.sprintf("%.2f", max);
        ui->lineEditMaxtEarningDiv->clear();
        ui->lineEditMaxtEarningDiv->insert(str);
        str.clear();
        str.sprintf("%.2f", min);
        ui->lineEditMintEarningDiv->clear();
        ui->lineEditMintEarningDiv->insert(str);
        str.clear();
        str.sprintf("%.2f", average);
        ui->lineEditAvgtEarningDiv->clear();
        ui->lineEditAvgtEarningDiv->insert(str);

        str.clear();
        if(min >= (double) 1.9951)
        {
            ui->labelResEarningDiv->setPalette(*m_blue_palette);
            str = str.fromUtf8("Godkänd >= 2");
            ui->labelResEarningDiv->setText(str);
        }
        else
        {
            ui->labelResEarningDiv->setPalette(*m_red_palette);
            str = QString::fromUtf8("Underkänd < 2");
            ui->labelResEarningDiv->setText(str);
        }
    }


    indexToPlot = 4;
    nofPlotToClear = 0;
    lineColor = Qt::blue;
    useAutoScale = false;
    skipDenominatorEqZero = true;
    resetMinMaxValue = true;

    plotLinearReportData(ui->qwtPlotEarningDiv_16,
                         m_qwtAllAnalysisPlotData,
                         useAutoScale,
                         resetMinMaxValue,
                         resultArr,
                         nofDataResultArr,
                         indexToPlot,
                         nofPlotToClear,
                         lineColor);


    //-----------------------------------------------------------------------------------
    // Operativt kassaflöde - CAPEX - Utdelning > 0
    //-----------------------------------------------------------------------------------
    SubAnalysDataST resultArr2[MAX_NOF_CASH_FLOW_CAPEX];
    int nofDataResultArr2;

    // Data is receive
    bool changeSignFirstTerm = false;
    bool changeSignSecondTerm = true;

    // Step 1 (2) res2 = operating Cash Flow - Capex
    subAnalysisCalcDifference(resultArr2,
                              nofDataResultArr2,
                              m_operatingCashFlowArr,
                              m_nofOperatingCashFlowData,
                              m_cashFlowCapexArr,
                              m_nofCashFlowCapexData,
                              changeSignFirstTerm,
                              changeSignSecondTerm);

    changeSignFirstTerm = false;
    changeSignSecondTerm = true;

    // Step 2 (2) Res = operating Cash Flow - Capex - dividens
    subAnalysisCalcDifference(resultArr,
                              nofDataResultArr,
                              resultArr2,
                              nofDataResultArr2,
                              m_totDividensArr,
                              m_nofTotDividensData,
                              changeSignFirstTerm,
                              changeSignSecondTerm);

    indexToPlot = 6;
    nofPlotToClear = 0;
    lineColor = Qt::blue;
    useAutoScale = false;
    skipDenominatorEqZero = true;
    resetMinMaxValue = true;

    plotLinearReportData(ui->qwtPlotCashflow1_15,
                         m_qwtAllAnalysisPlotData,
                          useAutoScale,
                         resetMinMaxValue,
                          resultArr,
                          nofDataResultArr,
                          indexToPlot,
                          nofPlotToClear,
                          lineColor);


    //-----------------------------------------------------------------------------------
    // Vinsttillväxt (%)
    //-----------------------------------------------------------------------------------
    str.clear();
    ui->labelResEarningDiv->clear();

    // Nof data > 2
    if(m_nofEarningsArrData > 2)
    {
        cu.annualGrowthRate(m_earningsDataArr[0].data.toDouble(),
                            m_earningsDataArr[m_nofEarningsArrData - 1].data.toDouble(),
                            (double) (m_nofEarningsArrData - 1),
                            growthRate);

        growthRate = growthRate - 1.0;
        growthRate = growthRate * 100;

        str.sprintf("%.2f (%%)", (growthRate));
        ui->lineEditAnualEarningGrowth->setText(str);

        // We are rounding to two digits
        if(growthRate >= (double) 2.9951)
        {
            ui->lineEditAnualEarningGrowth->setPalette(*m_blue_palette);
        }
        else
        {
            ui->lineEditAnualEarningGrowth->setPalette(*m_red_palette);
        }

        ui->labelResEarningDiv->setText(str);
    }



    // Add zero line to plot
     SubAnalysDataST  zeroLineDataArr[MAX_NOF_EARNINGS_ARR_DATA];
     int              m_nofZeroLineArrData;

     QString data("0.0");
     m_nofZeroLineArrData = m_nofEarningsArrData;
     for(int zz = 0; zz < m_nofEarningsArrData; zz++)
     {
         zeroLineDataArr[zz].date = m_earningsDataArr[zz].date;
         zeroLineDataArr[zz].data = data;
     }

     indexToPlot = 12;
     nofPlotToClear = 0;
     lineColor = Qt::black;
     useAutoScale = false;
     bool hideDataSample = true;
     skipDenominatorEqZero = true;
     resetMinMaxValue = true;

     plotLinearReportData(ui->qwtPlot_19,
                          m_qwtAllAnalysisPlotData,
                           useAutoScale,
                          resetMinMaxValue,
                           zeroLineDataArr,
                           m_nofZeroLineArrData,
                           indexToPlot,
                           nofPlotToClear,
                           lineColor,
                          hideDataSample);


    subAnalysisDisplayGraphData sadgd;
    sadgd.subAnalysisOneArrCalcProcentRationPrevCurrSlot(m_earningsDataArr,
                                                  m_nofEarningsArrData,
                                                  resultArr,
                                                  nofDataResultArr,
                                                  skipDenominatorEqZero);    
    indexToPlot = 5;
    nofPlotToClear = 0;
    lineColor = Qt::blue;
    useAutoScale = false;

    plotBarGraphReportData(ui->qwtPlot_19,
                                useAutoScale,
                                resultArr,
                                nofDataResultArr,
                                indexToPlot,
                                nofPlotToClear,
                                lineColor);




    //-----------------------------------------------------------------------------------
    // Vinstmarginal (%) (Vinst efter skatt / Omsättning)
    //-----------------------------------------------------------------------------------
    // Beräkna kvoten
    skipDenominatorEqZero = true;
    convToProcent = true;

    if(true == subAnalysisCalcQuotient(resultArr,
                            nofDataResultArr,
                            m_totEarningsDataArr,
                            m_nofTotEarningsArrData,
                            m_revenueDataArr,
                            m_nofRevenueArrData,
                            min,
                            max,
                            average,
                            skipDenominatorEqZero,
                            convToProcent))
        {
            QString str;
            str.clear();
            str.sprintf("%.2f", max);
            ui->lineEditMaxProfitMargin->clear();
            ui->lineEditMaxProfitMargin->insert(str);
            str.clear();
            str.sprintf("%.2f", min);
            ui->lineEditMinProfitMargin->clear();
            ui->lineEditMinProfitMargin->insert(str);
            str.clear();
            str.sprintf("%.2f", average);
            ui->lineEditAvgProfitMargin->clear();
            ui->lineEditAvgProfitMargin->insert(str);
        }



    indexToPlot = 7;
    nofPlotToClear = 0;
    lineColor = Qt::blue;
    useAutoScale = false;
    resetMinMaxValue = true;

    plotLinearReportData(ui->qwtPlotProfitMargin,
                         m_qwtAllAnalysisPlotData,
                          useAutoScale,
                          resetMinMaxValue,
                          resultArr,
                          nofDataResultArr,
                          indexToPlot,
                          nofPlotToClear,
                          lineColor);

    //-----------------------------------------------------------------------------------
    // Avkastning på det egna kapitalet (%) [Vinst/Eget kapital]
    //-----------------------------------------------------------------------------------
    // Beräkna kvoten

    skipDenominatorEqZero = true;
    convToProcent = true;

    if(true == subAnalysisCalcQuotient(resultArr,
                            nofDataResultArr,
                            m_totEarningsDataArr,
                            m_nofTotEarningsArrData,
                            m_totEquityArr,
                            m_nofTotEquityData,
                            min,
                            max,
                            average,
                            skipDenominatorEqZero,
                            convToProcent))
    {
        QString str;
        str.sprintf("%.2f", max);
        ui->lineEditMaxEquityMargin->clear();
        ui->lineEditMaxEquityMargin->insert(str);
        str.sprintf("%.2f", min);
        ui->lineEditMinEquityMargin->clear();
        ui->lineEditMinEquityMargin->insert(str);
        str.sprintf("%.2f", average);
        ui->lineEditAvgEquityMargin->clear();
        ui->lineEditAvgEquityMargin->insert(str);
    }


    indexToPlot = 8;
    nofPlotToClear = 0;
    lineColor = Qt::blue;
    useAutoScale = false;
    resetMinMaxValue = true;

    plotLinearReportData(ui->qwtPlotEquityMargin_22,
                         m_qwtAllAnalysisPlotData,
                         useAutoScale,
                         resetMinMaxValue,
                         resultArr,
                         nofDataResultArr,
                         indexToPlot,
                         nofPlotToClear,
                         lineColor);


    //-----------------------------------------------------------------------------------
    // Totala Skulder, Eget kapital, Vinst, Utdelning
    //-----------------------------------------------------------------------------------
    indexToPlot = 0;
    resetMinMaxScale = true;
    xScaleStep = 1;
    plotYAxisLogData(m_qwtAnalysisPlotDataArr2, resetMinMaxScale,
                     m_totalCurrentLiabilitiesArr, m_nofTotalLiabilitiesData,
                     ui->qwtPlotLiabEquityEarningDiv_21, indexToPlot, Qt::red, xScaleStep);


    //-----------------------------------------------------------------------------------
    // Totala Skulder, Eget kapital, Vinst, Utdelning
    //-----------------------------------------------------------------------------------
    indexToPlot = 1;
    resetMinMaxScale = false;
    xScaleStep = 1;
    plotYAxisLogData(m_qwtAnalysisPlotDataArr2, resetMinMaxScale,
                     m_totEquityArr, m_nofTotEquityData,
                     ui->qwtPlotLiabEquityEarningDiv_21, indexToPlot, Qt::blue, xScaleStep);

    //-----------------------------------------------------------------------------------
    // Totala Skulder, Eget kapital, Vinst, Utdelning
    //-----------------------------------------------------------------------------------
    indexToPlot = 2;
    resetMinMaxScale = false;
    xScaleStep = 1;
    plotYAxisLogData(m_qwtAnalysisPlotDataArr2, resetMinMaxScale,
                     m_totEarningsDataArr, m_nofTotEarningsArrData,
                     ui->qwtPlotLiabEquityEarningDiv_21, indexToPlot, Qt::magenta, xScaleStep);


    //-----------------------------------------------------------------------------------
    // Totala Skulder, Eget kapital, Vinst, Utdelning
    //-----------------------------------------------------------------------------------
    indexToPlot = 3;
    resetMinMaxScale = false;
    xScaleStep = 1;
    bool changeSignYdata = true;
    plotYAxisLogData(m_qwtAnalysisPlotDataArr2, resetMinMaxScale,
                     m_totDividensArr, m_nofTotDividensData,
                     ui->qwtPlotLiabEquityEarningDiv_21, indexToPlot, Qt::black,
                     xScaleStep, changeSignYdata);


    //-------------------------------------------------------
    // Cashflow 2
    //--------------------------------------------------------

    indexToPlot = 4;
    useAutoScale = false;
    resetMinMaxScale = true;
    xScaleStep = 1;
    changeSignYdata = false;
    nofPlotToClear = 0;
    lineColor = Qt::blue;
    double xOffset = -0.25;

    plotBarGraphReportDataWithXOffset(ui->qwtPlotCashFlow2_18,
                                      m_qwtAnalysisPlotDataArr2,
                                      useAutoScale,
                                      m_operatingCashFlowArr,
                                      m_nofOperatingCashFlowData,
                                      indexToPlot,
                                      nofPlotToClear,
                                      lineColor,
                                      changeSignYdata,
                                      xOffset,
                                      resetMinMaxScale);



    //--------------------------------------------------------

    indexToPlot = 5;
    useAutoScale = false;
    resetMinMaxScale = false;
    xScaleStep = 1;
    changeSignYdata = true;
    nofPlotToClear = 0;
    lineColor = Qt::magenta;
    xOffset = 0;


    plotBarGraphReportDataWithXOffset(ui->qwtPlotCashFlow2_18,
                                      m_qwtAnalysisPlotDataArr2,
                                      useAutoScale,
                                      m_cashFlowCapexArr,
                                      m_nofOperatingCashFlowData,
                                      indexToPlot,
                                      nofPlotToClear,
                                      lineColor,
                                      changeSignYdata,
                                      xOffset,
                                      resetMinMaxScale);


    //--------------------------------------------------------
    indexToPlot = 6;
    useAutoScale = false;
    resetMinMaxScale = false;
    xScaleStep = 1;
    changeSignYdata = true;
    nofPlotToClear = 0;
    lineColor = Qt::red;
    xOffset = +0.25;



    plotBarGraphReportDataWithXOffset(ui->qwtPlotCashFlow2_18,
                                      m_qwtAnalysisPlotDataArr2,
                                      useAutoScale,
                                      m_totDividensArr,
                                      m_nofTotDividensData,
                                      indexToPlot,
                                      nofPlotToClear,
                                      lineColor,
                                      changeSignYdata,
                                      xOffset,
                                      resetMinMaxScale);
}


/*******************************************************************
 *
 * Function:        subAnalysisCalcQuotient()
 *
 * Description:
 *              Numerator =     täljare
 *              Denominator =   nämnare
 *
 *******************************************************************/
bool StockAnalysisTab::subAnalysisCalcQuotient(SubAnalysDataST *resultArr,     int &nofDataResultArr,
                                                SubAnalysDataST *numeratorArr, int nofDataNumeratorArr,
                                               SubAnalysDataST *denominatorArr, int nofDataDenominatorArr,
                                               double &min,
                                               double &max,
                                               double &average,
                                               bool skipDenominatorEqZero,
                                               bool convToProcent)
{
    bool res = false;
    double tmpRes;
    double procent = 100.0;
    double result;
    bool isInit = false;

    nofDataResultArr = 0;
    for(int i = 0; i < nofDataNumeratorArr; i++)
    {
        for(int j = 0; j < nofDataDenominatorArr; j++)
        {
            if((skipDenominatorEqZero == true) && (denominatorArr[j].data.toDouble() == 0))
            {
                continue;
            }

            if(numeratorArr[i].date.toInt() == denominatorArr[j].date.toInt())
            {
                resultArr[nofDataResultArr].date = numeratorArr[i].date;

                // Do not divide by zero
                if((denominatorArr[j].data.toDouble() >= 0) &&
                   (denominatorArr[j].data.toDouble() <  0.0001))
                {
                    tmpRes = numeratorArr[i].data.toDouble() / 0.0001;
                }
                else
                {
                    tmpRes = (numeratorArr[i].data.toDouble() / denominatorArr[j].data.toDouble());

                }

                result =tmpRes;
                if(convToProcent == true)
                {
                    result = tmpRes * procent;
                }

                if(isInit == false)
                {
                    isInit = true;
                    min = result;
                    max = result;
                    average = result;
                }
                else
                {
                    if(min > result)
                    {
                        min = result;
                    }

                    if(max < result)
                    {
                        max = result;
                    }
                    average += result;
                }

                resultArr[nofDataResultArr].data.sprintf("%f", result);
                nofDataResultArr++;
                res = true;
            }
        }
    }
    if(nofDataResultArr > 0)
    {
        average = average / (double) nofDataResultArr;
    }
    return res;
}




/*******************************************************************
 *
 * Function:        clearGUIIntrinsicValue()
 *
 * Description:
 *              Numerator =     täljare
 *              Denominator =   nämnare
 *
 *******************************************************************/
void StockAnalysisTab::subAnalysisCalcDifference(SubAnalysDataST *resultArr,     int &nofDataResultArr,
                                                 SubAnalysDataST *firstTermArr, int nofDataFirstTermArr,
                                                 SubAnalysDataST *secondTermArr, int nofDataSecondTermArr,
                                                 bool changeSignFirstTerm, bool changeSignSecondTerm)
{
    double tmpRes;
    double tmpFirstTerm;
    double tmpSeconfTerm;

    nofDataResultArr = 0;
    for(int i = 0; i < nofDataFirstTermArr; i++)
    {
        for(int j = 0; j < nofDataSecondTermArr; j++)
        {
            if(firstTermArr[i].date.toInt() == secondTermArr[j].date.toInt())
            {
                resultArr[nofDataResultArr].date = firstTermArr[i].date;

                tmpFirstTerm = firstTermArr[i].data.toDouble();
                tmpSeconfTerm = secondTermArr[j].data.toDouble();

                if(changeSignFirstTerm == true)
                {
                    tmpFirstTerm = -tmpFirstTerm;
                }

                if(changeSignSecondTerm == true)
                {
                    tmpSeconfTerm = -tmpSeconfTerm;
                }

                tmpRes = (tmpFirstTerm - tmpSeconfTerm);

                resultArr[nofDataResultArr].data.sprintf("%f", tmpRes);
                nofDataResultArr++;
            }
        }
    }
}





/*******************************************************************
 *
 * Function:        clearGUIIntrinsicValue()
 *
 * Description:
 *
 * pos = QwtPlot::TopLegend
 *       QwtPlot::BottomLegend
 *       QwtPlot::RightLegend
 *
 *
 *******************************************************************/
void StockAnalysisTab::initAnalysisPlot(QwtPlot *qwtPlot,
                                                QString plotHeader,
                                                QColor canvasColor,
                                                QString legendText,
                                                QwtSymbol legendSymbol,
                                                QColor legendColor,
                                                QwtPlot::LegendPosition location,
                                                int legendSize)
{
    CExtendedQwtPlot eqp;

    bool enableMinorXGrid = false;
    bool enableMajorXGrid = true;
    bool enableMinorYGrid = false;
    bool enableMajorYGrid = true;



    eqp.setPlotTitle(qwtPlot, plotHeader);
    eqp.setCanvasBackground(qwtPlot, canvasColor);

    eqp.setLegendSymbol(qwtPlot, legendText, legendSymbol, legendColor, legendSize);
    eqp.setLegend(qwtPlot, location);

    eqp.turnOnPlotGrid(qwtPlot, Qt::darkYellow, enableMinorXGrid, enableMajorXGrid,
                       enableMinorYGrid, enableMajorYGrid);

    // Displays x,y coordinate on screen when user moves mouse over the plot.
    eqp.initPlotPicker(qwtPlot);
}


/*******************************************************************
 *
 * Function:        plotBarGraphReportData()
 *
 * Description:
 *
 *
 *
 *******************************************************************/
void StockAnalysisTab::plotBarGraphReportData(QwtPlot *qwtPlot,
                                            bool useAutoScale,
                                            SubAnalysDataST *dataArr,
                                            int nofData,
                                            int indexToPlot,
                                            int nofPlotToClear,
                                            QColor lineColor)
{
    CYahooStockPlotUtil cyspu;

    // Do not excide array bondary
    if((indexToPlot >= CYahooStockPlotUtil::MAX_NOF_PLOT_COLORS) ||
       (nofPlotToClear >= CYahooStockPlotUtil::MAX_NOF_PLOT_COLORS))
    {
        return;
    }


    for(int i = 0; i < nofPlotToClear; i++)
    {
        cyspu.removePlotData(m_qwtAllAnalysisPlotData, i, qwtPlot);
    }


    m_qwtAllAnalysisPlotData.nofStocksToPlot = 1;
    m_qwtAllAnalysisPlotData.axis.minMaxIsInit = false;

    for(int i = 0; i < nofData; i++)
    {
        m_x[i] = dataArr[i].date.toDouble();
        m_y[i] = dataArr[i].data.toDouble();
        cyspu.updateMinMaxAxis(m_qwtAllAnalysisPlotData.axis, m_x[i], m_y[i]);
    }

    // Make bars fit on plot
    m_qwtAllAnalysisPlotData.axis.minX -= 0.5;
    m_qwtAllAnalysisPlotData.axis.maxX += 0.5;


    if((useAutoScale == false) && (m_qwtAllAnalysisPlotData.axis.minY > 0))
    {
        m_qwtAllAnalysisPlotData.axis.minY = 0;
    }

    // Data to be plotted
    m_qwtAllAnalysisPlotData.stock[indexToPlot].data.setSamples(m_x, m_y, nofData);

    // set plot grid
    qwtPlot->setAxisMaxMinor(QwtPlot::xBottom, 1);
    qwtPlot->setAxisMaxMajor(QwtPlot::xBottom, nofData);


    // Set plot to bar graph
    m_qwtAllAnalysisPlotData.stock[indexToPlot].data.setStyle(QwtPlotCurve::Sticks);


    // Importent to use Qt::FlatCap together with QwtPlotCurve::Sticks if width
    // is larger then 4 otherwise you get wrong y - value
    QPen pen;
    pen.setWidth(20);
    pen.setBrush(lineColor);
    pen.setCapStyle(Qt::FlatCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setStyle(Qt::SolidLine);

    m_qwtAllAnalysisPlotData.stock[indexToPlot].data.setPen(pen);
    m_qwtAllAnalysisPlotData.stock[indexToPlot].data.setBaseline(0);


    cyspu.plotData(m_qwtAllAnalysisPlotData, qwtPlot, indexToPlot, useAutoScale);

    // Disable legend when we plot to not get two
    m_qwtAllAnalysisPlotData.stock[indexToPlot].data.setItemAttribute(QwtPlotItem::Legend, false);
    m_qwtAllAnalysisPlotData.stock[indexToPlot].data.attach(qwtPlot);
    qwtPlot->replot();
}

/*******************************************************************
 *
 * Function:        plotBarGraphReportData()
 *
 * Description:
 *
 *
 *
 *******************************************************************/
void StockAnalysisTab::plotBarGraphReportDataWithXOffset(QwtPlot *qwtPlot,
                                                         CYahooStockPlotUtil::PlotData_ST &allPlotData,
                                                         bool useAutoScale,
                                                         SubAnalysDataST *dataArr,
                                                         int nofData,
                                                         int indexToPlot,
                                                         int nofPlotToClear,
                                                         QColor lineColor,
                                                         bool changeSignYdata,
                                                         double xOffset,
                                                         bool initMinMaxData)
{
    CYahooStockPlotUtil cyspu;

    // Do not excide array bondary
    if((indexToPlot >= CYahooStockPlotUtil::MAX_NOF_PLOT_COLORS) ||
       (nofPlotToClear >= CYahooStockPlotUtil::MAX_NOF_PLOT_COLORS))
    {
        return;
    }


    for(int i = 0; i < nofPlotToClear; i++)
    {
        cyspu.removePlotData(allPlotData, i, qwtPlot);
    }


    allPlotData.nofStocksToPlot = 1;

    if(initMinMaxData == true)
    {
        allPlotData.axis.minMaxIsInit = false;
    }
    else
    {
        allPlotData.axis.minMaxIsInit = true;
    }


    for(int i = 0; i < nofData; i++)
    {
        m_x[i] = xOffset + dataArr[i].date.toDouble();
        m_y[i] = dataArr[i].data.toDouble();
        if(changeSignYdata == true)
        {
            m_y[i] = -m_y[i];
        }

        cyspu.updateMinMaxAxis(allPlotData.axis, m_x[i], m_y[i]);
    }

    // Make bars fit on plot
    allPlotData.axis.minX -= 0.5;
    allPlotData.axis.maxX += 0.5;


    if((useAutoScale == false) && (allPlotData.axis.minY > 0))
    {
        allPlotData.axis.minY = 0;
    }

    // Data to be plotted
    allPlotData.stock[indexToPlot].data.setSamples(m_x, m_y, nofData);

    // set plot grid
    qwtPlot->setAxisMaxMinor(QwtPlot::xBottom, 1);
    qwtPlot->setAxisMaxMajor(QwtPlot::xBottom, nofData);


    // Set plot to bar graph
    allPlotData.stock[indexToPlot].data.setStyle(QwtPlotCurve::Sticks);


    // Importent to use Qt::FlatCap together with QwtPlotCurve::Sticks if width
    // is larger then 4 otherwise you get wrong y - value
    QPen pen;
    pen.setWidth(20);
    pen.setBrush(lineColor);
    pen.setCapStyle(Qt::FlatCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setStyle(Qt::SolidLine);

    allPlotData.stock[indexToPlot].data.setPen(pen);
    allPlotData.stock[indexToPlot].data.setBaseline(0);


    cyspu.plotData(allPlotData, qwtPlot, indexToPlot, useAutoScale);

    // Disable legend when we plot to not get two
    allPlotData.stock[indexToPlot].data.setItemAttribute(QwtPlotItem::Legend, false);
    allPlotData.stock[indexToPlot].data.attach(qwtPlot);
    qwtPlot->replot();
}




/*******************************************************************
 *
 * Function:        plotLinearReportData()
 *
 * Description:
 *
 *
 *
 *******************************************************************/
void StockAnalysisTab::plotLinearReportData(QwtPlot *qwtPlot,
                                            CYahooStockPlotUtil::PlotData_ST &qwtAllPlotData,
                                            bool useAutoScale,
                                            bool resetMinMaxValue,
                                            SubAnalysDataST *dataArr,
                                            int nofData,
                                            int indexToPlot,
                                            int nofPlotToClear,
                                            QColor lineColor,
                                            bool hideDataSample)
{
    CYahooStockPlotUtil cyspu;

    // Do not excide array bondary
    if((indexToPlot >= CYahooStockPlotUtil::MAX_NOF_PLOT_COLORS) ||
       (nofPlotToClear >= CYahooStockPlotUtil::MAX_NOF_PLOT_COLORS))
    {
        return;
    }


    for(int i = 0; i < nofPlotToClear; i++)
    {
        cyspu.removePlotData(qwtAllPlotData, i, qwtPlot);
    }


    qwtAllPlotData.nofStocksToPlot = 1;

    if(resetMinMaxValue == true)
    {
        qwtAllPlotData.axis.minMaxIsInit = false;
    }

    for(int i = 0; i < nofData; i++)
    {
        m_x[i] = dataArr[i].date.toDouble();
        m_y[i] = dataArr[i].data.toDouble();
        cyspu.updateMinMaxAxis(qwtAllPlotData.axis, m_x[i], m_y[i]);
    }


    if((useAutoScale == false) && (qwtAllPlotData.axis.minY > 0))
    {
        qwtAllPlotData.axis.minY = 0;
    }

    qwtAllPlotData.stock[indexToPlot].data.setSamples(m_x, m_y, nofData);
    qwtPlot->setAxisMaxMinor(QwtPlot::xBottom, 2);
    qwtPlot->setAxisMaxMajor(QwtPlot::xBottom, nofData);

    qwtPlot->setAxisMaxMinor(QwtPlot::yLeft, 10);


    if(hideDataSample == false)
    {
        qwtAllPlotData.stock[indexToPlot].data.setPen(QPen(lineColor, 2));
        qwtAllPlotData.stock[indexToPlot].data.setSymbol(new QwtSymbol(QwtSymbol::Ellipse,                                                            Qt::blue,
                                                                   QPen(lineColor), QSize(7, 7) ) );
    }

    cyspu.plotData(qwtAllPlotData, qwtPlot, indexToPlot, useAutoScale);

    // Dirty way to acutoscale y axis
    //qwtPlot->setAxisAutoScale(QwtPlot::yLeft, true);

    // Disable legend when we plot to not get two
    qwtAllPlotData.stock[indexToPlot].data.setItemAttribute(QwtPlotItem::Legend, false);
    qwtAllPlotData.stock[indexToPlot].data.attach(qwtPlot);
    qwtPlot->replot();
}


#define START_VALUE_FIND_PLOT_SCALES 0.000000001
#define MAX_MAJOR_TICKS 20

/*******************************************************************
 *
 * Function:        logScaleFindStartStopTicksValue()
 *
 * Description:     Try to find First and last value on any log axis
 *
 *                  |.....|.....|.....|
 *                  10    100   1000  10000
 *
 *******************************************************************/
bool StockAnalysisTab::
logScaleFindStartStopTicksValue(double minValue,
                                double maxValue,
                                double &minPlotTicValue,
                                double &maxPlotTicValue,
                                int &nofTicks)
{
    bool foundMin = false;      // 1, 10, 100.          Coarse search of correct decad
    bool tmpFoundMin = false;   // 100, 200, 300        Fine search in same correct decad

    bool foundMax = false;      // 1, 10, 100
    bool tmpFoundMax = false;   // 100, 200, 300  searching for optomal value in a decad

    double cmpMinValue = START_VALUE_FIND_PLOT_SCALES;
    double cmpMaxValue;

    if((maxValue < 0) ||
       (minValue >= maxValue) ||
       (minValue < START_VALUE_FIND_PLOT_SCALES))
    {
        return false;
    }

    cmpMinValue = START_VALUE_FIND_PLOT_SCALES;

    //========================================
    // Find min value
    //========================================

    // Increment test cnt 1, 10, 100 ...
    for(int cnt = 0; cnt < 1000; cnt++)
    {
        if(cmpMinValue >= minValue)
        {
            // Step back one step so min scale is less than min y value
            cmpMinValue = cmpMinValue / (double) 10.0;
            qDebug() << "found" << "cmpMinValue" << cmpMinValue;
            foundMin = true;
            break;
        }

        cmpMinValue = cmpMinValue * (double) 10.0;

    }

    double tmpCmpMinValue = cmpMinValue;

    // Now increment 100, 200 , 300, ... 900
    for(int i = 1; i < 9; i++)
    {
        tmpCmpMinValue = cmpMinValue * (double) i;

        if(tmpCmpMinValue >= minValue)
        {
            if(i > 2)
            {
                tmpCmpMinValue = cmpMinValue * (double) (i - 1);
                qDebug() << "found" << "tmpCmpMinValue" << tmpCmpMinValue;
                tmpFoundMin = true;
                break;
            }
            else
            {
                break;
            }
        }
    }


    //========================================
    // Find max value
    //========================================
    cmpMaxValue = cmpMinValue;

    // Increment test cnt 1, 10, 100 ...
    for(int cnt = 0; cnt < 1000; cnt++)
    {
        if(cmpMaxValue >= maxValue)
        {
            qDebug() << "found" << "cmpMaxValue" << cmpMaxValue;
            foundMax = true;
            break;
        }

        cmpMaxValue = cmpMaxValue * (double) 10.0;

    }

    double scaleFactor = cmpMaxValue / 10;
    double tmpCmpMaxValue = scaleFactor * (double) 9.0;


    // Now increment 100, 200 , 300, ... 900
    for(int i = 9; i > 1; i--)
    {
        if(tmpCmpMaxValue < maxValue)
        {
            if(i < 9)
            {
                tmpCmpMaxValue = scaleFactor * (double) (i + 1);
                tmpCmpMaxValue = tmpCmpMaxValue * (double) 10.0;
                qDebug() << "found" << "tmpCmpMaxValue" << tmpCmpMaxValue;
                tmpFoundMax = true;
                break;
            }
            else
            {
                break;
            }
        }

        tmpCmpMaxValue = scaleFactor * (double) i;
    }


    // Select best minvalue
    if(tmpFoundMin == true && foundMin == true)
    {
        qDebug() << "minValue" << minValue;
        qDebug() << "cmpMinValue" << cmpMinValue;
        qDebug() << "tmpCmpMinValue" << tmpCmpMinValue;

        if(cmpMinValue > tmpCmpMinValue)
        {
            minPlotTicValue = cmpMinValue;
        }
        else
        {
            minPlotTicValue = tmpCmpMinValue;
        }
        qDebug() << "minPlotTicValue" << minPlotTicValue;
    }
    else
    {
        // Set Min plot tick on plot
        if(tmpFoundMin == true)
        {
            minPlotTicValue = tmpCmpMinValue;
        }
        else
        {
            minPlotTicValue = cmpMinValue;
        }
    }



    if((tmpFoundMax == true) && (foundMax == true))
    {
        qDebug() << "maxValue" << maxValue;
        qDebug() << "cmpMaxValue" << cmpMaxValue;
        qDebug() << "tmpCmpMaxValue" << tmpCmpMaxValue;


        if(cmpMaxValue < tmpCmpMaxValue)
        {
            maxPlotTicValue = cmpMaxValue;
        }
        else
        {
            maxPlotTicValue = tmpCmpMaxValue;
        }
        qDebug() << "maxPlotTicValue" << maxPlotTicValue;
    }
    else
    {
        // Set Max plot tick on plot
        if(tmpFoundMax == true)
        {
            maxPlotTicValue = tmpCmpMaxValue;
        }
        else
        {
            maxPlotTicValue = cmpMaxValue;
        }
    }



    if( ((foundMin == true) || (tmpFoundMin == true)) &&
        ((foundMax == true) || (tmpFoundMax == true) ))
    {

        double res = minPlotTicValue;
        // Calc number of Major tixs;
        for(nofTicks = 0; nofTicks < MAX_MAJOR_TICKS; nofTicks++)
        {
            if(res  >  maxPlotTicValue)
            {
                qDebug() << "nofTicks" << nofTicks;
                if(nofTicks < 2)
                {
                    nofTicks = 2;
                    qDebug() << "adj nofTicks" << nofTicks;
                }
                return true;
            }
            res = res * (double) 10.0;
        }
    }

    return false;
}




/*****************************************************************
 *
 * Function:		plotYAxisLogData()
 *
 * Description:		This function plots with log scale on Y-axis.
 *
 * CYahooStockPlotUtil::StockData_ST
 *
 *****************************************************************/
bool StockAnalysisTab::
plotYAxisLogData(CYahooStockPlotUtil::PlotData_ST &allPlotData, bool resetMinMaxScale,
                 SubAnalysDataST *dataArr, int nofData,
                 QwtPlot *qwtPlot, int index, QColor lineColor, int xScaleStep,bool changeSignYdata)
{
    CYahooStockPlotUtil cyspu;

    QString str;
    double yAxisStartTicValue;
    double yAxisStopTicValue;
    int nofTicks;


    if(index >= CYahooStockPlotUtil::MAX_NOF_PLOT_COLORS)
    {
        str.sprintf("Felaktigt index för inte vara större (Max = %d)", CYahooStockPlotUtil::MAX_NOF_PLOT_COLORS);
        QMessageBox::information(NULL, QString::fromUtf8("Förstort index"), str);
        return false;
    }

    if(index < 0)
    {
        str.sprintf("Felaktigt index. (Max = %d)", index);
        QMessageBox::information(NULL, QString::fromUtf8("Index förlite"), str);
        return false;
    }


    if(nofData == 0)
    {
        cyspu.removePlotData(m_qwtAllAnalysisPlotData, index, qwtPlot);
    }


    if(resetMinMaxScale == true)
    {
        allPlotData.axis.minMaxIsInit = false;
    }

    allPlotData.nofStocksToPlot = 1;



    // Find min max x and y values
    for(int i = 0; i < nofData; i++)
    {
        m_x[i] = dataArr[i].date.toDouble();
        m_y[i] = dataArr[i].data.toDouble();
        if(changeSignYdata == true)
        {
            m_y[i] = -m_y[i];
        }
        cyspu.updateMinMaxAxis(allPlotData.axis, m_x[i], m_y[i]);
    }

    // Insert data to data array suitable to update qwt plot
    allPlotData.stock[index].data.setSamples(m_x, m_y, nofData);


    // Calc start stop tick value for Y axis
    if(false == logScaleFindStartStopTicksValue(allPlotData.axis.minY, allPlotData.axis.maxY,
                                                yAxisStartTicValue, yAxisStopTicValue, nofTicks))
    {
        return false;
    }

    // Set curve color
    allPlotData.stock[index].data.setPen(QPen(lineColor, 2));
    allPlotData.stock[index].data.setSymbol(new QwtSymbol(QwtSymbol::Ellipse,                                                            Qt::blue,
                                                               QPen(lineColor),
                                                               QSize(7, 7) ) );

    //-----------------------------------------
    // Set x axis scale
    //-----------------------------------------
    qwtPlot->setAxisScale(QwtPlot::xBottom, allPlotData.axis.minX, allPlotData.axis.maxX, xScaleStep);


    //-----------------------------------------
    // Init y axis scale
    //-----------------------------------------
    qwtPlot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);

    // Set number of major tix on the plot
    qwtPlot->setAxisMaxMajor(QwtPlot::yLeft, nofTicks+1);

    // Set number of minor tix inbetween the major tix. | .... |    Where | = major . = minior
    qwtPlot->setAxisMaxMinor(QwtPlot::yLeft, 10);

    // Set x-axis scale
    //qwtPlot->setAxisScale(QwtPlot::yLeft, yAxisStartTicValue, yAxisStopTicValue, 1);
    qwtPlot->setAxisScale(QwtPlot::yLeft, (yAxisStartTicValue/10.0), yAxisStopTicValue, 0);

    // Update plot with new data
    allPlotData.stock[index].data.attach(qwtPlot);
    qDebug() << "index" << index;
    qwtPlot->replot();

     #if 0
    //qwtPlot->setAxisScaleDiv();
    //qwtPlot->setAxisScale()
    //qwtPlot->setAxisScale(QwtPlot::yLeft, 5, 250);
    //qwtPlot->setAxisScale(QwtPlot::yLeft, (allPlotData.axis.minY), (allPlotData.axis.maxY)/*1, 100000000*/);


    // qDebug() << "minY" << allPlotData.axis.minY;
    // qDebug() << "maxY" << allPlotData.axis.maxY;

    //QwtScaleDiv *scaleDiv = new QwtPlotGrid;

    //scaleDiv->setTicks();

    // qwtPlot->axisScaleDiv(QwtPlot::xBottom)->upperBound(); //  and ->hBound()
    tmp.sprintf("%g", qwtPlot->axisScaleDiv(QwtPlot::xBottom)->upperBound());
    qDebug() << "x axis upper" << tmp << "\n";

    tmp.sprintf("%g", qwtPlot->axisScaleDiv(QwtPlot::xBottom)->lowerBound());
    qDebug() << "x axis lower" << tmp << "\n";

    tmp.sprintf("%g", qwtPlot->axisScaleDiv(QwtPlot::xBottom)->range());
    qDebug() << "x range " << tmp << "\n";

    qDebug() << "x axisInterval" << tmp << "\n";
    #endif

    return true;
}



/*******************************************************************
 *
 * Function:        clearGUIIntrinsicValue()
 *
 * Description:
 *
 *
 *
 *******************************************************************/
void StockAnalysisTab::clearGUIIntrinsicValue(void)
{
    CYahooStockPlotUtil cyspu;


    ui->labelWarningEps->clear();
    ui->labelWarningDebt->clear();

    ui->lineEditTotErningsPerShare->clear();
    ui->lineEditTotEquityIncPerShare->clear();
    ui->lineEditTotDividendPerShare->clear();


    ui->lineEditHistoricalInterestOnEquity->clear();
    ui->lineEditCurrEquityPerShare->clear();
    ui->lineEditEstimateYearlyDividend->clear();
    ui->lineEditIntrinsicValueResult->clear();

    ui->lineEditIntrinsicValueResult->clear();
    for(int i = 0; i < 2; i++)
        cyspu.removePlotData(m_qwtEquityPlotData, i, ui->qwtPlot_10);
}


/*******************************************************************
 *
 * Function:
 *
 * Description: Not used
 *
 *
 *
 *******************************************************************/
void StockAnalysisTab::getminMaxLogScale(double minIn, double &minOut, double maxIn, double &maxOut)
{

    if(minIn < 0)
    {
        minOut = 1;
    }

    if(maxIn < 0)
    {
        maxOut = 1;
    }


    double maxTenPotenseData = 0.00002;
    double minTenPotenseData = 1000000000000;

    for(int i = 0; i < 20; i++)
    {
        if(minTenPotenseData > minIn)
        {
            minTenPotenseData = minTenPotenseData / 10;
        }
        else
        {
            minOut = minTenPotenseData;
        }


        if(maxTenPotenseData < maxIn)
        {
            maxTenPotenseData = maxTenPotenseData * 10;
        }
        else
        {
            maxOut = maxTenPotenseData;
        }
    }
}


/*******************************************************************
 *
 * Function:    on_pushSaveIValueDividend_clicked()
 *
 * Description:
 *
 *
 *
 *******************************************************************/
void StockAnalysisTab::on_pushSaveIValueDividend_clicked()
{
    CDbHndl db;
    bool res;
    int mainAnalysisId;
    QString str;

    bool isValid;
    int dividendDateId;
    int inputDividendDataId;
    int dividendDataId;
    bool dividendDataIdIsValid = false;

    int nofData;
    QString dividendDate;
    QString dividendData;

    if((m_stockName.length() < 1) || m_stockSymbol.length() < 1)
    {
        QMessageBox::critical(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Välj aktie"));
        return;
    }


    str = (QString::fromUtf8("Vill du lägga till sub data?\n"));
    str = str + m_stockName;
    str = str + ", ";
    str = str + m_stockSymbol;
    str = str + ", ";
    str = str + m_analysisDate;





    if(QMessageBox::No == QMessageBox::question(this, QString::fromUtf8("Uppdatera databas"), str, QMessageBox::Yes|QMessageBox::No))
    {
        return;
    }

    // Check if this stocksymbol and stockname is already added, if not add it
    res = db.mainAnalysisDataExists(m_stockName,
                                m_stockSymbol,
                                mainAnalysisId);
    if(false == res)
    {
        res = db.insertMainAnalysisData(m_stockName,
                                     m_stockSymbol,
                                     mainAnalysisId);
    }



    nofData = ui->tableWidgetIntrinsicValueDividendPerShare->rowCount();

    for(int row = 0; row < nofData; row++)
    {
       if(NULL != ui->tableWidgetIntrinsicValueDividendPerShare->item(row, 0))
       {
            dividendDate = ui->tableWidgetIntrinsicValueDividendPerShare->item(row, 0)->text();
            dividendData = ui->tableWidgetIntrinsicValueDividendPerShare->item(row, 1)->text();

            dividendDate.toInt(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Year is not a number"));
                continue;
            }

            CUtil cu;
            cu.number2double(dividendData, dividendData);
            dividendData.toDouble(&isValid);
            if (false == isValid)
            {
                QMessageBox::information(this, QString::fromUtf8("Uppdatera databas"), QString::fromUtf8("Dividend is not a number"));
                continue;
            }
       }
       else
       {
           break;
       }


        res = db.subAnalysisDividendDateExists(dividendDate,
                                                   mainAnalysisId,
                                                   dividendDateId);

        // Exist anaysis date?
        if(false == res)
        {
            res = db.insertSubAnalysisDividendDate(dividendDate,
                                                   mainAnalysisId,
                                                   dividendDateId);
        }


        if(true == res)
        {
           dividendDataIdIsValid = false;

           if( true == db.getSubAnalysisDividendId(mainAnalysisId, dividendDateId, inputDividendDataId))
           {
               dividendDataIdIsValid = true;
           }

            res = db.insertSubAnalysisDividend(dividendDateId,
                                               mainAnalysisId,
                                               inputDividendDataId,
                                               dividendDataIdIsValid,
                                               dividendData,
                                               dividendDataId);
        }
    }
}


/*******************************************************************
 *
 * Function:    on_pushButtonAltCalcAvgAnnualGrowthRateEquity_clicked()
 *
 * Description:
 *
 *
 *
 *******************************************************************/
void StockAnalysisTab::on_pushButtonAltCalcAvgAnnualGrowthRateEquity_clicked()
{

}

/*******************************************************************
 *
 * Function:    on_pushButtonAltcalcAvgAnnualGrowthRateEquity_clicked()
 *
 * Description: Open a dialog so user is able to adjust and
 *              calc Avg Annual Growth Rate of Equity
 *
 *
 *******************************************************************/
void StockAnalysisTab::on_pushButtonAltcalcAvgAnnualGrowthRateEquity_clicked()
{
    QString nofYears;

    if(m_nofEquityPerShareData >  2)
    {
        nofYears.sprintf("%d", (m_nofEquityPerShareData -1));
        calcAvgAnnualGrowthRateEquity_dlg.initDlg(m_equityPerShareArr[0].data,
                                                  m_equityPerShareArr[(m_nofEquityPerShareData -1)].data,
                                                  nofYears,
                                                  m_equityPerShareArr,
                                                  m_nofEquityPerShareData);
    }

    calcAvgAnnualGrowthRateEquity_dlg.show();
}




/*******************************************************************
 *
 * Function:    ()
 *
 * Description: Test LibCurl
 *
 *
 *******************************************************************/
void StockAnalysisTab::on_pushButtonSaveImg_2_clicked()
{
    MyLibCurl mlc;
    CURL *curlHndl;
    char url[256];
    char filename[80];
    char cookieResArr[256];

    char hostname[80];
    char incSubdomains[80];       // "TRUE"
    char path[80];                // "/",
    char secure[80];              // "FALSE"
    char expirationDate[80];      // "1527424259" (Linux time: Sun, 27 May 2018 12:30:59 GMT)
    char name[80];                // "B"
    char value[80];               // "95jdfnpci12gs&b=3&s=to",
    char cookieArr[256];

    // ".yahoo.com	TRUE	/	FALSE	1527437638	B	btn35mhcij9e6&b=3&s=e6"
    strcpy(hostname, ".yahoo.com");
    strcpy(incSubdomains, "TRUE");
    strcpy(path, "/");
    strcpy(secure, "FALSE");
    strcpy(expirationDate, "1527424259");
    strcpy(name, "B");
    strcpy(value, "5ljb2u1ciiiki&b=3&s=m8");


    strcpy(filename, "curlAbb.txt");

    //Crumb: 'mWnh3sO2quo', Cookie: '5ljb2u1ciiiki&b=3&s=m8'
    strcpy(url,"https://query1.finance.yahoo.com/v7/finance/download/ABB?period1=1493062089&period2=1495654089&interval=1d&events=history&crumb=mWnh3sO2quo");

    // strcpy(url,"https://finance.yahoo.com/quote/ABB?p=ABB");



    curlHndl = mlc.beginCurlSession();
    if(curlHndl)
    {
#if 0
        mlc.addYahooCookie(curlHndl,
                           hostname,            // ".yahoo.com",
                           incSubdomains,       // "TRUE"
                           path,                // "/",
                           secure,              // "FALSE"
                           expirationDate,      // "1527424259" (Linux time: Sun, 27 May 2018 12:30:59 GMT)
                           name,                // "B"
                           value,               // "95jdfnpci12gs&b=3&s=to",
                           cookieArr);
#endif

        if(true == mlc.requestYahooWebPageAndCookie(curlHndl, url, filename, cookieResArr))
        {
            QString str;
            str.sprintf("%s", cookieResArr);
            QMessageBox::information(NULL, QString::fromUtf8("Cookie"), str);
        }
        mlc.endCurlSession(curlHndl);
    }

}



#if 0
void StockAnalysisTab::on_pushButtonSaveImg_2_clicked()
{


    QString filename = "img_test1.png";

    QPixmap qPix = QPixmap::grabWidget(ui->qwtPlotCurrAssLiab);

    if(qPix.isNull())
    {
        qDebug("Failed to capture the plot for saving");
        return;
    }

   qPix.save(filename, "png");
}
#endif


// (\w+)? and (\w*) both match the same (0..+inf word characters)
//---------------------------------------------------------------
// However, there is a slight difference:
// In the first case, if this part of the regex matches "",
// the capturing group is absent. In the second case, it is empty.
// In some languages, the former manifests as a null while the
// latter should always be "".

// ?                The question mark indicates zero or one occurrences of the preceding element. For example, colou?r matches both "color" and "colour".
// *                The asterisk indicates zero or more occurrences of the preceding element. For example, ab*c matches "ac", "abc", "abbc", "abbbc", and so on.
// +                The plus sign indicates one or more occurrences of the preceding element. For example, ab+c matches "abc", "abbc", "abbbc", and so on, but not "ac".
// {n}[19]          The preceding item is matched exactly n times.
// {min,}[19]       The preceding item is matched min or more times.
// {min,max}[19] 	The preceding item is matched at least min times, but not more than max times.

// "CrumbStore":{"crumb":"FtGNqEzOvZp"}
// "CrumbStore":{"crumb":"D4Q3fGUPvOO"}
void StockAnalysisTab::on_pushButton_2_clicked()
{
    CUtil cu;
    QString filename("abb.html");
    QRegExp regExp("\"CrumbStore\":[{]\"crumb\":\"(\\w+)\"[}]");
    QString outSubstring;

    cu.getSubstringFromFile(filename, regExp, outSubstring);

}
