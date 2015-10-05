/*******************************************************************
 *
 * Filename: dbHndl.h
 * Date:     2012-07-08
 *
 * Description: Handles the sqlite databases
 *
 *
 *******************************************************************/

#ifndef CDbHndl_H
#define CDbHndl_H

#include <QString>
#include <QMutex>
#include <QStandardItemModel>
#include <QStringList>
#include <QtCore>
#include <QtGui>
#include <QObject>
#include <QChar>
#include <QVector>
#include "stockPlotUtil.h"
#include <QList>
#include <QTreeWidget>
#include <QTableView>

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlError>
#include "yahooStockPlotUtil.h"
#include "fifo.h"
#include "common.h"
#include "linkedList.h"


// Forward declaration
class CGuiUtils;


#define SQL_STR_ASC      "ASC"
#define SQL_STR_DESC     "DESC"



//extern QStringList g_filter1DividendSortColParamLst;

enum StockIndicatorSortType
{
    SISORT_TYPE_FLOAT,
    SISORT_TYPE_TEXT
};

struct TableColumnIndicatorInfo_ST
{
    QString name;
    qint64 indicatorlMask;
    QString nameInDb;
    QString sortOrder;
    StockIndicatorSortType sortType;

};


struct NordnetBalanceIncomeSheet_ST
{
    QString yearAssetName;
    unsigned int year;
    QString assetName;
    QString assetGroup;
    double operatingIncome;
    double operatingExpensesAndDepreciation;          // Depreciation = avskrivning
    double operatingIncomeAfterDepreciation;
    double netProfitAfterTax;                         // Årets resultat efter skatt
    double earningsPerShare;
    double numberOfShares;
    double fixedAssets;                               // Anläggningstillgångar
    double currentAssets;                             // Omsättningstillgångar
    double totalAssets;                               // Summa tillgångar
    double equity;                                    // Eget kapital
    double liabilities;                               // Skulder
    double totalEquityAndLiabilities;
};


struct NordnetCompanyDescription_ST
{
    QString assetSymbol;
    QString assetName;
    QString sector;
    QString bransch;
    QString executiveDirector; // VD
    QString businessDescription;
};




class CDbHndl//: public CDbBase
{
public:

    static const quint64 MASK_GPSEL_NOT_USED;                        //  0
    static const quint64 MASK_GPSEL_STOCK_PRICE;                     //  1
    static const quint64 MASK_GPSEL_ONE_DAY_PRICE_CHANGE;            //  2
    static const quint64 MASK_GPSEL_ONE_DAY_PERCENT_CHANGE;          //  3
    static const quint64 MASK_GPSEL_BID_PRICE;                       //  4
    static const quint64 MASK_GPSEL_OFFER_PRICE;                     //  5
    static const quint64 MASK_GPSEL_HIGHEST_PRICE;                   //  6
    static const quint64 MASK_GPSEL_LOWEST_PRICE;                    //  7
    static const quint64 MASK_GPSEL_VOLUME;                          //  8
    static const quint64 MASK_GPSEL_CURRENCY;                        //  9
    static const quint64 MASK_GPSEL_TIME;                            // 10
    static const quint64 MASK_GPSEL_PE;                              // 11
    static const quint64 MASK_GPSEL_PS;                              // 12
    static const quint64 MASK_GPSEL_EARN_PER_SHARE;                  // 13
    static const quint64 MASK_GPSEL_NAV_PER_SHARE;                   // 14
    static const quint64 MASK_GPSEL_DIV_PER_SHARE;                   // 15
    static const quint64 MASK_GPSEL_YIELD;                           // 16
    static const quint64 MASK_GPSEL_COURSE_PER_NET_EQUITY;           // 17
    static const quint64 MASK_GPSEL_ONE_WEEK_PROCENT_CHANGE;         // 18
    static const quint64 MASK_GPSEL_ONE_MONTH_PERCENT_CHANGE;        // 19
    static const quint64 MASK_GPSEL_THREE_MONTH_PERCENT_CHANGE;      // 20
    static const quint64 MASK_GPSEL_SIX_MONTH_PERCENT_CHANGE;        // 21
    static const quint64 MASK_GPSEL_ONE_YEAR_PERCENT_CHANGE;         // 22
    static const quint64 MASK_GPSEL_TWO_YEAR_PERCENT_CHANGE;         // 23
    static const quint64 MASK_GPSEL_THREE_YEAR_PERCENT_CHANGE;       // 24
    static const quint64 MASK_GPSEL_FIVE_YEAR_PERCENT_CHANGE;        // 25
    static const quint64 MASK_GPSEL_STOCK_NAME;                      // 26
    static const quint64 MASK_GPSEL_EARNING_DIVIDED_BY_DIVIDEND;     // 27
    static const quint64 MASK_GPSEL_NAV_DIV_LAST_STOCK_PRICE;        // 28
    static const quint64 MASK_GPSEL_MAX_NOF_GUI_PARAMETERS;          //MAX_PRAMS

