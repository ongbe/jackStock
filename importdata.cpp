#include "importdata.h"
#include "ui_importdata.h"
#include "parsecompanyeconomyinfo.h"
#include "dbHndl.h"
#include "nordnetparsecompanyid.h"

#include <QMessageBox>
#include <QtGui>
#include <QtCore>
#include <QString>
#include <QObject>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlError>
#include "util.h"

#include "parsekeytabridgedata.h"
#include "parsecompdescription.h"






#define TIME_2_MIN ((int)120000)

#define TAG_START_COMPANY_NAME QObject::tr("&marketplace=11\" class=\"underline\">")
#define TAG_START_COMPANY_NAME1 QObject::tr("&marketplace=14\" class=\"underline\">")
#define TAG_START_COMPANY_NAME2 QObject::tr("&marketplace=15\" class=\"underline\">")
#define TAG_START_COMPANY_NAME3 QObject::tr("&marketplace=24\" class=\"underline\">")


#define TAG_START_COMPANY_NAME_LEN strlen("&marketplace=11\" class=\"underline\">")
#define TAG_END_COMPANY_NAME QObject::tr("</a></div></td>")

const ImportData::TimePeriodDays_ST ImportData::m_timePeriodDaysArr[ImportData::MAX_NOF_TIME_PERIOD_DAYS_ITEMS] =
{
    {QString::fromUtf8("AllaData"),       ImportData::TIME_PERIOD_DAYS_ALL_DATA,},
    {QString::fromUtf8("10 År"),          ImportData::TIME_PERIOD_DAYS_10_YEAR,},
    {QString::fromUtf8("7 År"),           ImportData::TIME_PERIOD_DAYS_7_YEAR,},
    {QString::fromUtf8("5 År"),           ImportData::TIME_PERIOD_DAYS_5_YEAR,},
    {QString::fromUtf8("3 År"),           ImportData::TIME_PERIOD_DAYS_3_YEAR,},
    {QString::fromUtf8("2 År"),           ImportData::TIME_PERIOD_DAYS_2_YEAR,},
    {QString::fromUtf8("1 År"),           ImportData::TIME_PERIOD_DAYS_1_YEAR,},
    {QString::fromUtf8("6 månader"),      ImportData::TIME_PERIOD_DAYS_6_MONTH,},
    {QString::fromUtf8("3 månader"),      ImportData::TIME_PERIOD_DAYS_3_MONTH,},
    {QString::fromUtf8("1 månader"),      ImportData::TIME_PERIOD_DAYS_1_MONTH,},
    {QString::fromUtf8("2 veckor"),       ImportData::TIME_PERIOD_DAYS_2_WEEK,},
    {QString::fromUtf8("1 vecka"),        ImportData::TIME_PERIOD_DAYS_1_WEEK}
};


/****************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *
 *
 ****************************************************************/
ImportData::ImportData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportData)
{
    ui->setupUi(this);
    m_waitOnServerResp = false;
    m_singleStockDataReqStatus = STATUS_REQ_SINGLE_STOCK_IDLE;

    ui->progressBar->setValue(0);
    m_timeoutTimer = new QTimer;

    // Init time period
    initTimePeriodCtrls(TIME_PERIOD_DAYS_1_YEAR);
}

/****************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *
 *
 ****************************************************************/
ImportData::~ImportData()
{
    delete ui;
    delete m_timeoutTimer;
    delete m_importYahooTaDataThread;
}


/****************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *
 *
 ****************************************************************/
void ImportData::reqHtmlFile(QString path, QUrl url)
{
    if(m_waitOnServerResp == true)
    {
        QMessageBox::information(NULL, QString::fromUtf8("Hämta data"), QString::fromUtf8("Vänta: Upptagen med hämta data"));
        return;
    }

    QObject::connect(&m_hw1, SIGNAL(sendSignalTextToDlg2(int)), this, SLOT(slotHtmlPageIsRecv(int)));

    m_hw1.startRequest(url, path, 0x01);
    m_waitOnServerResp = true;
}



/****************************************************************
 *
 * Function:    readFile()
 *
 * Description:
 *
 * Link to get Stock ids: https://www.nordnet.se/mux/web/marknaden/kurslista/aktier.html?marknad=Sverige&lista=1_1&large=on&mid=on&sektor=0&subtyp=price&sortera=aktie&sorteringsordning=stigande
 * Company info text:     https://www.nordnet.se/mux/web/marknaden/aktiehemsidan/bolagsfakta.html?identifier=3966&marketplace=11
 * History:               https://www.nordnet.se/mux/web/marknaden/aktiehemsidan/nyckeltal.html?identifier=3966&marketplace=11
 *
 ****************************************************************/
bool ImportData::readFile(QString filename)
{
    QFile file(filename);
    QString errStr = QString("Fail to open file: %1").arg(filename);
    QString result;
    bool res;


    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::critical(NULL, QObject::tr("Fail to open file"), errStr);
        return false;
    }

    QTextStream inStream(&file);
    inStream.setCodec("ISO 8859-1");


    while(!inStream.atEnd())
    {
        res = parseCompanyName(inStream, result);

        if(res)
        {
            qDebug() << "result:" << result << ", ";
        }
    }

    file.close();
    return true;
}



/****************************************************************
 *
 * Function:    parseCompanyName()
 *
 * Description:
 *
 *
 *
 *
 ****************************************************************/
bool ImportData::
parseCompanyName(QTextStream &inStream, QString &result)
{
    QString str;
    QString id;
    int index;

    str = inStream.readLine();

    index = str.indexOf(TAG_START_COMPANY_NAME);
    if(index == -1)
        index = str.indexOf(TAG_START_COMPANY_NAME1);
    if(index == -1)
        index = str.indexOf(TAG_START_COMPANY_NAME2);
    if(index == -1)
        index = str.indexOf(TAG_START_COMPANY_NAME3);

    if(index > -1)
    {
        qDebug() << "str" << str;
        result = str.right(str.length()- index - TAG_START_COMPANY_NAME_LEN);

        id = str.left(str.length()- TAG_START_COMPANY_NAME_LEN);

        QString substring("			<td class=\"text\"><div class=\"truncate\"><a href=\"/mux/web/marknaden/aktiehemsidan/index.html?identifier=");

        id = str.right(str.length() - substring.length());

        id = id.left(id.indexOf("&"));

        qDebug() << "id =" << id;
        qDebug() << "result =" << result;


        index = result.indexOf(TAG_END_COMPANY_NAME);
        if(index > -1)
        {
            result = result.left(index);
            result = result.trimmed();
            qDebug() << "result =" << result << "\n\n";
            return true;
        }
    }
    return false;
}



/****************************************************************
 *
 * Function:    ()
 *
 * Description: Request next file with company info
 *
 *
 *
 *
 ****************************************************************/