    enum GuiParamSelIndex_ET
    {
        GPSEL_INDEX_NOT_USED,                        // 0
        GPSEL_INDEX_STOCK_NAME,                      // 26
        GPSEL_INDEX_STOCK_PRICE,                     // 1
        GPSEL_INDEX_ONE_DAY_PRICE_CHANGE,            // 2
        GPSEL_INDEX_BID_PRICE,                       // 4
        GPSEL_INDEX_OFFER_PRICE,                     // 5
        GPSEL_INDEX_HIGHEST_PRICE,                   // 6
        GPSEL_INDEX_LOWEST_PRICE,                    // 7
        GPSEL_INDEX_VOLUME,                          // 8
        GPSEL_INDEX_CURRENCY,                        // 9
        GPSEL_INDEX_TIME,                            // 10
        GPSEL_INDEX_PE,                              // 11
        GPSEL_INDEX_PS,                              // 12
        GPSEL_INDEX_EARNING_DIVIDED_BY_DIVIDEND,     // 27
        GPSEL_INDEX_NAV_DIV_LAST_STOCK_PRICE,        // 28
        GPSEL_INDEX_EARN_PER_SHARE,                  // 13
        GPSEL_INDEX_NAV_PER_SHARE,                   // 14
        GPSEL_INDEX_DIV_PER_SHARE,                   // 15
        GPSEL_INDEX_YIELD,                           // 16
        GPSEL_INDEX_COURSE_PER_NET_EQUITY,           // 17
        GPSEL_INDEX_ONE_DAY_PERCENT_CHANGE,          // 3
        GPSEL_INDEX_ONE_WEEK_PROCENT_CHANGE,         // 18
        GPSEL_INDEX_ONE_MONTH_PERCENT_CHANGE,        // 19
        GPSEL_INDEX_THREE_MONTH_PERCENT_CHANGE,      // 20
        GPSEL_INDEX_SIX_MONTH_PERCENT_CHANGE,        // 21
        GPSEL_INDEX_ONE_YEAR_PERCENT_CHANGE,         // 22
        GPSEL_INDEX_TWO_YEAR_PERCENT_CHANGE,         // 23
        GPSEL_INDEX_THREE_YEAR_PERCENT_CHANGE,       // 24
        GPSEL_INDEX_FIVE_YEAR_PERCENT_CHANGE,        // 25
        MAX_NOF_GPSEL_INDEX                          //MAX_PRAMS
    };


    enum
    {
        TTAB_GUI_CTRL1,
        TTAB_GUI_CTRL2,
        TTAB_GUI_CTRL3,
        TTAB_GUI_CTRL4,
        TTAB_GUI_CTRL5,
        TTAB_GUI_CTRL6,
        TTAB_GUI_CTRL7,
        MAX_NOF_TABLE_TAB_GUI_SEL_CTRLS};

    struct TableTabWidget_ST
    {
        QComboBox *analyseSelComboBox;
        QComboBox *indicatorComboBox[MAX_NOF_TABLE_TAB_GUI_SEL_CTRLS];
        QComboBox *compareComboBox[MAX_NOF_TABLE_TAB_GUI_SEL_CTRLS];
        QLineEdit *IndicatorValueLineEdit[MAX_NOF_TABLE_TAB_GUI_SEL_CTRLS];
    };

private:

    enum StateBollingerBand_ET
    {
        BOLLSTATE_PREPARE,
        BOLLSTATE_CALC
    };

    enum AVANZA_TYPE_DATA_ET
    {
        AVANZA_TYPE_DATA_DIVIDENDE,
        AVANZA_TYPE_DATA_BUYING_PRICE,
        AVANZA_TYPE_DATA_SELLING_PRICE,
        AVANZA_TYPE_DATA_MONEY_INSERTION,
        AVANZA_TYPE_DATA_OTHER
    };