void ImportData::slotReqNextCompanyData()
{
    QString path;
    ParseCompanyEconomyInfo pce;

    QString assetId;
    CompanyNameAndId_ST data;
    QString req;
    QUrl url;


    //QMessageBox::information(NULL, QObject::tr("Hämta data"), QString::fromUtf8("Filen hämtad"));
    m_timeoutTimer->stop();

    if(m_companyListIndex < m_companyList.count()-1)
    {
        m_oldCompanyListIndex = m_companyListIndex;
        data = m_companyList.at(m_companyListIndex);
        m_companyListIndex++;
        qDebug() << "m_companyListIndex" << m_companyListIndex;


        path = PATH_COMPANY_HIST_INFO;
        data.AssetName.trimmed();
        pce.readFile(path, data.AssetName);

            // Parse file
        bool found = false;
        do
        {
            found = false;
            data = m_companyList.at(m_companyListIndex);
            if(m_companyListIndex >= m_companyList.count()-2)
            {
                QMessageBox::information(this, QObject::tr("Finish"), QObject::tr("Finish"));
                return;
            }


            // SKa tas bort fixa parser buggen istället
            if(data.assetId.compare("103937") == 0 || data.assetId.compare("104058") == 0 || // Transcom WorldWide AB, Thule Group AB
               data.assetId.compare("104058") == 0 || data.assetId.compare("100208") == 0 ||
               data.assetId.compare("98395") == 0  || data.assetId.compare("104597") == 0 ||
               data.assetId.compare("100152") == 0 || data.assetId.compare("101601") == 0 ||
               data.assetId.compare("99667") == 0  ||
               data.assetId.compare("103110") == 0 || data.assetId.compare("104593") == 0 ||
               data.assetId.compare("101618") == 0 || data.assetId.compare("98809") == 0  ||
               data.assetId.compare("101207") == 0 || data.assetId.compare("100378") == 0 ||
               data.assetId.compare("106202") == 0 || data.assetId.compare("106028") == 0 || // Inwido AB
               data.assetId.compare("101639") == 0 || data.assetId.compare("102786") == 0 || // Fenix Outdoor International AG Ser. B
               data.assetId.compare("103930") == 0 || data.assetId.compare("105183") == 0 || // Lifco AB ser.B, Lundin Gold Inc.,
               data.assetId.compare("104592") == 0 || data.assetId.compare("101634") == 0 || // NP3 Fastigheter AB, Scandi Standard AB
               data.assetId.compare("110079") == 0 || data.assetId.compare("107904") == 0 || // Collector AB, Hoist Finance AB
               data.assetId.compare("877") == 0) // Active Biotech AB OK,
            {
                found = true;
                m_companyListIndex++;
                if(m_companyListIndex >= m_companyList.count()-2)
                {
                    QMessageBox::information(this, QObject::tr("Finish"), QObject::tr("Finish"));
                    return;
                }
            }

        }while(found == true);

        path = PATH_COMPANY_HIST_INFO;
        assetId = data.assetId;
        assetId.trimmed();
        qDebug() << "aa name: " << data.AssetName << "Id: " << data.assetId;
        req.sprintf("https://www.nordnet.se/mux/web/marknaden/aktiehemsidan/nyckeltal.html?identifier=%s&marketplace=11", assetId.toAscii().constData());
        url = req;
        m_waitOnServerResp = false;
        reqHtmlFile(path, url);
        m_sendNextReq = true;

    }
    else
    {
        QMessageBox::information(this, QObject::tr("Finish"), QObject::tr("Finish"));
    }



}


/*******************************************************************
 *
 * Function:    slotHtmlPageIsRecv()
 *
 * Description: This function is invoked when a html page is
 *              completely received.
 *
 *
 *******************************************************************/
void ImportData::slotHtmlPageIsRecv(int number)
{

    number = number;
    m_waitOnServerResp = false;

    if(m_companyListIndex < m_companyList.count()-1)
    {
        m_timeoutTimer->stop();
        if(m_sendNextReq == true)
        {
            if(m_oldCompanyListIndex <= m_companyListIndex)
            {
                qDebug() << "Index" << m_companyListIndex << "count" << m_companyList.count()-2;
                m_timeoutTimer->singleShot(200, this, SLOT(slotReqNextCompanyData()));
                m_sendNextReq = false;
                m_oldCompanyListIndex = m_companyListIndex;
            }
        }
    }
    else
    {
        m_timeoutTimer->stop();
        if(m_displayFinish== true)
        {
            m_displayFinish = false;
            QMessageBox::information(this, QObject::tr("Finish"), QObject::tr("Finish"));
        }
    }
}


/****************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *
 *
 ****************************************************************/
void ImportData::on_pushButtonReqCompInfo_clicked()
{
    QString path;
    QUrl url;
    CDbHndl db;
    CompanyNameAndId_ST data;
    QString req;
    QString assetId;
    m_oldCompanyListIndex = -1;
    m_displayFinish = true;

    m_companyList.clear();

    if(true == db.getListWithNordnetStockId(m_companyList))
    {
        db.delAllTblNordnetBalanceIncomeSheet();
        for(int i = 0; i < m_companyList.count(); i++)
        {
            data = m_companyList.at(i);
            qDebug() << i << data.assetId << data.AssetName;
        }

        m_companyListIndex = 0;
        data = m_companyList.at(0);
        path = PATH_COMPANY_HIST_INFO;
        assetId = data.assetId;
        assetId.trimmed();
        // req.sprintf("https://www.nordnet.se/mux/web/marknaden/aktiehemsidan/nyckeltal.html?identifier=%s&marketplace=11", assetId.toAscii().constData());



        req.sprintf("https://www.nordnet.se/mux/web/marknaden/aktiehemsidan/nyckeltal.html?identifier=%s&marketplace=11", assetId.toAscii().constData());
        // url = "https://www.nordnet.se/mux/web/marknaden/aktiehemsidan/nyckeltal.html?identifier=3966&marketplace=11";
        url = req;

        qDebug() << req;

        reqHtmlFile(path, url);
        m_sendNextReq = true;

    }
    //QMessageBox::information(this, QObject::tr("Finish"), QObject::tr("Finish"));

}






/****************************************************************
 *
 * Function:    on_pushButtonParseCompanyIds_clicked()
 *
 * Description: parse nordnet stock IDs
 *
 *              This Ids is used when requesting html pages
 *              with company info balance and Profit and loss
 *              sheet data.
 *
 ****************************************************************/
void ImportData::on_pushButtonParseCompanyIds_clicked()
{
    CDbHndl db;
    nordnetParseCompanyId ppd;
    QString filename = DWLD_PATH_STOCK_PRICE_FILE;

    db.delAllTblNordnetStockIds();
    m_timerId = startTimer(100);

    //qDebug() << filename;
    if(false == ppd.readFile(filename))
    {
        QMessageBox::information(this, QObject::tr("Error"), QObject::tr("Fail to parse company Ids"));
        killTimer(m_timerId);
    }
    else
    {
        QMessageBox::information(this, QObject::tr("Finish"), QObject::tr("Finish"));
        killTimer(m_timerId);
    }

}


//========= TA ====================


/****************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *
 *
 ****************************************************************/