    QSqlDatabase m_db;
    QMutex m_mutex;
    QUEUE_T m_Queue;
    CLinkedList m_lst;


    enum{MAX_NOF_SNAPSHOT_PRICE_DATA_COLUMNS = 11};

    enum{MAX_NOF_SNAPSHOT_YIELD_DATA_COLUMNS = 10};

    enum{MAX_NOF_SNAPSHOT_KEY_NUMBER_DATA_COLUMNS = 8};

    enum{MAX_NOF_SNAPSHOT_ANALYSE_DIVIDENT_COLUMNS = 16};//14};





    enum {GUI_PARAM_MAX_STR_SIZE = 255};

    enum GuiParamCompIndex_ET
    {
        GPCOMP_INDEX_NOT_USED,                        // 0
        GPCOMP_INDEX_GREATER,                         // 1
        GPCOMP_INDEX_GREATER_OR_EQUAL,                // 2
        GPCOMP_INDEX_EQUAL,                           // 3
        GPCOMP_INDEX_LESS_OR_EQUAL,                   // 4
        GPCOMP_INDEX_LESS,                            // 5
        MAX_NOF_GPCOMP_INDEX
    };

    enum{MAX_NOF_COMPARE_VALUES_PER_STOCK_PARAM = 2};

    enum NofFilterCompareValues_ET
    {
        COMP_VALUE1,
        COMP_VALUE2,
        MAX_NOF_COMP_VALUE
    };






    // GUI compare parameters
    static const unsigned short MASK_GPCOMP_GREATER;
    static const unsigned short MASK_GPCOMP_GREATER_OR_EQUAL;
    static const unsigned short MASK_GPCOMP_EQUAL;
    static const unsigned short MASK_GPCOMP_LESS_OR_EQUAL;
    static const unsigned short MASK_GPCOMP_LESS;
    static const unsigned short MASK_GPCOMP_MAX_NOF_GUI_PARAMS;


    // Plot Yahoo TA data
    // CYahooStockPlotUtil::XYPlotData_ST m_qwtStockPlotData;

    struct MacdQueData_ST
    {
        double data;
        double date;
        QString strDate;
    };

    // MACD
    QQueue <MacdQueData_ST> m_shortExpMovAvgQue;
    QQueue <MacdQueData_ST> m_longExpMovAvgQue;
    QQueue <MacdQueData_ST> m_macdQue;



public:

    enum treeWidgetColum_ET
    {
        TWColum_Name,
        TWColum_SYMBOL,  // Hidden
        TWCOLUM_IS_SELECTED,
        TWCOLUM_DEFAULT_SETTINGS,
        TWCOLUM_RISK,
        TWCOLUM_RETURN,
        TWCOLUM_MIN_PROCENT,
        TWCOLUM_MAX_PROCENT,
        TWCOLUM_SEL_PROCENT,
        TWCOLUM_NOF_DATA,
        TWCOLUM_COLOR_NUMBER
    };

    struct EfficPortStockData_ST
    {
        QString stockName;
        QString stockSymbol;
        bool isSelected;
        bool isValid;
        long nofSamples;
        QString startDate;
        QString endDate;
        double meanReturns;
        double riskStdDev;
    };

    struct PlotData_ST
    {
        double x;
        double y;
    };

    struct StockData_ST
    {
        QString stockName;
        QVector <PlotData_ST> data;
    };

    enum SortSymbolNameTreeWidget_ET
    {
        SORT_TWIDGET_SYMBOL,
        SORT_TWIDGET_NAME
    };



    bool delAllTblNordnetCompanyDescription(void);
    bool insertNordnetCompanyDescriptionData(NordnetCompanyDescription_ST data, bool dbIsHandledExternly=false);
    bool getNordnetCompanyDescription(NordnetCompanyDescription_ST &data, QString assetName);




    bool delAllTblMainHelpSection(void);
    bool delAllTblSubHelpSection(void);
    bool delAllTblHelpSectionTxt(void);
    bool findMainHelpSectionName(QString  mainHelpSectionName, int &MainHelpSectionId);
    bool findSubHelpSectionName(QString subHelpSectionName, int &subHelpSectionId);
    bool modifySubHelpSectionName(QString oldSubHelpSectionName,
                                  QString newSubHelpSectionName,
                                  bool dbIsHandledExternly=false);
    bool getHelpSectionTxt(int mainHelpSectionId, int subHelpSectionId, QString &HelpSectionTxt);
    bool insertMainHelpSection(QString mainHelpSectionName);
    bool insertSubHelpSection(QString subHelpSectionName, int mainHelpSectionId);
    bool insertHelpSectionTxt(QString mainHelpSectionName, QString subHelpSectionName, QString helpSectionTxt);
    bool modifyHelpSectionTxt(QString mainHelpSectionName,
                              QString subHelpSectionName,
                              QString helpSectionTxt,
                              bool dbIsHandledExternly=false);

    bool deleteSubHelpSectionTxt(QString mainHelpSectionName,
                                 QString subHelpSectionName);

    bool deleteSubHelpSectionName(QString subHelpSectionName);

    bool deleteMainHelpSectionName(QString mainHelpSectionName);

    bool findMainHelpSectionInTblHelpSectionTxt(QString  mainHelpSectionName, int &mainHelpSectionId);





    bool getAllMainHelpSectionNames(QComboBox *comboBox);
    bool getAllSubHelpSectionNames(QComboBox *comboBox, int mainHelpSectionId);





    bool addFa3NetProfitAfterTaxToTreeWidget(QTreeWidget *treeWidget, QString assetName);
    bool addFa2NetProfitAfterTaxToTreeWidget(QTreeWidget *treeWidget, QString assetName, QString &assetGroup);
    bool addFa2operatingIncomeToTreeWidget(QTreeWidget *treeWidget, QString assetName);


    bool addFa3MinMaxPEAndPrice(QTreeWidget *treeWidget, QTreeWidget *treeWidget1, QString assetName, QString assetSymbol);
    bool getYahooTaMinMaxAvgPrice(QString assetSymbol,
                                  QString startDate,
                                  QString endDate,
                                  double &minPrice,
                                  double &maxPrice,
                                  double &avgPrice);


    bool insertNordnetBalanceAndIncomeData(NordnetBalanceIncomeSheet_ST data, bool dbIsHandledExternly=false);
    bool delAllTblNordnetBalanceIncomeSheet(void);

    bool getListWithNordnetStockId(QList <CompanyNameAndId_ST> &companyList, bool dbIsHandledExternly=false);
    bool insertNordnetStockIdData(QString assetId, QString assetName, bool dbIsHandledExternly=false);
    bool delAllTblNordnetStockIds(void);


    bool delAllTblTmpEfficPortTreeWidget(bool dbIsHandledExternly=false);

    bool insertTmpEfficPortTreeWidgetData(QString assetName,
                                          QString assetSymbol,
                                          bool isUsed,
                                          bool defaultSettings,
                                          double risk,
                                          double meanReturn,
                                          int minProcent,
                                          int maxProcent,
                                          int selProcent,
                                          int nofItems,
                                          int colorNumber,
                                          bool dbIsHandledExternly=false);

    bool addTmpEfficPortDataToTreeWidget(QTreeWidget *treeWidget,
                                         int column,
                                         char *sortOrder,
                                         bool dbIsHandledExternly=false);


    bool addStockSymbolAndNameInTreeWidget1(QTreeWidget *treeWidget,
                                            QVector<CDbHndl::EfficPortStockData_ST> &efficPortStockArr,
                                            int stockListId,
                                            CDbHndl::SortSymbolNameTreeWidget_ET sortOn,
                                            char *sortOrder);


    bool addStockSymbolAndNameInTreeWidget(QTreeWidget *treeWidget,
                                           int stockListId,
                                           CDbHndl::SortSymbolNameTreeWidget_ET sortOn,
                                           char *sortOrder);

    bool addStockSymbolAndNameInTreeWidget1(QTreeWidget *treeWidget,
                                            int stockListId,
                                            CDbHndl::SortSymbolNameTreeWidget_ET sortOn,
                                            char *sortOrder);

    bool addStockListsToComboBox(QComboBox *comboBox);

    QVector <StockData_ST> m_stockArr;



    struct Filter_ST
    {
        QString indikator;
        QString compare[MAX_NOF_COMP_VALUE];
        QString value[MAX_NOF_COMP_VALUE];
        QString nofcompareValues;
    };


    enum DbInsertMode_ET
    {
        DB_MODE_INSERT_DATA,
        DB_MODE_UPDATE_DATA
    };