void ImportData::on_SelStockListButton_clicked()
{
    QString stockListName;
    int stockListId;

     stockListName = ui->StockListComboBox->currentText().toUtf8();

     if(true == m_db.findTaStockListId(stockListName, stockListId))
     {
         ui->treeWidget->clear();
         m_db.addStockSymbolAndNameInTreeWidget1(ui->treeWidget,
                                            stockListId,
                                            CDbHndl::SORT_TWIDGET_NAME,
                                            (char *) SQL_STR_ASC);


         ui->treeWidget->setColumnWidth(0, 170);
         ui->treeWidget->scrollToTop();
         ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(0));
     }
}

/****************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *
 *
 ****************************************************************/
void ImportData::on_pushButInit_clicked()
{
    initStockList();
    openUserStockListsFile();

    ui->checkBoxReqestData->setChecked(true);


    // Create thread that import data from yahoo
    m_importYahooTaDataThread = new importYahooTaData(this);

    if(m_importYahooTaDataThread == 0)
    {
        QMessageBox::information(NULL, QObject::tr("Create thread"), QString::fromUtf8("Fail to create Ta Import Data thread"));
    }
    else
    {
        QMessageBox::information(NULL, QObject::tr("Success"), QObject::tr("Finish"));
    }

}

/*******************************************************************
 *
 * Function:    openStockListFile()
 *
 * Description: This function open the main file that contains
 *              all user defined stock lists stored as csv files
 *              and add the name of these list into the stocklist
 *              combobox.
 *
 *
 *******************************************************************/
void ImportData::on_treeWidget_doubleClicked(const QModelIndex &index)
{

    QString stockName;
    QString stockSymbol;
    CDbHndl cd;
    CUtil cu;
    QString lastDbDate;
    QString currDate;
    QString tmp;
    int nofDays;

    // Are we already processing data?
    if(m_singleStockDataReqStatus == STATUS_REQ_SINGLE_STOCK_IDLE)
    {
        m_singleStockDataReqStatus = STATUS_REQ_SINGLE_STOCK_PENDING;
        getSelStockListItem(stockName, stockSymbol, index);

        m_reqStockSymbol = stockSymbol;
        m_reqStockName = stockName;

        qDebug() << m_reqStockSymbol;
        qDebug() << m_reqStockName;

        if(true == cd.getLatestDateYahooTaData(lastDbDate, stockSymbol))
        {
            cu.getCurrentDate(currDate);
            qDebug() << currDate;
            qDebug() << lastDbDate;
            cu.nofDaysBeteenDates(currDate, lastDbDate, nofDays);
            if(nofDays < -1)
            {
                bool isUpdate;

                findDateList(stockSymbol, isUpdate);
                QString tmp;
                tmp.sprintf("%d",isUpdate);
                qDebug() << tmp;
                qDebug() << stockSymbol;
                if(/*isUpdate == false &&*/ ui->checkBoxReqestData->isChecked() == true)
                {
                    // Prepare request of data from Yahoo
                    qDebug() << stockName;
                    qDebug() << stockSymbol;
                    qDebug() << lastDbDate;
                    qDebug() << currDate;

                    prepReqTaDataFromServer(stockName, stockSymbol, lastDbDate, currDate);
                }
                else
                {
                    m_singleStockDataReqStatus = STATUS_REQ_SINGLE_STOCK_IDLE;
                    displayStockData();
                }
            }
            else
            {
                m_singleStockDataReqStatus = STATUS_REQ_SINGLE_STOCK_IDLE;
                displayStockData();
            }
       }
        // No latest date was found lets add our own
        else
        {
            // Prepare request of data from Yahoo
            lastDbDate = "1992-01-01";
            cu.getCurrentDate(currDate);
            prepReqTaDataFromServer(stockName, stockSymbol, lastDbDate, currDate);
        }
    }
    else
    {
        m_singleStockDataReqStatus = STATUS_REQ_SINGLE_STOCK_IDLE;
        QMessageBox::information(this, tr("Vänta"), tr("Vänta Processar redan data.."));
    }
}


/*******************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *******************************************************************/
void ImportData::getSelStockListItem(QString &stockName, QString &stockSymbol, const QModelIndex &index)
{
    ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(index.row()));
    ui->treeWidget->setFocus();

    stockName.clear();
    stockSymbol.clear();


    stockName = ui->treeWidget->currentItem()->text(STOCKLIST_NAME);
    stockSymbol = ui->treeWidget->currentItem()->text(STOCKLIST_SYMBOL);

}



/*******************************************************************
 *
 * Function:    on_pushButtonImportAllData_clicked()
 *
 * Description:
 *
 *
 *******************************************************************/
void ImportData::prepReqTaDataFromServer(QString stockName, QString stockSymbol, QString startDate, QString endDate)
{

    if(m_singleStockDataReqStatus == STATUS_REQ_SINGLE_STOCK_PENDING)
    {
        m_reqStartDate = startDate;
        m_reqEndDate = endDate;
        m_reqStockSymbol = stockSymbol;
        m_reqStockName = stockName;

        qDebug() << m_reqStartDate;
        qDebug() << m_reqEndDate;
        qDebug() << m_reqStockSymbol;
        qDebug() << m_reqStockName;


        // Signal used when thread is finish importing data
       connect(m_importYahooTaDataThread, SIGNAL(emitImportSingelStockDataIsFinish(int)), this, SLOT(slotImportSingelStockDataIsFinish(int)));
       connect(this, SIGNAL(emitReqSingleStockDataFromServer()), this, SLOT(slotReqSingleStockDataFromServer()));

       emit emitReqSingleStockDataFromServer();

    }

}


/*******************************************************************
 *
 * Function:    slotReqSingleStockDataFromServer()
 *
 * Description: This function is invoked when singel stock data need
 *              to be updated. (price data is old)
 *
 *******************************************************************/
void ImportData::slotReqSingleStockDataFromServer()
{
    QString qry;
    CUtil cu;

    QString filename = DWLD_PATH_TA_LIST_FILE;

    QString startYear;
    QString startMonth;
    QString startDay;

    QString endYear;
    QString endMonth;
    QString endDay;


    if(m_singleStockDataReqStatus == STATUS_REQ_SINGLE_STOCK_PENDING)
    {
        m_singleStockDataReqStatus = STATUS_REQ_SINGLE_STOCK_PROCESSING;

        QByteArray ba1 = m_reqStockSymbol.toLocal8Bit();
        const char *c_reqStockSymbol = ba1.data();

        cu.splitDate(m_reqStartDate, startYear, startMonth, startDay);

        cu.splitDate(m_reqEndDate, endYear, endMonth, endDay);

        qry.sprintf("http://ichart.finance.yahoo.com/table.csv?s=%s&d=%d&e=%d&f=%d&g=d&a=%d&b=%d&c=%d&ignore=.csv",
                    c_reqStockSymbol,
                    (endMonth.toInt()-1),
                    endDay.toInt(),
                    endYear.toInt(),
                    (startMonth.toInt()-1),
                    startDay.toInt(),
                    startYear.toInt());

        qDebug() << startYear;
        qDebug() << startMonth;
        qDebug() << startDay;

        qDebug() << endYear;
        qDebug() << endMonth;
        qDebug() << endDay;

        qDebug() << qry;


        QObject::connect(&m_hw1, SIGNAL(sendSignalTextToDlg2(int)), this, SLOT(slotReceivedAssetTaDataFromServer(int)));
        QUrl url(qry);

        startReqSingleStockDataTimeoutTimer(TIME_2_MIN);
        m_hw1.startRequest(url, filename, 0x01);
    }
}