    struct snapshotStockData_ST
    {
        QString companyName;
        QString lastPrice;
        QString priceChange;
        QString procentChangeOneDay;
        QString bidPrice;
        QString offerPrice;
        QString highestPrice;
        QString lowestPrice;
        QString volume;
        QString currency;
        QString time;
        QString date;                        // When read from home page

        QString procentChangeOneWeek;
        QString procentChange1Month;
        QString procentChange3Month;
        QString procentChange6Month;
        QString procentChange1Year;
        QString procentChange2Year;
        QString procentChange3Year;
        QString procentChange5Year;

        QString keyValuePE;
        QString keyValuePS;
        QString keyValueEarningsPerShare;        // Vinst/aktie
        QString keyValueNAVPerShare;             // Substans/aktie
        QString keyValueDividendPerShare;        // Utdelning/aktie
        QString keyValueYield;                   // Direktavkastning
        QString keyValueCoursePerJEK;            // Kurs/JEK

        QString earningsDividedByDividend;
        QString navDivLastStockPrice;

    };

    struct GPCompLookUpTable_ST
    {
        char compName[GUI_PARAM_MAX_STR_SIZE];
        qint64 compMaskBit;
    };


    enum{MAX_STR_SIZE_SORT_ORDER = 10};


    struct StockIndicatorlLookUpTable_ST
    {
        //char name[GUI_PARAM_MAX_STR_SIZE];
        QString name;
        qint64 indicatorlMask;
        char nameInDb[GUI_PARAM_MAX_STR_SIZE];
        char sortOrder[MAX_STR_SIZE_SORT_ORDER];
        StockIndicatorSortType sortType;

    };

    struct StockIndicatorlSetting_ST
    {
        StockIndicatorlLookUpTable_ST indicator;
        GPCompLookUpTable_ST compareParam[MAX_NOF_COMPARE_VALUES_PER_STOCK_PARAM];
        QString indicatorValue[MAX_NOF_COMPARE_VALUES_PER_STOCK_PARAM];
        unsigned char nofCompareValues;
    };

    struct FilterSettings_ET
    {
        StockIndicatorlSetting_ST indicatorlSetting[CDbHndl::MAX_NOF_TABLE_TAB_GUI_SEL_CTRLS];
        GuiParamSelIndex_ET index[CDbHndl::MAX_NOF_TABLE_TAB_GUI_SEL_CTRLS];
        unsigned char nofStockIndicators;
        qint64 mainStockIndicatorMask;
    };

    FilterSettings_ET m_filter;

    static const StockIndicatorlLookUpTable_ST m_gpSelLookUpTable[MAX_NOF_GPSEL_INDEX];

    static const GPCompLookUpTable_ST m_gpCompLookUpTable[MAX_NOF_GPCOMP_INDEX];


    snapshotStockData_ST m_snapshotStockData;

    bool m_disableMsgBoxes;

    CDbHndl();
    ~CDbHndl();


    bool getYahooTaBoillingerBand(const QString assetSymbol,
                                  const QString startDate,
                                  const QString endDate,
                                  const double dbTimePeriod,
                                  const double numbStdDev,
                                  CYahooStockPlotUtil::PlotData_ST &qwtStockPlotData,
                                  CYahooStockPlotUtil::StockData_ST &stockData);

    bool getYahooTaMovingAverage(const QString assetSymbol,
                                           const QString startDate,
                                           const QString endDate,
                                           const double dbTimePeriod,
                                           CYahooStockPlotUtil::PlotData_ST &qwtStockPlotData,
                                           CYahooStockPlotUtil::StockData_ST &stockData);




    void disableMsgBoxes(bool status);

// protected:
    bool openDb(QString path, bool useMutex=false);
    void closeDb(bool useMutex =false);
    QString m_connection;

public:
    bool addTaData(QString stockList,
                   QString name,
                   QString  date,
                   QString open,
                   QString high,
                   QString low,
                   QString close,
                   QString volume,
                   QString adjClose);


    // TA-data
    bool delAllTblYahooTaStockName(void);
    bool delAllTblYahooTaData(void);
    bool delAllTblYahooNordetIntercon(void);


    bool findYahooTaData(QString date, int assetNameId, bool dbIsHandledExternly = false);
    bool getLatestDateYahooTaData(QString &date, QString assetSymbol, bool dbIsHandledExternly = false);
    bool findYahooTaStockSymbol(QString assetSymbol, int &assetNameId, bool dbIsHandledExternly = false);

    bool insertYahooStockName(QString assetName,
                              QString assetSymbol,
                              int &assetNameId,
                              bool dbIsHandledExternly=false,
                              QString marketList = "",          // Not implemented yet
                              QString assetType = "",           // Not implemented yet
                              int yahooNordetInterconId = 0);    // Not implemented yet


    bool insertYahooStockData(QString date,
                              QString open,
                              QString high,
                              QString low,
                              QString close,
                              QString volume,
                              QString adjClose,
                              int assetNameId,
                              bool dbIsHandledExternly = false);


    bool getYahooTaPriceData(QString assetName,
                              QString startDate,
                              QString endDate,
                              CYahooStockPlotUtil::PlotData_ST &m_qwtStockPlotData,
                              CYahooStockPlotUtil::StockData_ST &stockData);

    bool getYahooTaLeastSqrFit(const QString assetSymbol,
                               const QString startDate,
                               const QString endDate,
                               CYahooStockPlotUtil::PlotData_ST &qwtStockPlotData,
                               CYahooStockPlotUtil::StockData_ST &stockData,
                               QString &kValue,
                               QString &RValue);


    // Indicators
    bool getYahooTaMomentum(const QString assetSymbol,
                            const QString startDate,
                            const QString endDate,
                            const double dbTimePeriod,
                            CYahooStockPlotUtil::PlotData_ST &qwtStockPlotData,
                            CYahooStockPlotUtil::StockData_ST &stockData);

    bool getYahooTaStochastics(const QString assetSymbol,
                               const QString startDate,
                               const QString endDate,
                               const double dbTimePeriod,
                               double procDAvgTime,
                               CYahooStockPlotUtil::PlotData_ST &qwtStockPlotData,
                               CYahooStockPlotUtil::StockData_ST &stockData);

    bool getYahooTaRsi(const QString assetSymbol,
                       const QString startDate,
                       const QString endDate,
                       const double dbTimePeriod,
                       CYahooStockPlotUtil::PlotData_ST &qwtStockPlotData,
                       CYahooStockPlotUtil::StockData_ST &stockData);


    bool getYahooTaRateOfChange(const QString assetSymbol,
                                const QString startDate,
                                const QString endDate,
                                const double dbTimePeriod,
                                CYahooStockPlotUtil::PlotData_ST &qwtStockPlotData,
                                CYahooStockPlotUtil::StockData_ST &stockData);


    bool getYahooTaMacd(const QString assetSymbol,
                        const QString startDate,
                        const QString endDate,
                        const double dbLongTimePeriod,
                        const double dbMidTimePeriod,
                        const double dbshortTimePeriod,
                        CYahooStockPlotUtil::PlotData_ST &qwtStockPlotData,
                        CYahooStockPlotUtil::StockData_ST &stockData);
private:
    bool calcMacdMovAvg(const QString assetSymbol,
                        const QString startDate,
                        const QString endDate,
                        const double dbTimePeriod,
                        QQueue <MacdQueData_ST> &expMovAvgQue);

    bool clacMacdLine(QQueue <MacdQueData_ST> shortExpMovAvgQue,
                      QQueue <MacdQueData_ST> longExpMovAvgQue,
                      QQueue <MacdQueData_ST> &macdExpMovAvgQue);

    bool calcMacdSignalHist(const double dbTimePeriod,
                            QQueue <MacdQueData_ST> macdExpMovAvgQue,
                            CYahooStockPlotUtil::PlotData_ST &qwtStockPlotData,
                            CYahooStockPlotUtil::StockData_ST &stockData);



public:

    double m_tmpStdDev;

    struct VarianceData_ST
    {
      QString date;
      double variance;
    };

    QVector <VarianceData_ST> m_rowVarianseArr;
    QVector <VarianceData_ST> m_colVarianseArr;
    QVector <double> m_tmpArr;

    bool getKeyDataUseBridge(QString assetSymbol,
                             snapshotStockData_ST &keyData,
                             bool dbIsHandledExternly=false);


    bool insertKeyTaBridgeData(QString assetName,
                               QString assetSymbol,
                               bool dbIsHandledExternly=false);

    bool delAllTblNordnetYahooBridge(void);