/*******************************************************************
 *
 * Function:    startResendTimer()
 *
 * Description: This function activate resend timer that is used when
 *              request data from Yahoo server.
 *
 *              This is a single shot timer.
 *
 *******************************************************************/
void ImportData::startReqSingleStockDataTimeoutTimer(int ms)
{
    QTimer::singleShot(ms, this, SLOT(slotReqSingleStockDataTimerExpired()));
    qDebug() << "startReqSingleStockDataTimeoutTimer()";

}

/*******************************************************************
 *
 * Function:    slotReqSingleStockDataTimerExpired()
 *
 * Description: This function is invoked when resend timer has expired
 *
 *
 *******************************************************************/
void ImportData::slotReqSingleStockDataTimerExpired()
{
    if(m_singleStockDataReqStatus != STATUS_REQ_SINGLE_STOCK_IDLE)
    {
        m_singleStockDataReqStatus = STATUS_REQ_SINGLE_STOCK_IDLE;
        modifyDateList(m_reqStockSymbol, true);
        qDebug() << m_reqStockSymbol;
        QMessageBox::information(this, tr("Timeout"), tr("Timeout: ingen data kunde hämtas"));
    }
}



/*******************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *******************************************************************/
void ImportData::findDateList(QString stockSymbol, bool &isUpdate)
{
    isUpdate = true;

    NameKey node;
     QMutableListIterator <NameKey> iterator(m_dateIsUpdatedList);
     while(iterator.hasNext())
     {
        node = iterator.next();
        qDebug() << node.symbolName;
        QString tmp;
        tmp.sprintf("%d", node.dataIsUpdated);
        qDebug() << tmp;
        if(node.symbolName.compare(stockSymbol)== 0)
        {
            isUpdate = node.dataIsUpdated;
            return;
        }
     }
}






/*******************************************************************
 *
 * Function:    initStockList()
 *
 * Description:
 *
 *
 *******************************************************************/
void ImportData::initStockList(void)
{

    QString column0 = QObject::tr("Namn");
    QString column1 = QObject::tr("Symbol");
    QString column2 = QObject::tr("Notera");

    ui->treeWidget->setColumnCount(3);
    ui->treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    if(QTreeWidgetItem* header = ui->treeWidget->headerItem())
    {
        header->setText(0, column0.toLocal8Bit());
        header->setText(1, column1.toLocal8Bit());
        header->setText(2, column2.toLocal8Bit());
    }
}


/*******************************************************************
 *
 * Function:    on_OpenFileButton_clicked()
 *
 * Description:
 *
 *
 *******************************************************************/
void ImportData::openUserStockListsFile(void)
{
    QString filename = DWLD_PATH_TA_STOCKLIST_FILE;

    m_db.delAllTaStockLists();
    m_db.delAllTaStockNames();
    m_db.delAllTaStockData();
    m_db.delAllTblYahooTaData();

    openStockListFile(filename);
    addDataToStockListAndStockListCombo();

}


/*******************************************************************
 *
 * Function:    initTimePeriodCtrls()
 *
 * Description:
 *
 *
 *******************************************************************/
void ImportData::initTimePeriodCtrls(int value)
{
    CUtil cu;
    int nofDays;

    // Init to valid date
    cu.getCurrentDate(m_startDate);
    cu.getCurrentDate(m_endDate);
    m_endDateRef = m_endDate;

    qDebug() << m_startDate;
    qDebug() << m_endDate;

    m_timePeriodDaysInc = value;
    ui->timePeriodLlineEdit->clear();
    ui->timePeriodLlineEdit->insert((QString)m_timePeriodDaysArr[value].TxtTimePeriod.toLatin1());
    ui->selTimePeriodSlider->setValue(value);
    setTimePeriodDaysUpdateStartStopDate(m_startDate, m_endDate, value);

    if(cu.nofDaysBeteenDates(m_startDate, m_endDate, nofDays) == true)
    {
    }

}






/*******************************************************************
 *
 * Function:    openStockListFile()
 *
 * Description: This function open the main file that contains
 *              all user defined stock lists stored as csv files
 *              and add the name of these list into the stocklist
 *              combobox.
 *
 *
 *******************************************************************/
bool ImportData::openStockListFile(QString filename)
{
    QFile file(filename);
    QString str;
    QString stockListName;
    QString stockNameFile;


    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QString errStr = QString("Fail to open file: %1").arg(filename);
        QMessageBox::critical(NULL, QObject::tr("Fail to open file"), errStr);
        return false;
    }

    QTextStream inStream(&file);

    while(!inStream.atEnd())
    {
        str = inStream.readLine();
        qDebug() << str << "\n";

        // Get filename of stockname
        if(str.indexOf("\t") != -1)
        {
            stockListName = str.left(str.indexOf("\t"));
            qDebug() << stockListName;
        }

        // Get filename of stockname
        if(str.indexOf("\t") != -1)
        {
            stockNameFile = qApp->applicationDirPath();
            stockNameFile += DWLD_PATH_TA_INPUT_DATA_PATH;
            stockNameFile += str.right(str.length() - str.indexOf("\t")-1);
            stockNameFile.trimmed();


            if(false == m_db.addTaStockList(stockListName))
            {
                str.sprintf("Kan inte lägga till stockList");
                QMessageBox::critical(NULL, QObject::tr("Läga till stock list"), str.toLocal8Bit().constData());
                return false;
            }
            else
            {
                openStockNameFile(stockNameFile, stockListName);
            }
        }
    }

    qDebug() << "close file\n";
    file.close();
    return true;
}







/*******************************************************************
 *
 * Function:    addDataToStockListAndStockListCombo()
 *
 * Description: Populates comboBox with all user defined stock lists
 *              and populate list ctrl with all stocks in selected
 *              stock list.
 *
 *******************************************************************/
void ImportData::addDataToStockListAndStockListCombo(void)
{
   QString stockListName;
   int stockListId;
   int cnt = 0;


   if(false == m_db.addStockListsToComboBox(ui->StockListComboBox))
   {
       return;
   }

   ui->StockListComboBox->setCurrentIndex(0);

    stockListName = ui->StockListComboBox->currentText().toLatin1();

    if(true == m_db.findTaStockListId(stockListName, stockListId))
    {
        ui->treeWidget->clear();
        m_db.addStockSymbolAndNameInTreeWidget1(ui->treeWidget,
                                           stockListId,
                                           CDbHndl::SORT_TWIDGET_NAME,
                                           (char *) SQL_STR_ASC);

        cnt++;
        ui->treeWidget->setColumnWidth(0, 170);
        ui->treeWidget->scrollToTop();
        ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(0));
    }
}