    bool efficPortfCreateVarianceArr(const QString startDate,
                                     const QString endDate,
                                     CDbHndl::EfficPortStockData_ST &stockData,
                                     QVector <CDbHndl::VarianceData_ST> &tmpVarianseArr,
                                     bool dbIsHandledExternly = false)
    ;


    bool efficPortfCalcMeanAndStdDev(const QString startDate,
                                const QString endDate,
                                CDbHndl::EfficPortStockData_ST &stockData);

    bool
    efficPortfCalcCovar(QVector <CDbHndl::VarianceData_ST> &rowVarianseArr,
                              QVector <CDbHndl::VarianceData_ST> &colVarianseArr,
                              double &result);


    // Portfolio data
    bool delAllTblAvanzaAccount(void);
    bool delAllTblAvanzaAssetName(void);
    bool delAllTblAvanzaAssetData(void);

    bool getTotNofAvanzaPortfolioRecords(int &nofRow);
    bool listAllAvanzaPortfolioRecords(QObject *thisPointer, int rows, int cols, QTableView *tableView);


    bool findAvanzaAccount(int account);
    bool findAvanzaAssetName(int account, QString assetName);
    bool findAvanzaData(int account, QString assetName, QString date);

    bool insertAvanzaAssetData(QString date,
                          QString typeOfData,
                          int     typeOfDataId,
                          double assetPrice,
                          QString currency,
                          QString assetNameId);

    bool insertAvanzaAssetData(QString account,
                          QString assetName,
                          QString date,
                          QString typeOfData,
                          QString assetPrice,
                          QString currency);


    int getAvanzaTypeOfDataId(QString typeOfData);
    bool insertAvanzaAccount(int account);
    bool insertAvanzaAssetName(int account, QString assetName);


    bool addTaStockList(QString stockList);
    bool addTaStockName(QString stockSymbol, QString stockName, int stockListId, bool dbIsHandledExternly=false);
    bool findTaStockListId(QString stockListName, int &stockListId, bool dbIsHandledExternly=false);
    bool delAllTaStockLists(void);
    bool delAllTaStockNames(void);


    bool execSingleCmd(QSqlQuery qry);

    // Nordnet keydata P/E ...
    bool getAllSnapshotPlotStocksData(QVector <CStockPlotUtil::StockData_ST> &stockArr);
    bool getSnapshotStockData(CStockPlotUtil::StockData_ST &stockData);
    bool getOneSnapshotStock(CStockPlotUtil::StockData_ST &stock, CStockPlotUtil::MinMaxAxisValues_ST &axis);

    void updateMinMaxData(CStockPlotUtil::MinMaxAxisValues_ST &axis, double x, double y);
    bool filter1DividendGetColumnSortParameter(int index, QString &result);

    int getNofFilterIndicators(void);
    bool getFilterCompareInfo(GuiParamSelIndex_ET paramIndex, StockIndicatorlSetting_ST &compInfo);
    bool getFilterSetting(int i, CDbHndl::Filter_ST &data);
    qint64 getFilterMask(void);
    bool getFilterNofCompareVal(GuiParamSelIndex_ET paramIndex, unsigned char &nofCompareVal);
    void debugPrintFilter(void);
    bool filterAndSortCreateWherePart(QString &str, QString sortParam, QString sortOrder);

    void addDb(QString path);

    bool createTable(void);
    bool insertData(void);
    bool selectData(void);
    bool updateData(void);
    bool deleteData(void);

    void initKeyNumberList(QTableView *tableView, QStandardItemModel **tableModel);
    void initKeyNumberListFromDb(QTableView *tableView, QStandardItemModel **tableModel);
    bool dbInsertUserSelSnapshotData(void);
    bool saveUserSelectStockParamsInDb(QStandardItemModel *tableModel);
    bool delAllUserSelSnapshotData(void);
    bool userSelSnapshotDataIsInitialized(void);
    void createSelectQryUserSelStockParams(QString &str);

    void resetSnapshotRamData(snapshotStockData_ST &snapshotStockData, bool dbIsHandledExternly=false);
    bool insertOneRowPriceData(bool dbIsHandledExternly = false);
    bool insertOneRowYieldData(bool dbIsHandledExternly = false);
    bool insertOneRowKeyNumberData(bool dbIsHandledExternly = false);

    bool updateOneRowYieldData(bool dbIsHandledExternly=false);
    bool updateOneRowKeyNumberData(bool dbIsHandledExternly=false);

    bool findStockName(QString stockName, bool dbIsHandledExternly = false);

    void addOneSlotTableHeaderData(int tableHeaderIndex, CDbHndl::GuiParamSelIndex_ET indicatorIndex);
    void filter1DividendCreateTableHeader(void);
    void createTabelTabFilter(CDbHndl::TableTabWidget_ST tableTabW);
    bool initFilter1Dividend(QObject *thisPointer, QTableView *tableView, CDbHndl::TableTabWidget_ST *w, bool dbIsHandledExternly = false);
    bool filterAndSortCreateSqlQry(QString &str, QString sortParam, QString sortOrder);
    bool filter1Dividend(QObject *thisPointer, QTableView *tableView,
                         CDbHndl::TableTabWidget_ST *w,
                         QString sortParam = (QString) "",
                         QString sortOrder = (QString)SQL_STR_ASC,
                         bool filter = false);
    bool filterCreateSqlQry(QString &str);
    bool insertFilterDataInDb(void);
    bool comboBoxSetSelStr(QComboBox* combo, QString str);

    bool delAllStockSnapshotData(bool dbIsHandledExternly=false);
    bool delAllFilterStockSnapshotData(void);
    bool copyDbPriceToTable(QObject *thisPointer, QTableView *tableView);
    bool copyDbYieldToTable(QObject *thisPointer, QTableView *tableView);
    bool copyDbKeyNumberToTable(QObject *thisPointer, QTableView *tableView);
    bool getNofStockSnapshotTblRecords(int &nofRow);
    bool addCustomerSnabshotData(bool dbIsHandledExternly=false);
    bool dbDataSetFilterComboSel(CDbHndl::TableTabWidget_ST *tableTabW);
    void initSortCompareToComboBoxes(CDbHndl::TableTabWidget_ST *w);
    void initIndicatorToComboBoxes(CDbHndl::TableTabWidget_ST *w);

private:
    void initGuiCtrlSettings(void);
    bool getGuiCompareParamMask(char *param, quint64 &mask);
    bool filterCreateWherePart(QString &str);
    bool updateFilterParam(QComboBox *indicatorComboBox, QComboBox *compareComboBox, QLineEdit *IndicatorValueLineEdit, const int index);

    bool guiCntrlDataIsValid(QComboBox *indicatorComboBox, QComboBox *compareComboBox, QLineEdit *IndicatorValueLineEdit);
    bool findFilterParam(QComboBox *indicatorComboBox, int &index);
    bool insertFilterParam(QComboBox *indicatorComboBox, QComboBox *compareComboBox, QLineEdit *IndicatorValueLineEdit, GuiParamSelIndex_ET index);
    bool getGuiSelectParamMask(char *param, quint64 &mask, char *dbIndicatorName);
    void guiFilterCtrlResetContents(CDbHndl::TableTabWidget_ST *filterCtrls);



    void calcBoillingerBand(const double sumOfPrice,
                            const double timePeriod,
                            const double numbStdDev,
                            double &lowerBand,
                            double &average,
                            double &upperBand);

    void calcMovingAverage(const double sumOfPrice,
                           const double timePeriod,
                           double &average);

public:
    void init1dLeastSrq(int &nofData,
                        double &meanXSum,
                        double &meanYSum,
                        double &prodXXSum,
                        double &prodYYSum,
                        double &prodXYSum);

    bool calc1dLeastSrqFitRParam(int &nofData,
                                 double &prodXXSum,
                                 double &prodYYSum,
                                 double &prodXYSum,
                                 double &r);



    void gather1dLeastSrqData(const double x,
                                   const double y,
                                   int &nofData,
                                   double &meanXSum,
                                   double &meanYSum,
                                   double &prodXXSum,
                                   double &prodYYSum,
                                   double &prodXYSum);



    bool calc1dLeastSrqFitParams(int &nofData,
                           double &meanXSum,
                           double &meanYSum,
                           double &prodXXSum,
                           double &prodYYSum,
                           double &prodXYSum,
                           double &m,
                           double &k);

private:

    void calcMovingLeastSrqFit(const double k, const double m, double x, double &y);












};


extern TableColumnIndicatorInfo_ST m_tableHeaderList[CDbHndl::MAX_NOF_GPSEL_INDEX];
extern int m_maxTableColumns;

#endif // CDbHndl_H