/*******************************************************************
 *
 * Function:    setTimePeriodDaysUpdateStart()
 *
 * Description: This function is used when user changes set
 *              Time Period Slider position.
 *
 *
 *******************************************************************/
void ImportData::
setTimePeriodDaysUpdateStartStopDate(QString &startDate, QString &endDate, int value)
{
    CUtil cu;
    int intYear;
    int intMonth;
    int intDays;


    if(false == cu.dateIsValid(endDate))
    {
         QMessageBox::information(this, QString::fromUtf8("Datum"), QString::fromUtf8("1. Slut datum felaktigt"));
        return;
    }



    qDebug() << "sw" << m_timePeriodDaysArr[value].timePeriod;

    switch(m_timePeriodDaysArr[value].timePeriod)
    {
    case TIME_PERIOD_DAYS_ALL_DATA:
        // TBD
        break;
    case TIME_PERIOD_DAYS_10_YEAR:
        intYear = -10;
        cu.addYear(endDate, startDate, intYear);
        break;
    case TIME_PERIOD_DAYS_7_YEAR:
        intYear = -7;
        cu.addYear(endDate, startDate, intYear);
        break;
    case TIME_PERIOD_DAYS_5_YEAR:
        intYear = -5;
        cu.addYear(endDate, startDate, intYear);
        break;
    case TIME_PERIOD_DAYS_3_YEAR:
        intYear = -3;
        cu.addYear(endDate, startDate, intYear);
        break;
    case TIME_PERIOD_DAYS_2_YEAR:
        intYear = -2;
        cu.addYear(endDate, startDate, intYear);
        break;
    case TIME_PERIOD_DAYS_1_YEAR:
        intYear = -1;
        cu.addYear(endDate, startDate, intYear);
        break;
    case TIME_PERIOD_DAYS_6_MONTH:
        intMonth = -6;
        cu.addMonth(endDate, startDate, intMonth);
        break;
    case TIME_PERIOD_DAYS_3_MONTH:
        intMonth = -3;
        cu.addMonth(endDate, startDate, intMonth);
        break;
    case TIME_PERIOD_DAYS_1_MONTH:
        intMonth = -1;
        cu.addMonth(endDate, startDate, intMonth);
        break;
    case TIME_PERIOD_DAYS_2_WEEK:
        intDays = -14;
        cu.addDays(endDate, startDate, intDays);
        break;
    case TIME_PERIOD_DAYS_1_WEEK:
        intDays = -7;
        cu.addDays(endDate, startDate, intDays);
        break;
    default:
        break;
    }

    qDebug() << "startTime" << startDate;
    qDebug() << "endTime" << endDate;

    ui->setStartDateLineEdit->clear();
    ui->setEndDateLineEdit->clear();
    ui->setEndDateLineEdit->insert(endDate);
    ui->setStartDateLineEdit->insert(startDate);

}



/*******************************************************************
 *
 * Function:    openStockListFile()
 *
 * Description:
 *
 *
 *******************************************************************/
bool ImportData::openStockNameFile(QString filename, QString stockListName)
{
    QFile file(filename);
    QString str;
    QString stockSymbol;
    QString stockName;
    int stockListId;


    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QString errStr = QString("Fail to open file: %1").arg(filename);
        QMessageBox::critical(NULL, QObject::tr("Fail to open file"), errStr);
        return false;
    }

    m_db.openDb(PATH_JACK_STOCK_DB, true);


    QTextStream inStream(&file);



    while(!inStream.atEnd())
    {
        str = inStream.readLine(); //.toUtf8();


        // Get filename of stockname
        if(str.indexOf("\t") != -1)
        {
            stockSymbol = str.left(str.indexOf("\t"));
            qDebug() << stockSymbol;
        }

        // Get filename of stockname
        if(str.indexOf("\t") != -1)
        {
            stockName = str.right(str.length() - str.indexOf("\t")-1);
            qDebug() << stockName;
        }

        // QString stockListName = QObject::tr("Stockholm OMX");

        if(true == m_db.findTaStockListId(stockListName, stockListId, true))
        {
            m_db.addTaStockName(stockSymbol, stockName, stockListId, true);
            addStatusDateList(stockSymbol, false);
        }
    }

    qDebug() << "close file\n";
    file.close();
    m_db.closeDb(true);
    return true;
}


/*******************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *******************************************************************/
void ImportData::addStatusDateList(QString stockSymbol, bool isUpdate)
{
    NameKey data;
    data.symbolName = stockSymbol;
    data.dataIsUpdated = isUpdate;
    m_dateIsUpdatedList.append(data);

}


/*******************************************************************
 *
 * Function:    on_treeWidget_clicked()
 *
 * Description: This function is invoked when user double click
 *              on the stock list.
 *
 *
 *******************************************************************/
void ImportData::displayStockData(void)
{
    CDbHndl db;
    DataPlotIndex_ET plotIndex;
    QString legendLable;
    QString titleXAxis;
    CYahooStockPlotUtil::MinMaxAxisValues_ST mainPlotAxis;


    deleteAllPlotData(m_qwtStockPlotData, ui->qwtPlot);

    QString startDate = ui->setStartDateLineEdit->text();
    QString endDate = ui->setEndDateLineEdit->text();

    m_qwtStockPlotData.nofStocksToPlot = 0;

    // This variable is automaticly updated when first data is handled by UpdateMinMaxAxis()
    m_qwtStockPlotData.axis.minMaxIsInit = false;
    clearStockAndIndicatorTempMem();

    db.getYahooTaPriceData(m_reqStockSymbol, startDate, endDate, m_qwtStockPlotData, m_stockData);
    initMinMaxAxis(m_qwtStockPlotData.axis, mainPlotAxis);

    titleXAxis = "Tid (dagar)";
    legendLable = "Pris (SEK)";
    setAxisTitles(titleXAxis, legendLable, ui->qwtPlot, true);

    plotIndex = PLOT_INDEX_PRICE;
    addStockDataToPlot(plotIndex, legendLable);


    //  boxesUpdate edit
    ui->lineEditAssetName->clear();
    ui->lineEditAssetName->insert(m_reqStockName);


    // Reset zoomer
    m_plot.initPlotPicker(ui->qwtPlot);
}


/*******************************************************************
 *
 * Function:    clearStockAndIndicatorTempMem()
 *
 * Description:
 *
 *
 *******************************************************************/
void ImportData::clearStockAndIndicatorTempMem(void)
{
    m_stockData.data.x.clear();
    m_stockData.data.y.clear();
    m_stockData.data.xDate.clear();
    m_stockData.data.indicator1.clear();
    m_stockData.data.indicator2.clear();
    m_stockData.data.indicator3.clear();
    m_stockData.data.indicator4.clear();

    m_stockArr.clear();
}


/*******************************************************************
 *
 * Function:    deleteAllPlotData()
 *
 * Description:
 *
 *
 *******************************************************************/
void ImportData::deleteAllPlotData(CYahooStockPlotUtil::PlotData_ST &allPlotData, QwtPlot *qwtPlot)
{
    for(int i = 0; i < PLOT_INDEX_MAX; i++)
    {
        m_plot.removePlotData(allPlotData, i, qwtPlot);
    }

}


/*******************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *******************************************************************/
void ImportData::initMinMaxAxis(CYahooStockPlotUtil::MinMaxAxisValues_ST inputAxis,
               CYahooStockPlotUtil::MinMaxAxisValues_ST &outputAxis)
{
    outputAxis.minMaxIsInit = inputAxis.minMaxIsInit;
    outputAxis.maxX = inputAxis.maxX;
    outputAxis.maxY = inputAxis.maxY;
    outputAxis.minX  = inputAxis.minX;
    outputAxis.maxXDate  = inputAxis.maxXDate;
    outputAxis.minXDate  = inputAxis.minXDate;
    outputAxis.minY  = inputAxis.minY;
}


/*******************************************************************
 *
 * Function:    setAxisTitles()
 *
 * Description:
 *
 *
 *******************************************************************/
void ImportData::setAxisTitles(QString titleXAxis, QString titleYAxis, QwtPlot *qwtPlot,bool showXAxis)
{
    if(showXAxis==true)
        m_plot.setXAxisTitle(qwtPlot, titleXAxis);

    m_plot.setYAxisTitle(qwtPlot, titleYAxis);

}


/*******************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *******************************************************************/
void ImportData::addStockDataToPlot(DataPlotIndex_ET plotIndex,
                                    QString legendLable)
{
    m_stockArr.insert(0, m_stockData);

    // This is the first added graph
    m_plot.addStockToPlot(m_qwtStockPlotData, m_stockArr[0], m_qwtStockPlotData.axis, plotIndex, legendLable);

    m_plot.plotData(m_qwtStockPlotData, ui->qwtPlot, m_qwtStockPlotData.nofStocksToPlot-1);

    m_plot.removeAllStockArrData(m_stockArr);

}




/*******************************************************************
 *
 * Function:    slotReceivedAssetTaDataFromServer()
 *
 * Description:
 *
 *
 *******************************************************************/
void ImportData::slotReceivedAssetTaDataFromServer(int)
{
    if(m_singleStockDataReqStatus == STATUS_REQ_SINGLE_STOCK_PROCESSING)
    {
        // Init thread with data before starting it
        m_importYahooTaDataThread->setImportInfoSingleStock(m_reqStockName, m_reqStockSymbol);
        m_importYahooTaDataThread->start(QThread::NormalPriority);
    }
}


/*******************************************************************
 *
 * Function:    slotImportSingelStockDataIsFinish()
 *
 * Description: This function is invoked singel stock data is
 *              parsed by: m_importYahooTaDataThread
 *
 *******************************************************************/
void ImportData::slotImportSingelStockDataIsFinish(int dummy)
{
    dummy = dummy;
    if(m_singleStockDataReqStatus == STATUS_REQ_SINGLE_STOCK_PROCESSING)
    {
        m_singleStockDataReqStatus = STATUS_REQ_SINGLE_STOCK_IDLE;
        modifyDateList(m_reqStockSymbol, true);
        qDebug() << m_reqStockSymbol;
        displayStockData();
    }

}


/*******************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *******************************************************************/
void ImportData::modifyDateList(QString stockSymbol, bool isUpdate)
{
    NameKey node;
     QMutableListIterator <NameKey> iterator(m_dateIsUpdatedList);
     while(iterator.hasNext())
     {
        node = iterator.next();
        if(node.symbolName.compare(stockSymbol)== 0)
        {
            //iterator.remove();
            // addStatusDateList(stockSymbol, isUpdate);

            QString tmp;
            tmp.sprintf("%d",isUpdate);
            qDebug() << tmp;
            qDebug() << stockSymbol;
            node.dataIsUpdated = isUpdate;
            iterator.setValue(node);
            return;
        }

     }
}


/*******************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *******************************************************************/
void ImportData::on_selTimePeriodSlider_valueChanged(int value)
{
    m_timePeriodDaysInc = value;

    ui->timePeriodLlineEdit->clear();
    ui->timePeriodLlineEdit->insert((QString)m_timePeriodDaysArr[value].TxtTimePeriod.toLatin1());
    setTimePeriodDaysUpdateStartStopDate(m_startDate, m_endDate, value);
}



/*******************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *******************************************************************/
void ImportData::on_pushButtImportKeyTaBridgeData_clicked()
{
    ParseKeyTaBridgeData pktd;
    QString filename;

    // filename = "/home/ajn/Documents/swProj/MyQtProj/JackStockProj/JackStock/database/inputData/KeyTaBridgeData/NordnetNameYahooSymbol.csv";

    filename = DWLD_PATH_FA_TA_BRIDGE_DATA;
    pktd.ParseKeyTaBridgeData::readFile(filename);
    QMessageBox::information(NULL, QString::fromUtf8("Bridge Data"), QString::fromUtf8("Finish"));

}


#if 0
/*******************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *******************************************************************/
void ImportData::on_pushButDescription_clicked()
{
    parseCompDescription pcd;

    QString filename;
    filename = DWLD_PATH_FA_TA_COMPANY_DESCRIPTION;

    pcd.readFile(filename);
}
#endif

/*******************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *******************************************************************/
void ImportData::on_pushButDescription_clicked()
{
    QString path;
    QUrl url;
    CDbHndl db;
    CompanyNameAndId_ST data;
    QString req;
    QString assetId;
    m_oldCompanyListIndex = -1;
    m_displayFinish = true;

    m_companyList.clear();

    if(true == db.getListWithNordnetStockId(m_companyList))
    {
        db.delAllTblNordnetCompanyDescription();
        for(int i = 0; i < m_companyList.count(); i++)
        {
            data = m_companyList.at(i);
            qDebug() << i << data.assetId << data.AssetName;
        }

        m_companyListIndex = 0;
        data = m_companyList.at(0);
        path = DWLD_PATH_FA_TA_COMPANY_DESCRIPTION;
        assetId = data.assetId;
        assetId.trimmed();
        req.sprintf("https://www.nordnet.se/mux/web/marknaden/aktiehemsidan/bolagsfakta.html?identifier=%s&marketplace=11", assetId.toAscii().constData());
        url = req;

        reqCompanyDescriptionHtmlFile(path, url);
        m_sendNextReq = true;
    }
}


/****************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *
 *
 ****************************************************************/
void ImportData::reqCompanyDescriptionHtmlFile(QString path, QUrl url)
{
    if(m_waitOnServerResp == true)
    {
        QMessageBox::information(NULL, QString::fromUtf8("Hämta data"), QString::fromUtf8("Vänta: Upptagen med hämta data"));
        return;
    }

    QObject::connect(&m_hw1, SIGNAL(sendSignalTextToDlg2(int)), this, SLOT(slotCompanyDescriptionHtmlPageIsRecv(int)));

    m_hw1.startRequest(url, path, 0x01);
    m_waitOnServerResp = true;
}


/*******************************************************************
 *
 * Function:    slotHtmlPageIsRecv()
 *
 * Description: This function is invoked when a html page is
 *              completely received.
 *
 *
 *******************************************************************/
void ImportData::slotCompanyDescriptionHtmlPageIsRecv(int number)
{

    number = number;
    m_waitOnServerResp = false;

    if(m_companyListIndex < m_companyList.count()-1)
    {
        m_timeoutTimer->stop();
        if(m_sendNextReq == true)
        {
            if(m_oldCompanyListIndex <= m_companyListIndex)
            {
                qDebug() << "Index" << m_companyListIndex << "count" << m_companyList.count()-2;
                m_timeoutTimer->singleShot(200, this, SLOT(slotReqNextCompanyDescriptionData()));
                m_sendNextReq = false;
                m_oldCompanyListIndex = m_companyListIndex;
            }
        }
    }
    else
    {
        m_timeoutTimer->stop();
        if(m_displayFinish== true)
        {
            m_displayFinish = false;
            QMessageBox::information(this, QObject::tr("Finish"), QObject::tr("Finish"));
        }
    }
}


/****************************************************************
 *
 * Function:    ()
 *
 * Description: Request next file with company info
 *
 *
 *
 *
 ****************************************************************/
void ImportData::slotReqNextCompanyDescriptionData()
{
    QString path;
    QString assetId;
    CompanyNameAndId_ST data;
    QString req;
    QUrl url;

    parseCompDescription pcd;



    //QMessageBox::information(NULL, QObject::tr("Hämta data"), QString::fromUtf8("Filen hämtad"));
    m_timeoutTimer->stop();

    if(m_companyListIndex < m_companyList.count()-1)
    {
        m_oldCompanyListIndex = m_companyListIndex;
        data = m_companyList.at(m_companyListIndex);
        m_companyListIndex++;
        qDebug() << "m_companyListIndex" << m_companyListIndex;


        path = DWLD_PATH_FA_TA_COMPANY_DESCRIPTION;

        data.AssetName.trimmed();
        pcd.readFile(path, data.AssetName, data.assetId);

            // Parse file
        data = m_companyList.at(m_companyListIndex);
        path = DWLD_PATH_FA_TA_COMPANY_DESCRIPTION;
        assetId = data.assetId;
        assetId.trimmed();
        qDebug() << "name: " << data.AssetName << "Id: " << data.assetId;
        req.sprintf("https://www.nordnet.se/mux/web/marknaden/aktiehemsidan/bolagsfakta.html?identifier=%s&marketplace=11", assetId.toAscii().constData());
        url = req;
        m_waitOnServerResp = false;
        reqCompanyDescriptionHtmlFile(path, url);
        m_sendNextReq = true;

    }
    else
    {
        QMessageBox::information(this, QObject::tr("Finish"), QObject::tr("Finish"));
    }



}


/****************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *
 *
 ****************************************************************/
void ImportData::on_pushButParseCompanyInfo_clicked()
{
    QString path;
    ParseCompanyEconomyInfo pce;

    CompanyNameAndId_ST data;

    path = PATH_COMPANY_HIST_INFO;
    data.AssetName.trimmed();
    pce.readFile(path, data.AssetName);
    QMessageBox::information(this, QObject::tr("Finish"), QObject::tr("Finish"));

}


/****************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *
 *
 ****************************************************************/
void ImportData::on_pushButton_clicked()
{
    QString path;
    path = QDir::currentPath();
    qDebug() << path;

    // QString filename("database/inputData/Yahoo/keyStatistics/ABB_Key_Statistics.html");
    //QString filename("database/inputData/Yahoo/keyStatistics/KLOV-B.ST_Key Statistics.html");
    QString filename("database/inputData/Yahoo/keyStatistics/UNIB-SDB.ST_Key_Statistics.html");
    readYahooKeyStatistics(filename);
}

const QString ImportData::yahooKeyStatisticsArr[YAHOO_KEY_STATISTICS_ARR_SIZE] =
{
    QString::fromUtf8("Market Cap"),
    QString::fromUtf8("Enterprise Value"),
    QString::fromUtf8("Trailing P/E"),
    QString::fromUtf8("Forward P/E"),
    QString::fromUtf8("PEG Ratio"),
    QString::fromUtf8("Price/Sales"),
    QString::fromUtf8("Price/Book"),
    QString::fromUtf8("Enterprise Value/Revenue"),
    QString::fromUtf8("Enterprise Value/EBITDA"),
    QString::fromUtf8("Fiscal Year Ends"),
    QString::fromUtf8("Most Recent Quarter"),
    QString::fromUtf8("Profit Margin"),
    QString::fromUtf8("Operating Margin"),
    QString::fromUtf8("Return on Assets"),
    QString::fromUtf8("Return on Equity"),
    QString::fromUtf8("Revenue"),
    QString::fromUtf8("Revenue Per Share"),
    QString::fromUtf8("Qtrly Revenue Growth"),
    QString::fromUtf8("Gross Profit"),
    QString::fromUtf8("EBITDA"),
    QString::fromUtf8("Net Income Avl to Common"),
    QString::fromUtf8("Diluted EPS"),
    QString::fromUtf8("Qtrly Earnings Growth"),
    QString::fromUtf8("Total Cash"),
    QString::fromUtf8("Total Cash Per Share"),
    QString::fromUtf8("Total Debt"),
    QString::fromUtf8("Total Debt/Equity"),
    QString::fromUtf8("Current Ratio"),
    QString::fromUtf8("Book Value Per Share"),
    QString::fromUtf8("Operating Cash Flow"),
    QString::fromUtf8("Levered Free Cash Flow"),
    QString::fromUtf8("Beta"),
    QString::fromUtf8("52-Week Change"),
    QString::fromUtf8("S&amp;P500 52-Week Change"),
    QString::fromUtf8("52-Week High"),
    QString::fromUtf8("52-Week Low"),
    QString::fromUtf8("50-Day Moving Average"),
    QString::fromUtf8("200-Day Moving Average"),
    QString::fromUtf8("Avg Vol (3 month)"),
    QString::fromUtf8("Avg Vol (10 day)"),
    QString::fromUtf8("Shares Outstanding"),
    QString::fromUtf8("Float"),
    QString::fromUtf8("% Held by Insiders"),
    QString::fromUtf8("% Held by Institutions"),
    QString::fromUtf8("Shares Short"),
    QString::fromUtf8("Short Ratio"),
    QString::fromUtf8("Short % of Float"),
    QString::fromUtf8("Shares Short (prior month)"),
    QString::fromUtf8("Forward Annual Dividend Rate"),
    QString::fromUtf8("Forward Annual Dividend Yield"),
    QString::fromUtf8("Trailing Annual Dividend Yield"),
    QString::fromUtf8("Trailing Annual Dividend Yield"),
    QString::fromUtf8("5 Year Average Dividend Yield"),
    QString::fromUtf8("Payout Ratio"),
    QString::fromUtf8("Dividend Date"),
    QString::fromUtf8("Ex-Dividend Date"),
    QString::fromUtf8("Last Split Factor"),
    QString::fromUtf8("Last Split Date")
};



/****************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *
 *
 ****************************************************************/
bool ImportData::readYahooKeyStatistics(QString filename)
{
    QString outFilename("YahooKeyStat1.txt");
    QFile file(filename);
    QFile outFile(outFilename);

    QString str;
    QString errStr = QString("Fail to open file: %1").arg(filename);



    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::critical(NULL, QObject::tr("Fail to open file"), errStr);
        return false;
    }

    if(!outFile.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::critical(NULL, QObject::tr("Fail to open file"), errStr);
        return false;
    }


    QTextStream out(&outFile);   // we will serialize the data into the file
    QTextStream inStream(&file);

    inStream.setCodec("UTF-8");
    out.setCodec("UTF-8");


    int mainStrIndex;
    QString tmpStr;
    QString mainStr;
    QString subStr;
    QString startToken("<td class=\"yfnc_tablehead1\" width=\"74%\">");

    QString startDataToken("<td class=\"yfnc_tabledata1\">");

    int startTokenLen = startToken.length();
    int startDataTokenLen = startDataToken.length();
    int startIndex;
    int nofchars;
    bool found;
    int n = 0;

    m_yksState = YKS_STATE_READ_FILE;

    while(!inStream.atEnd())
    {
        switch(m_yksState)
        {
        case YKS_STATE_READ_FILE:
            mainStr = inStream.readLine();
            mainStrIndex = mainStr.indexOf(startToken);
            if((mainStrIndex > -1) && (mainStr.length() > 0))
            {
                qDebug() << mainStr;
                m_yksState = YKS_STATE_FIND_TOKEN;
            }
            break;

        case YKS_STATE_FIND_TOKEN:
            found = false;
            mainStrIndex = mainStr.indexOf(startToken);
            startIndex = mainStrIndex + startTokenLen;

            if(mainStrIndex > -1)
            {
                for(int i = startIndex; i < mainStr.length(); i++)
                {
                    if(mainStr.at(i) == '<')
                    {
                        nofchars = i - startIndex;
                        if(nofchars >= 0)
                        {
                            subStr = mainStr.mid(startIndex, nofchars);
                            qDebug() << subStr;
                            // QString::size_type pos = yahooKeyStatisticsArr[n+1].toStdString().find(subStr.toStdString());
                            qWarning() << subStr.contains(yahooKeyStatisticsArr[n]);

                            if(false == subStr.contains(yahooKeyStatisticsArr[n]))
                            {
                                str = QString::fromUtf8("Fail to parse: ");
                                str += yahooKeyStatisticsArr[n];
                                QMessageBox::information(this, QString::fromUtf8("Fail:"), str);

                                return false;
                            }

                            out << subStr.toUtf8() << "\n";
                            mainStr = mainStr.right(mainStr.length() - (mainStrIndex + startTokenLen));
                            found = true;
                            n++;
                            m_yksState = YKS_STATE_FIND_DATA_TOKEN;
                            break;
                        }
                    }
                }
            }

            if(found == false)
            {
                m_yksState = YKS_STATE_READ_FILE;
            }
           break;


         case YKS_STATE_FIND_DATA_TOKEN:
            found = false;
            mainStrIndex = mainStr.indexOf(startDataToken);
            startIndex = mainStrIndex + startDataTokenLen;
            tmpStr = mainStr.right(mainStr.length() - startIndex);

            if(mainStrIndex > -1)
            {
                for(int i = startIndex; i < mainStr.length(); i++)
                {
                    if(mainStr.at(i) == '<')
                    {
                        nofchars = i - startIndex;
                        if(nofchars >= 0)
                        {
                            subStr = mainStr.mid(startIndex, nofchars);
                            qDebug() << subStr;
                            out << subStr.toUtf8() << "\n";
                            mainStr = mainStr.right(mainStr.length() - (mainStrIndex + startDataTokenLen));
                            found = true;
                            m_yksState = YKS_STATE_FIND_TOKEN;
                            break;
                        }

                    }
                }
            }

            if(found == false)
            {
                m_yksState = YKS_STATE_READ_FILE;
            }
            break;

        }


    }



    QMessageBox::information(this, QString::fromUtf8("Finish"), QString::fromUtf8("Finish"));
    file.close();
    outFile.close();
    return true;
}

#if 0
/****************************************************************
 *
 * Function:    ()
 *
 * Description:
 *
 *
 *
 *
 ****************************************************************/
bool ImportData::readYahooKeyStatistics(QString filename)
{
    QString outFilename("YahooKeyStat1.txt");
    QFile file(filename);
    QFile outFile(outFilename);

    QString str;
    QString errStr = QString("Fail to open file: %1").arg(filename);



    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::critical(NULL, QObject::tr("Fail to open file"), errStr);
        return false;
    }

    if(!outFile.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::critical(NULL, QObject::tr("Fail to open file"), errStr);
        return false;
    }


    QTextStream out(&outFile);   // we will serialize the data into the file
    QTextStream inStream(&file);

    inStream.setCodec("UTF-8");
    out.setCodec("UTF-8");


    int mainStrIndex;
    int mainStrEndIndex;
    QString tmpStr;
    QString mainStr;
    QString subStr;
    QString startToken("<td class=\"yfnc_tablehead1\"");
    //QString endToken("</td>");
    QString endToken("<");
    int startTokenLen = startToken.length();
    int subStrIndex;

    m_yksState = YKS_STATE_READ_FILE;

    while(!inStream.atEnd())
    {
        switch(m_yksState)
        {
        case YKS_STATE_READ_FILE:
            mainStr = inStream.readLine();
            mainStrIndex = mainStr.indexOf(startToken);
            if((mainStrIndex > -1) && (mainStr.length() > 0))
            {
                qDebug() << mainStr;
                m_yksState = YKS_STATE_FIND_TOKEN;
            }
            break;

        case YKS_STATE_FIND_TOKEN:
            mainStrIndex = mainStr.indexOf(startToken);

            if(mainStrIndex > -1)
            {
                tmpStr = mainStr.right(mainStr.length() - (mainStrIndex + startTokenLen));
                mainStr = mainStr.right(mainStr.length() - (mainStrIndex + startTokenLen));

                if(mainStr.length() > 0)
                {
                    mainStrEndIndex = mainStr.indexOf(endToken);
                    if(mainStrEndIndex > 0)
                    {
                        subStr = mainStr.left(mainStrEndIndex);
                        m_yksState = YKS_STATE_SPLIT_STR;
                    }
                    else
                    {
                        m_yksState = YKS_STATE_READ_FILE;
                    }
                }
            }
            else
            {
                m_yksState = YKS_STATE_READ_FILE;
            }
            break;

        case YKS_STATE_SPLIT_STR:
            subStrIndex = subStr.indexOf(">");
            if(subStrIndex > -1)
            {
                subStr = subStr.mid(subStrIndex + 1, subStr.length());
                qDebug() << subStr;
                out << subStr.toUtf8() << "\n";
                m_yksState = YKS_STATE_FIND_TOKEN;
            }
            break;
        }


    }



    QMessageBox::information(this, QObject::tr("Finish"), QObject::tr("Finish"));
    file.close();
    outFile.close();
    return true;
}
#endif


