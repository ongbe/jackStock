#include "dbHndl.h"
#include <QMessageBox>
#include <stdlib.h>



/****************************************************************
 *
 * Function:    subAnalysisEarningsDateExists()
 *
 * Description:
 *
 *
 *
 *
 *
 ****************************************************************/
bool CDbHndl::subAnalysisEarningsDateExists(QString date,
                                     int mainAnalysisId,
                                     int &earningsDateId)
{
    QSqlRecord rec;
    QString str;

    m_mutex.lock();
    openDb(PATH_JACK_STOCK_DB);
    QSqlQuery qry(m_db);



    QByteArray ba = date.toLocal8Bit();
    const char *c_date = ba.data();



    str.sprintf("SELECT * "
                " FROM  TblDateEarningsSubAnalysis "
                " WHERE DateEarnings = '%s' AND MainAnalysisId = %d;",
                c_date,
                mainAnalysisId);

    qDebug() << str;


    qry.prepare(str);


    if( !qry.exec() )
    {
        if(m_disableMsgBoxes == false)
        {
            QMessageBox::critical(NULL, QString::fromUtf8("TblDateEarningsSubAnalysis error 1"), qry.lastError().text().toUtf8().constData());
        }
        qDebug() << qry.lastError();
        closeDb();
        m_mutex.unlock();
        return false;
    }
    else
    {
        if(qry.next())
        {
            rec = qry.record();



            if(rec.value("DateEarnings").isNull() == true || true == rec.value("MainAnalysisId").isNull())
            {
                qry.finish();
                closeDb();
                m_mutex.unlock();
                return false;
            }
            else
            {
                earningsDateId = rec.value("DateEarningsId").toInt();
                qry.finish();
                closeDb();
                m_mutex.unlock();
                return true;
            }
        }
    }

    qry.finish();
    closeDb();
    m_mutex.unlock();
    return false;
}




/****************************************************************
 *
 * Function:    insertSubAnalysisEarningsDate()
 *
 * Description:
 *
 *
 *
 *
 *
 ****************************************************************/
bool CDbHndl::
insertSubAnalysisEarningsDate(QString date,
                       int mainAnalysisId,
                       int &dateEarningsId,
                       bool dbIsHandledExternly)
{
    QString str;

    if(dbIsHandledExternly == false)
    {
        m_mutex.lock();
        openDb(PATH_JACK_STOCK_DB);
    }

    QSqlQuery qry(m_db);

    QByteArray ba = date.toLocal8Bit();
    const char *c_date = ba.data();


    str.sprintf("INSERT OR REPLACE INTO TblDateEarningsSubAnalysis "
                "(DateEarnings, MainAnalysisId) "
                " VALUES('%s', %d);",
                c_date,
                mainAnalysisId);

    qDebug() << str;

    qry.prepare(str);

    if(!qry.exec())
    {
        qDebug() << qry.lastError();

        if(m_disableMsgBoxes == false)
        {
            QMessageBox::critical(NULL, QString::fromUtf8("TblDateEarningsSubAnalysis"), qry.lastError().text().toUtf8().constData());
        }

        if(dbIsHandledExternly == false)
        {
            closeDb();
            m_mutex.unlock();
        }
        return false;
    }


    dateEarningsId = (int) qry.lastInsertId().toInt();

    qry.finish();

    if(dbIsHandledExternly==false)
    {
        closeDb();
        m_mutex.unlock();
    }

    return true;
}






/*****************************************************************
 *
 * Function:		getSubAnalysisEarningsId()
 *
 * Description:
 *
 *
 *
 *****************************************************************/
bool CDbHndl::
getSubAnalysisEarningsId(int mainAnalysisId,
                  int earningsDateId,
                  int &earningsDataId,
                  bool dbIsHandledExternly)
{


    QSqlRecord rec;
    QString str;

    if(dbIsHandledExternly == false)
    {
        m_mutex.lock();
        openDb(PATH_JACK_STOCK_DB);
    }

    QSqlQuery qry(m_db);

  bool found = false;


    str.sprintf("SELECT TblDataEarningsSubAnalysis.DataEarningsId, TblDataEarningsSubAnalysis.DateEarningsId, TblDataEarningsSubAnalysis.MainAnalysisId   "
                " FROM TblDataEarningsSubAnalysis   "
                " WHERE  "
                "       TblDataEarningsSubAnalysis.DateEarningsId = %d AND "
                "       TblDataEarningsSubAnalysis.MainAnalysisId = %d;",
                                                                           earningsDateId,
                                                                           mainAnalysisId);


    qDebug() << str << "\n";

    qry.prepare(str);


    if( !qry.exec() )
    {
        if(m_disableMsgBoxes == false)
        {
            QMessageBox::critical(NULL, QString::fromUtf8("db error"), qry.lastError().text().toUtf8().constData());
        }
        qDebug() << qry.lastError();
        if(dbIsHandledExternly == false)
        {
            closeDb();
            m_mutex.unlock();
        }
        return false;
    }
    else
    {
        while(qry.next())
        {
            rec = qry.record();




            if(rec.value("DataEarningsId").isNull() == true)
            {

                if(found == true)
                {
                    continue;
                }
                else
                {
                    qry.finish();
                    if(dbIsHandledExternly == false)
                    {
                        closeDb();
                        m_mutex.unlock();
                    }

                    return false;
                }
            }
            else
            {
                found = true;
                qDebug() << rec.value("DataEarningsId").toString();
                qDebug() << rec.value("MainAnalysisId").toString();


                if(rec.value("DataEarningsId").isNull() == false)
                {
                    earningsDataId = rec.value("DataEarningsId").toInt();
                }

            }
        }
    }


    qry.finish();
    if(dbIsHandledExternly == false)
    {
        closeDb();
        m_mutex.unlock();
    }

    if(found == true)
    {
        return true;
    }

    return false;
}





/****************************************************************
 *
 * Function:    insertSubEarnings()
 *
 * Description:
 *
 *
 *
 *
 *
 ****************************************************************/
bool CDbHndl::
insertSubAnalysisEarnings(int earningsDateId,
                          int mainAnalysisId,
                          int inputEarningsDataId,
                          bool earningsDataIdIsValid,
                          QString dataEarnings,
                          int &earningsDataId,
                          bool dbIsHandledExternly)
{
    QString str;

    if(dbIsHandledExternly == false)
    {
        m_mutex.lock();
        openDb(PATH_JACK_STOCK_DB);
    }


    QSqlQuery qry(m_db);


    if(earningsDataIdIsValid == true)
    {
        str.sprintf("INSERT OR REPLACE INTO TblDataEarningsSubAnalysis "
                " (DateEarningsId, "
                "  DataEarnings, "
                 " DataEarningsId,"
                 " MainAnalysisId) "
                 " VALUES( %d,  '%s', %d, %d);",
                        earningsDateId,
                        dataEarnings.toLocal8Bit().constData(),
                        inputEarningsDataId,
                        mainAnalysisId);
    }
    // New data
    else
    {
        str.sprintf("INSERT INTO TblDataEarningsSubAnalysis "
                    " (DateEarningsId, "
                    " DataEarnings, "
                    " MainAnalysisId) "
                    " VALUES( %d, '%s', %d);",
                            earningsDateId,
                            dataEarnings.toLocal8Bit().constData(),
                            mainAnalysisId);
    }


    qDebug() << str;

    qry.prepare(str);


    if(!qry.exec())
    {
        qDebug() << qry.lastError();
        if(m_disableMsgBoxes == false)
        {
            QMessageBox::critical(NULL, QString::fromUtf8("TblDataEarningsSubAnalysis"), qry.lastError().text().toLatin1().constData());
        }

        if(dbIsHandledExternly == false)
        {
            closeDb();
            m_mutex.unlock();
        }
        return false;
    }


    earningsDataId = (int) qry.lastInsertId().toInt();


    qry.finish();
    if(dbIsHandledExternly == false)
    {
        closeDb();
        m_mutex.unlock();
    }

    return true;
}







/*****************************************************************
 *
 * Function:		getSubAnalysisEarningsData()
 *
 * Description:
 *
 *
 *
 *****************************************************************/
bool CDbHndl::
getSubAnalysisEarningsData(QString stockName,
                           QString stockSymbol,
                           EarningsDataST *earningsDataArr,
                           int &nofEarningsArrData,
                           bool dbIsHandledExternly)
{


    QSqlRecord rec;
    QString str;
    EarningsDataST data;

    nofEarningsArrData = 0;


    if(dbIsHandledExternly == false)
    {
        m_mutex.lock();
        openDb(PATH_JACK_STOCK_DB);
    }

    QSqlQuery qry(m_db);

  bool found = false;


    str.sprintf("SELECT TblMainAnalysis.*, TblDateEarningsSubAnalysis.*, TblDataEarningsSubAnalysis.* "
                " FROM TblMainAnalysis, TblDateEarningsSubAnalysis, TblDataEarningsSubAnalysis  "
                " WHERE  "
                "       TblMainAnalysis.MainAnalysisId = TblDateEarningsSubAnalysis.MainAnalysisId AND "
                "       TblMainAnalysis.MainAnalysisId = TblDataEarningsSubAnalysis.MainAnalysisId AND "
                "       TblDateEarningsSubAnalysis.DateEarningsId = TblDataEarningsSubAnalysis.DateEarningsId AND "
                "       lower(TblMainAnalysis.stockName) = lower('%s') AND "
                "       lower(TblMainAnalysis.StockSymbol) = lower('%s') "
                " ORDER BY (CAST(TblDateEarningsSubAnalysis.DateEarnings AS REAL)) ASC;",              // DESC";",
                                                                           stockName.toLocal8Bit().constData(),
                                                                           stockSymbol.toLocal8Bit().constData());


    qDebug() << str << "\n";

    qry.prepare(str);


    if( !qry.exec() )
    {
        if(m_disableMsgBoxes == false)
        {
            QMessageBox::critical(NULL, QString::fromUtf8("db error"), qry.lastError().text().toLatin1().constData());
        }
        qDebug() << qry.lastError();
        if(dbIsHandledExternly == false)
        {
            closeDb();
            m_mutex.unlock();
        }
        return false;
    }
    else
    {
        while(qry.next())
        {
            rec = qry.record();

            if((rec.value("DateEarnings").isNull() == true) ||  // Date
                (rec.value("DataEarnings").isNull() == true))   // Data
            {

                if(found == true)
                {
                    continue;
                }
                else
                {
                    qry.finish();
                    if(dbIsHandledExternly == false)
                    {
                        closeDb();
                        m_mutex.unlock();
                    }

                    return false;
                }
            }
            else
            {
                found = true;
                qDebug() << rec.value("DateEarnings").toString() << "\n";
                qDebug() << rec.value("stockSymbol").toString();
                qDebug() << rec.value("stockName").toString();




                // Date
                data.date.clear();
                if(rec.value("DateEarnings").isNull() == false)
                {
                    data.date = rec.value("DateEarnings").toString();
                }

                // Data
                data.earnings.clear();
                if(rec.value("DataEarnings").isNull() == false)
                {
                    data.earnings = rec.value("DataEarnings").toString();
                }

                earningsDataArr[nofEarningsArrData] = data;
                nofEarningsArrData++;

            }
        }
    }


    qry.finish();
    if(dbIsHandledExternly == false)
    {
        closeDb();
        m_mutex.unlock();
    }

    if(found == true)
    {
        return true;
    }

    return false;
}








//========================
//========================



/****************************************************************
 *
 * Function:    subAnalysisDividendDateExists()
 *
 * Description:
 *
 *
 *
 *
 *
 ****************************************************************/
bool CDbHndl::subAnalysisDividendDateExists(QString date,
                                     int mainAnalysisId,
                                     int &dateDividendId)
{
    QSqlRecord rec;
    QString str;

    m_mutex.lock();
    openDb(PATH_JACK_STOCK_DB);
    QSqlQuery qry(m_db);



    QByteArray ba = date.toLocal8Bit();
    const char *c_date = ba.data();



    str.sprintf("SELECT * "
                " FROM  TblDateDividendSubAnalysis "
                " WHERE DateDividend = '%s' AND MainAnalysisId = %d;",
                c_date,
                mainAnalysisId);

    qDebug() << str;


    qry.prepare(str);


    if( !qry.exec() )
    {
        if(m_disableMsgBoxes == false)
        {
            QMessageBox::critical(NULL, QString::fromUtf8("TblDateDividendSubAnalysis error 1"), qry.lastError().text().toUtf8().constData());
        }
        qDebug() << qry.lastError();
        closeDb();
        m_mutex.unlock();
        return false;
    }
    else
    {
        if(qry.next())
        {
            rec = qry.record();



            if(rec.value("DateDividend").isNull() == true || true == rec.value("MainAnalysisId").isNull())
            {
                qry.finish();
                closeDb();
                m_mutex.unlock();
                return false;
            }
            else
            {
                dateDividendId = rec.value("DateDividendId").toInt();
                qry.finish();
                closeDb();
                m_mutex.unlock();
                return true;
            }
        }
    }

    qry.finish();
    closeDb();
    m_mutex.unlock();
    return false;
}




/****************************************************************
 *
 * Function:    insertSubAnalysisDividendDate()
 *
 * Description:
 *
 *
 *
 *
 *
 ****************************************************************/
bool CDbHndl::
insertSubAnalysisDividendDate(QString date,
                       int mainAnalysisId,
                       int &dateDividendId,
                       bool dbIsHandledExternly)
{
    QString str;

    if(dbIsHandledExternly == false)
    {
        m_mutex.lock();
        openDb(PATH_JACK_STOCK_DB);
    }

    QSqlQuery qry(m_db);

    QByteArray ba = date.toLocal8Bit();
    const char *c_date = ba.data();


    str.sprintf("INSERT OR REPLACE INTO TblDateDividendSubAnalysis "
                "(DateDividend, MainAnalysisId) "
                " VALUES('%s', %d);",
                c_date,
                mainAnalysisId);

    qDebug() << str;

    qry.prepare(str);

    if(!qry.exec())
    {
        qDebug() << qry.lastError();
        if(m_disableMsgBoxes == false)
        {
            QMessageBox::critical(NULL, QString::fromUtf8("TblDateDividendSubAnalysis"), qry.lastError().text().toUtf8().constData());
        }
        if(dbIsHandledExternly==false)
        {
            closeDb();
            m_mutex.unlock();
        }
        return false;
    }


    dateDividendId = (int) qry.lastInsertId().toInt();

    qry.finish();

    if(dbIsHandledExternly==false)
    {
        closeDb();
        m_mutex.unlock();
    }

    return true;
}






/*****************************************************************
 *
 * Function:		getAnalysisDataId()
 *
 * Description:
 *
 *
 *
 *****************************************************************/
bool CDbHndl::
getSubAnalysisDividendId(int mainAnalysisId,
                  int dividendDateId,
                  int &dividendDataId,
                  bool dbIsHandledExternly)
{


    QSqlRecord rec;
    QString str;

    if(dbIsHandledExternly == false)
    {
        m_mutex.lock();
        openDb(PATH_JACK_STOCK_DB);
    }

    QSqlQuery qry(m_db);

  bool found = false;


    str.sprintf("SELECT TblDataDividendSubAnalysis.DataDividendId, TblDataDividendSubAnalysis.DateDividendId, TblDataDividendSubAnalysis.MainAnalysisId   "
                " FROM TblDataDividendSubAnalysis   "
                " WHERE  "
                "       TblDataDividendSubAnalysis.DateDividendId = %d AND "
                "       TblDataDividendSubAnalysis.MainAnalysisId = %d;",
                                                                           dividendDateId,
                                                                           mainAnalysisId);


    qDebug() << str << "\n";

    qry.prepare(str);


    if( !qry.exec() )
    {
        if(m_disableMsgBoxes == false)
        {
            QMessageBox::critical(NULL, QString::fromUtf8("db error"), qry.lastError().text().toUtf8().constData());
        }
        qDebug() << qry.lastError();
        if(dbIsHandledExternly == false)
        {
            closeDb();
            m_mutex.unlock();
        }
        return false;
    }
    else
    {
        while(qry.next())
        {
            rec = qry.record();




            if(rec.value("DateDividendId").isNull() == true)
            {

                if(found == true)
                {
                    continue;
                }
                else
                {
                    qry.finish();
                    if(dbIsHandledExternly == false)
                    {
                        closeDb();
                        m_mutex.unlock();
                    }

                    return false;
                }
            }
            else
            {
                found = true;
                qDebug() << rec.value("DateDividendId").toString();
                qDebug() << rec.value("MainAnalysisId").toString();


                if(rec.value("DataDividendId").isNull() == false)
                {
                    dividendDataId = rec.value("DataDividendId").toInt();
                }

            }
        }
    }


    qry.finish();
    if(dbIsHandledExternly == false)
    {
        closeDb();
        m_mutex.unlock();
    }

    if(found == true)
    {
        return true;
    }

    return false;
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
 *
 ****************************************************************/
bool CDbHndl::
insertSubAnalysisDividend(int dividendDateId,
                          int mainAnalysisId,
                          int inputDividendDataId,
                          bool dividendDataIdIsValid,
                          QString dataDividend,
                          int &dividendDataId,
                          bool dbIsHandledExternly)
{
    QString str;

    if(dbIsHandledExternly==false)
    {
        m_mutex.lock();
        openDb(PATH_JACK_STOCK_DB);
    }


    QSqlQuery qry(m_db);


    if(dividendDataIdIsValid == true)
    {
        str.sprintf("INSERT OR REPLACE INTO TblDataDividendSubAnalysis "
                " (DateDividendId, "
                "  DataDividend, "
                 " DataDividendId,"
                 " MainAnalysisId) "
                 " VALUES( %d,  '%s', %d, %d);",
                        dividendDateId,
                        dataDividend.toLocal8Bit().constData(),
                        inputDividendDataId,
                        mainAnalysisId);
    }
    // New data
    else
    {
        str.sprintf("INSERT INTO TblDataDividendSubAnalysis "
                    " (DateDividendId, "
                    " DataDividend, "
                    " MainAnalysisId) "
                    " VALUES( %d, '%s', %d);",
                            dividendDateId,
                            dataDividend.toLocal8Bit().constData(),
                            mainAnalysisId);
    }


    qDebug() << str;

    qry.prepare(str);


    if(!qry.exec())
    {
        qDebug() << qry.lastError();
        if(m_disableMsgBoxes == false)
        {
            QMessageBox::critical(NULL, QString::fromUtf8("TblDataDividendSubAnalysis"), qry.lastError().text().toLatin1().constData());
        }

        if(dbIsHandledExternly == false)
        {
            closeDb();
            m_mutex.unlock();
        }
        return false;
    }


    dividendDataId = (int) qry.lastInsertId().toInt();


    qry.finish();
    if(dbIsHandledExternly == false)
    {
        closeDb();
        m_mutex.unlock();
    }

    return true;
}







/*****************************************************************
 *
 * Function:		getSubAnalysisDividendData()
 *
 * Description:
 *
 *
 *
 *****************************************************************/
bool CDbHndl::
getSubAnalysisDividendData(QString stockName,
                           QString stockSymbol,
                           DividendDataST *dividendDataArr,
                           int &nofDividendArrData,
                           bool dbIsHandledExternly)
{


    QSqlRecord rec;
    QString str;
    DividendDataST data;

    nofDividendArrData = 0;


    if(dbIsHandledExternly == false)
    {
        m_mutex.lock();
        openDb(PATH_JACK_STOCK_DB);
    }

    QSqlQuery qry(m_db);

  bool found = false;


    str.sprintf("SELECT TblMainAnalysis.*, TblDateDividendSubAnalysis.*, TblDataDividendSubAnalysis.* "
                " FROM TblMainAnalysis, TblDateDividendSubAnalysis, TblDataDividendSubAnalysis  "
                " WHERE  "
                "       TblMainAnalysis.MainAnalysisId = TblDateDividendSubAnalysis.MainAnalysisId AND "
                "       TblMainAnalysis.MainAnalysisId = TblDataDividendSubAnalysis.MainAnalysisId AND "
                "       TblDateDividendSubAnalysis.DateDividendId = TblDataDividendSubAnalysis.DateDividendId AND "
                "       lower(TblMainAnalysis.stockName) = lower('%s') AND "
                "       lower(TblMainAnalysis.StockSymbol) = lower('%s') "
                " ORDER BY (CAST(TblDateDividendSubAnalysis.DateDividend AS REAL)) ASC;",              // DESC";",
                //" ORDER BY (TblDateDividendSubAnalysis.DateDividend) ASC;",              // DESC";",
                                                                           stockName.toLocal8Bit().constData(),
                                                                           stockSymbol.toLocal8Bit().constData());


    qDebug() << str << "\n";

    qry.prepare(str);


    if( !qry.exec() )
    {
        if(m_disableMsgBoxes == false)
        {
            QMessageBox::critical(NULL, QString::fromUtf8("db error"), qry.lastError().text().toLatin1().constData());
        }
        qDebug() << qry.lastError();
        if(dbIsHandledExternly == false)
        {
            closeDb();
            m_mutex.unlock();
        }
        return false;
    }
    else
    {
        while(qry.next())
        {
            rec = qry.record();




            if((rec.value("DateDividend").isNull() == true) || (rec.value("DataDividend").isNull() == true))
            {

                if(found == true)
                {
                    continue;
                }
                else
                {
                    qry.finish();
                    if(dbIsHandledExternly == false)
                    {
                        closeDb();
                        m_mutex.unlock();
                    }

                    return false;
                }
            }
            else
            {
                found = true;
                qDebug() << rec.value("DateDividend").toString() << "\n";
                qDebug() << rec.value("stockSymbol").toString();
                qDebug() << rec.value("stockName").toString();




                // Date
                data.date.clear();
                if(rec.value("DateDividend").isNull() == false)
                {
                    data.date = rec.value("DateDividend").toString();
                }

                // Data
                data.dividend.clear();
                if(rec.value("DataDividend").isNull() == false)
                {
                    data.dividend = rec.value("DataDividend").toString();
                }

                dividendDataArr[nofDividendArrData] = data;
                nofDividendArrData++;

            }
        }
    }


    qry.finish();
    if(dbIsHandledExternly == false)
    {
        closeDb();
        m_mutex.unlock();
    }

    if(found == true)
    {
        return true;
    }

    return false;
}





/****************************************************************
 *
 * Function:    subAnalysisTotalCurrentAssetsDateExists()
 *
 * Description:
 *
 *
 *
 *
 *
 ****************************************************************/
bool CDbHndl::subAnalysisTotalCurrentAssetsDateExists(QString date,
                                                      int mainAnalysisId,
                                                      int &totalCurrentAssetsDateId)
{
    QSqlRecord rec;
    QString str;

    m_mutex.lock();
    openDb(PATH_JACK_STOCK_DB);
    QSqlQuery qry(m_db);



    QByteArray ba = date.toLocal8Bit();
    const char *c_date = ba.data();



    str.sprintf("SELECT * "
                " FROM  TblDateTotalCurrentAssetsSubAnalysis "
                " WHERE DateTotalCurrentAssets = '%s' AND MainAnalysisId = %d;",
                c_date,
                mainAnalysisId);

    qDebug() << str;


    qry.prepare(str);


    if( !qry.exec() )
    {
        if(m_disableMsgBoxes == false)
        {
            QMessageBox::critical(NULL, QString::fromUtf8("TblDateTotalCurrentAssetsSubAnalysis error 1"), qry.lastError().text().toUtf8().constData());
        }
        qDebug() << qry.lastError();
        closeDb();
        m_mutex.unlock();
        return false;
    }
    else
    {
        if(qry.next())
        {
            rec = qry.record();



            if(rec.value("DateTotalCurrentAssets").isNull() == true || true == rec.value("MainAnalysisId").isNull())
            {
                qry.finish();
                closeDb();
                m_mutex.unlock();
                return false;
            }
            else
            {
                totalCurrentAssetsDateId = rec.value("DateTotalCurrentAssetsId").toInt();
                qry.finish();
                closeDb();
                m_mutex.unlock();
                return true;
            }
        }
    }

    qry.finish();
    closeDb();
    m_mutex.unlock();
    return false;
}






/****************************************************************
 *
 * Function:    insertSubAnalysisTotalCurrentAssetsDate()
 *
 * Description:
 *
 *
 *
 *
 *
 ****************************************************************/
bool CDbHndl::
insertSubAnalysisTotalCurrentAssetsDate(QString date,
                       int mainAnalysisId,
                       int &dateTotalCurrentAssetsId,
                       bool dbIsHandledExternly)
{
    QString str;

    if(dbIsHandledExternly == false)
    {
        m_mutex.lock();
        openDb(PATH_JACK_STOCK_DB);
    }

    QSqlQuery qry(m_db);

    QByteArray ba = date.toLocal8Bit();
    const char *c_date = ba.data();


    str.sprintf("INSERT OR REPLACE INTO TblDateTotalCurrentAssetsSubAnalysis "
                "(DateTotalCurrentAssets, MainAnalysisId) "
                " VALUES('%s', %d);",
                c_date,
                mainAnalysisId);

    qDebug() << str;

    qry.prepare(str);

    if(!qry.exec())
    {
        qDebug() << qry.lastError();

        if(m_disableMsgBoxes == false)
        {
            QMessageBox::critical(NULL, QString::fromUtf8("TblDateTotalCurrentAssetsSubAnalysis"), qry.lastError().text().toUtf8().constData());
        }

        if(dbIsHandledExternly == false)
        {
            closeDb();
            m_mutex.unlock();
        }
        return false;
    }


    dateTotalCurrentAssetsId = (int) qry.lastInsertId().toInt();

    qry.finish();

    if(dbIsHandledExternly==false)
    {
        closeDb();
        m_mutex.unlock();
    }

    return true;
}







/*****************************************************************
 *
 * Function:		getSubAnalysisTotalCurrentAssetsDataId()
 *
 * Description:
 *
 *
 *
 *****************************************************************/
bool CDbHndl::
getSubAnalysisTotalCurrentAssetsDataId(int mainAnalysisId,
                                       int totalCurrentAssetsDateId,
                                       int &totalCurrentAssetsDataId,
                                       bool dbIsHandledExternly)
{


    QSqlRecord rec;
    QString str;

    if(dbIsHandledExternly == false)
    {
        m_mutex.lock();
        openDb(PATH_JACK_STOCK_DB);
    }

    QSqlQuery qry(m_db);

  bool found = false;


    str.sprintf("SELECT TblDataTotalCurrentAssetsSubAnalysis.DataTotalCurrentAssetsId, TblDataTotalCurrentAssetsSubAnalysis.DateTotalCurrentAssetsId, TblDataTotalCurrentAssetsSubAnalysis.MainAnalysisId   "
                " FROM TblDataTotalCurrentAssetsSubAnalysis   "
                " WHERE  "
                "       TblDataTotalCurrentAssetsSubAnalysis.DateTotalCurrentAssetsId = %d AND "
                "       TblDataTotalCurrentAssetsSubAnalysis.MainAnalysisId = %d;",
                                                                           totalCurrentAssetsDateId,
                                                                           mainAnalysisId);


    qDebug() << str << "\n";

    qry.prepare(str);


    if( !qry.exec() )
    {
        if(m_disableMsgBoxes == false)
        {
            QMessageBox::critical(NULL, QString::fromUtf8("db error"), qry.lastError().text().toUtf8().constData());
        }
        qDebug() << qry.lastError();
        if(dbIsHandledExternly == false)
        {
            closeDb();
            m_mutex.unlock();
        }
        return false;
    }
    else
    {
        while(qry.next())
        {
            rec = qry.record();




            if(rec.value("DataTotalCurrentAssetsId").isNull() == true)
            {

                if(found == true)
                {
                    continue;
                }
                else
                {
                    qry.finish();
                    if(dbIsHandledExternly == false)
                    {
                        closeDb();
                        m_mutex.unlock();
                    }

                    return false;
                }
            }
            else
            {
                found = true;
                qDebug() << rec.value("DataTotalCurrentAssetsId").toString();
                qDebug() << rec.value("MainAnalysisId").toString();


                if(rec.value("DataTotalCurrentAssetsId").isNull() == false)
                {
                    totalCurrentAssetsDataId = rec.value("DataTotalCurrentAssetsId").toInt();
                }

            }
        }
    }


    qry.finish();
    if(dbIsHandledExternly == false)
    {
        closeDb();
        m_mutex.unlock();
    }

    if(found == true)
    {
        return true;
    }

    return false;
}







/****************************************************************
 *
 * Function:    insertSubEarnings()
 *
 * Description:
 *
 *
 *
 *
 *
 ****************************************************************/
bool CDbHndl::
insertSubAnalysisTotalCurrentAssets(int totalCurrentAssetsDateId,
                          int mainAnalysisId,
                          int inputTotalCurrentAssetsDataId,
                          bool totalCurrentAssetsDataIdIsValid,
                          QString dataTotalCurrentAssets,
                          int &totalCurrentAssetsDataId,
                          bool dbIsHandledExternly)
{
    QString str;

    if(dbIsHandledExternly == false)
    {
        m_mutex.lock();
        openDb(PATH_JACK_STOCK_DB);
    }


    QSqlQuery qry(m_db);


    if(totalCurrentAssetsDataIdIsValid == true)
    {
        str.sprintf("INSERT OR REPLACE INTO TblDataTotalCurrentAssetsSubAnalysis "
                " (DateTotalCurrentAssetsId, "
                "  DataTotalCurrentAssets, "
                 " DataTotalCurrentAssetsId,"
                 " MainAnalysisId) "
                 " VALUES( %d,  '%s', %d, %d);",
                        totalCurrentAssetsDateId,
                        dataTotalCurrentAssets.toLocal8Bit().constData(),
                        inputTotalCurrentAssetsDataId,
                        mainAnalysisId);
    }
    // New data
    else
    {
        str.sprintf("INSERT INTO TblDataTotalCurrentAssetsSubAnalysis "
                    " (DateTotalCurrentAssetsId, "
                    " DataTotalCurrentAssets, "
                    " MainAnalysisId) "
                    " VALUES( %d, '%s', %d);",
                            totalCurrentAssetsDateId,
                            dataTotalCurrentAssets.toLocal8Bit().constData(),
                            mainAnalysisId);
    }


    qDebug() << str;

    qry.prepare(str);


    if(!qry.exec())
    {
        qDebug() << qry.lastError();
        if(m_disableMsgBoxes == false)
        {
            QMessageBox::critical(NULL, QString::fromUtf8("TblDataTotalCurrentAssetsSubAnalysis"), qry.lastError().text().toLatin1().constData());
        }

        if(dbIsHandledExternly == false)
        {
            closeDb();
            m_mutex.unlock();
        }
        return false;
    }


    totalCurrentAssetsDataId = (int) qry.lastInsertId().toInt();


    qry.finish();
    if(dbIsHandledExternly == false)
    {
        closeDb();
        m_mutex.unlock();
    }

    return true;
}







/*****************************************************************
 *
 * Function:		getSubAnalysisTotalCurrentAssetsData()
 *
 * Description:
 *
 *
 *
 *****************************************************************/
bool CDbHndl::
getSubAnalysisTotalCurrentAssetsData(QString stockName,
                                     QString stockSymbol,
                                     TotalCurrentAssetsST *totalCurrentAssetsDataArr,
                                     int &nofTotalCurrentAssetsArrData,
                                     bool dbIsHandledExternly)
{


    QSqlRecord rec;
    QString str;
    TotalCurrentAssetsST data;

    nofTotalCurrentAssetsArrData = 0;


    if(dbIsHandledExternly == false)
    {
        m_mutex.lock();
        openDb(PATH_JACK_STOCK_DB);
    }

    QSqlQuery qry(m_db);

  bool found = false;


    str.sprintf("SELECT TblMainAnalysis.*, TblDateTotalCurrentAssetsSubAnalysis.*, TblDataTotalCurrentAssetsSubAnalysis.* "
                " FROM TblMainAnalysis, TblDateTotalCurrentAssetsSubAnalysis, TblDataTotalCurrentAssetsSubAnalysis  "
                " WHERE  "
                "       TblMainAnalysis.MainAnalysisId = TblDateTotalCurrentAssetsSubAnalysis.MainAnalysisId AND "
                "       TblMainAnalysis.MainAnalysisId = TblDataTotalCurrentAssetsSubAnalysis.MainAnalysisId AND "
                "       TblDateTotalCurrentAssetsSubAnalysis.DateTotalCurrentAssetsId = TblDataTotalCurrentAssetsSubAnalysis.DateTotalCurrentAssetsId AND "
                "       lower(TblMainAnalysis.stockName) = lower('%s') AND "
                "       lower(TblMainAnalysis.StockSymbol) = lower('%s') "
                " ORDER BY (CAST(TblDateTotalCurrentAssetsSubAnalysis.DateTotalCurrentAssets AS REAL)) ASC;",              // DESC";",
                                                                           stockName.toLocal8Bit().constData(),
                                                                           stockSymbol.toLocal8Bit().constData());


    qDebug() << str << "\n";

    qry.prepare(str);


    if( !qry.exec() )
    {
        if(m_disableMsgBoxes == false)
        {
            QMessageBox::critical(NULL, QString::fromUtf8("db error"), qry.lastError().text().toLatin1().constData());
        }
        qDebug() << qry.lastError();
        if(dbIsHandledExternly == false)
        {
            closeDb();
            m_mutex.unlock();
        }
        return false;
    }
    else
    {
        while(qry.next())
        {
            rec = qry.record();

            if((rec.value("DateTotalCurrentAssets").isNull() == true) ||  // Date
                (rec.value("DataTotalCurrentAssets").isNull() == true))   // Data
            {

                if(found == true)
                {
                    continue;
                }
                else
                {
                    qry.finish();
                    if(dbIsHandledExternly == false)
                    {
                        closeDb();
                        m_mutex.unlock();
                    }

                    return false;
                }
            }
            else
            {
                found = true;
                qDebug() << rec.value("DateTotalCurrentAssets").toString() << "\n";
                qDebug() << rec.value("stockSymbol").toString();
                qDebug() << rec.value("stockName").toString();




                // Date
                data.date.clear();
                if(rec.value("DateTotalCurrentAssets").isNull() == false)
                {
                    data.date = rec.value("DateTotalCurrentAssets").toString();
                }

                // Data
                data.totalCurrentAssets.clear();
                if(rec.value("DataTotalCurrentAssets").isNull() == false)
                {
                    data.totalCurrentAssets = rec.value("DataTotalCurrentAssets").toString();
                }

                totalCurrentAssetsDataArr[nofTotalCurrentAssetsArrData] = data;
                nofTotalCurrentAssetsArrData++;

            }
        }
    }


    qry.finish();
    if(dbIsHandledExternly == false)
    {
        closeDb();
        m_mutex.unlock();
    }

    if(found == true)
    {
        return true;
    }

    return false;
}


//=====================================





/****************************************************************
 *
 * Function:    subAnalysisTotalCurrentLiabilitiesDateExists()
 *
 * Description:
 *
 *
 *
 *
 *
 ****************************************************************/
bool CDbHndl::subAnalysisTotalCurrentLiabilitiesDateExists(QString date,
                                     int mainAnalysisId,
                                     int &totalCurrentLiabilitiesDateId)
{
    QSqlRecord rec;
    QString str;

    m_mutex.lock();
    openDb(PATH_JACK_STOCK_DB);
    QSqlQuery qry(m_db);



    QByteArray ba = date.toLocal8Bit();
    const char *c_date = ba.data();



    str.sprintf("SELECT * "
                " FROM  TblDateTotalCurrentLiabilitiesSubAnalysis "
                " WHERE DateTotalCurrentLiabilities = '%s' AND MainAnalysisId = %d;",
                c_date,
                mainAnalysisId);

    qDebug() << str;


    qry.prepare(str);


    if( !qry.exec() )
    {
        if(m_disableMsgBoxes == false)
        {
            QMessageBox::critical(NULL, QString::fromUtf8("TblDateTotalCurrentLiabilitiesSubAnalysis error 1"), qry.lastError().text().toUtf8().constData());
        }
        qDebug() << qry.lastError();
        closeDb();
        m_mutex.unlock();
        return false;
    }
    else
    {
        if(qry.next())
        {
            rec = qry.record();



            if((rec.value("DateTotalCurrentLiabilities").isNull() == true) ||
               (true == rec.value("MainAnalysisId").isNull()))
            {
                qry.finish();
                closeDb();
                m_mutex.unlock();
                return false;
            }
            else
            {
                totalCurrentLiabilitiesDateId = rec.value("DateTotalCurrentLiabilitiesId").toInt();
                qry.finish();
                closeDb();
                m_mutex.unlock();
                return true;
            }
        }
    }

    qry.finish();
    closeDb();
    m_mutex.unlock();
    return false;
}





/****************************************************************
 *
 * Function:    insertSubAnalysisTotalCurrentLiabilitiesDate()
 *
 * Description:
 *
 *
 *
 *
 *
 ****************************************************************/
bool CDbHndl::
insertSubAnalysisTotalCurrentLiabilitiesDate(QString date,
                                             int mainAnalysisId,
                                             int &dateTotalCurrentLiabilitiesId,
                                             bool dbIsHandledExternly)
{
    QString str;

    if(dbIsHandledExternly == false)
    {
        m_mutex.lock();
        openDb(PATH_JACK_STOCK_DB);
    }

    QSqlQuery qry(m_db);

    QByteArray ba = date.toLocal8Bit();
    const char *c_date = ba.data();


    str.sprintf("INSERT OR REPLACE INTO TblDateTotalCurrentLiabilitiesSubAnalysis "
                "(DateTotalCurrentLiabilities, MainAnalysisId) "
                " VALUES('%s', %d);",
                c_date,
                mainAnalysisId);

    qDebug() << str;

    qry.prepare(str);

    if(!qry.exec())
    {
        qDebug() << qry.lastError();

        if(m_disableMsgBoxes == false)
        {
            QMessageBox::critical(NULL, QString::fromUtf8("TblDateTotalCurrentLiabilitiesSubAnalysis"), qry.lastError().text().toUtf8().constData());
        }

        if(dbIsHandledExternly == false)
        {
            closeDb();
            m_mutex.unlock();
        }
        return false;
    }


    dateTotalCurrentLiabilitiesId = (int) qry.lastInsertId().toInt();

    qry.finish();

    if(dbIsHandledExternly==false)
    {
        closeDb();
        m_mutex.unlock();
    }

    return true;
}






/*****************************************************************
 *
 * Function:		getSubAnalysisTotalCurrentLiabilitiesDataId()
 *
 * Description:
 *
 *
 *
 *****************************************************************/
bool CDbHndl::
getSubAnalysisTotalCurrentLiabilitiesDataId(int mainAnalysisId,
                  int totalCurrentLiabilitiesDateId,
                  int &totalCurrentLiabilitiesDataId,
                  bool dbIsHandledExternly)
{


    QSqlRecord rec;
    QString str;

    if(dbIsHandledExternly == false)
    {
        m_mutex.lock();
        openDb(PATH_JACK_STOCK_DB);
    }

    QSqlQuery qry(m_db);

  bool found = false;


    str.sprintf("SELECT TblDataTotalCurrentLiabilitiesSubAnalysis.DataTotalCurrentLiabilitiesId, TblDataTotalCurrentLiabilitiesSubAnalysis.DateTotalCurrentLiabilitiesId, TblDataTotalCurrentLiabilitiesSubAnalysis.MainAnalysisId   "
                " FROM TblDataTotalCurrentLiabilitiesSubAnalysis   "
                " WHERE  "
                "       TblDataTotalCurrentLiabilitiesSubAnalysis.DateTotalCurrentLiabilitiesId = %d AND "
                "       TblDataTotalCurrentLiabilitiesSubAnalysis.MainAnalysisId = %d;",
                                                                           totalCurrentLiabilitiesDateId,
                                                                           mainAnalysisId);


    qDebug() << str << "\n";

    qry.prepare(str);


    if( !qry.exec() )
    {
        if(m_disableMsgBoxes == false)
        {
            QMessageBox::critical(NULL, QString::fromUtf8("db error"), qry.lastError().text().toUtf8().constData());
        }
        qDebug() << qry.lastError();
        if(dbIsHandledExternly == false)
        {
            closeDb();
            m_mutex.unlock();
        }
        return false;
    }
    else
    {
        while(qry.next())
        {
            rec = qry.record();




            if(rec.value("DataTotalCurrentLiabilitiesId").isNull() == true)
            {

                if(found == true)
                {
                    continue;
                }
                else
                {
                    qry.finish();
                    if(dbIsHandledExternly == false)
                    {
                        closeDb();
                        m_mutex.unlock();
                    }

                    return false;
                }
            }
            else
            {
                found = true;
                qDebug() << rec.value("DataTotalCurrentLiabilitiesId").toString();
                qDebug() << rec.value("MainAnalysisId").toString();


                if(rec.value("DataTotalCurrentLiabilitiesId").isNull() == false)
                {
                    totalCurrentLiabilitiesDataId = rec.value("DataTotalCurrentLiabilitiesId").toInt();
                }

            }
        }
    }


    qry.finish();
    if(dbIsHandledExternly == false)
    {
        closeDb();
        m_mutex.unlock();
    }

    if(found == true)
    {
        return true;
    }

    return false;
}









/****************************************************************
 *
 * Function:    insertSubAnalysisTotalCurrentLiabilities()
 *
 * Description:
 *
 *
 *
 *
 *
 ****************************************************************/
bool CDbHndl::
insertSubAnalysisTotalCurrentLiabilities(int dateId,
                                         int mainAnalysisId,
                                         int inputDataId,
                                         bool dataIdIsValid,
                                         QString data,
                                         int &dataId,
                                         bool dbIsHandledExternly)
{
    QString str;

    if(dbIsHandledExternly == false)
    {
        m_mutex.lock();
        openDb(PATH_JACK_STOCK_DB);
    }


    QSqlQuery qry(m_db);


    if(dataIdIsValid == true)
    {
        str.sprintf("INSERT OR REPLACE INTO TblDataTotalCurrentLiabilitiesSubAnalysis "
                " (DateTotalCurrentLiabilitiesId, "
                "  DataTotalCurrentLiabilities, "
                 " DataTotalCurrentLiabilitiesId,"
                 " MainAnalysisId) "
                 " VALUES( %d,  '%s', %d, %d);",
                        dateId,
                        data.toLocal8Bit().constData(),
                        inputDataId,
                        mainAnalysisId);
    }
    // New data
    else
    {
        str.sprintf("INSERT INTO TblDataTotalCurrentLiabilitiesSubAnalysis "
                    " (DateTotalCurrentLiabilitiesId, "
                    " DataTotalCurrentLiabilities, "
                    " MainAnalysisId) "
                    " VALUES( %d, '%s', %d);",
                            dateId,
                            data.toLocal8Bit().constData(),
                            mainAnalysisId);
    }


    qDebug() << str;

    qry.prepare(str);


    if(!qry.exec())
    {
        qDebug() << qry.lastError();
        if(m_disableMsgBoxes == false)
        {
            QMessageBox::critical(NULL, QString::fromUtf8("TblDataTotalCurrentLiabilitiesSubAnalysis"), qry.lastError().text().toLatin1().constData());
        }

        if(dbIsHandledExternly == false)
        {
            closeDb();
            m_mutex.unlock();
        }
        return false;
    }


    dataId = (int) qry.lastInsertId().toInt();


    qry.finish();
    if(dbIsHandledExternly == false)
    {
        closeDb();
        m_mutex.unlock();
    }

    return true;
}








/*****************************************************************
 *
 * Function:		getSubAnalysisTotalCurrentLiabilitiesData()
 *
 * Description:
 *
 *
 *
 *****************************************************************/
bool CDbHndl::
getSubAnalysisTotalCurrentLiabilitiesData(QString stockName,
                           QString stockSymbol,
                           TotalCurrentLiabilitiesST *dataArr,
                           int &nofArrData,
                           bool dbIsHandledExternly)
{


    QSqlRecord rec;
    QString str;
    TotalCurrentLiabilitiesST data;

    nofArrData = 0;


    if(dbIsHandledExternly == false)
    {
        m_mutex.lock();
        openDb(PATH_JACK_STOCK_DB);
    }

    QSqlQuery qry(m_db);

  bool found = false;


    str.sprintf("SELECT TblMainAnalysis.*, TblDateTotalCurrentLiabilitiesSubAnalysis.*, TblDataTotalCurrentLiabilitiesSubAnalysis.* "
                " FROM TblMainAnalysis, TblDateTotalCurrentLiabilitiesSubAnalysis, TblDataTotalCurrentLiabilitiesSubAnalysis  "
                " WHERE  "
                "       TblMainAnalysis.MainAnalysisId = TblDateTotalCurrentLiabilitiesSubAnalysis.MainAnalysisId AND "
                "       TblMainAnalysis.MainAnalysisId = TblDataTotalCurrentLiabilitiesSubAnalysis.MainAnalysisId AND "
                "       TblDateTotalCurrentLiabilitiesSubAnalysis.DateTotalCurrentLiabilitiesId = TblDataTotalCurrentLiabilitiesSubAnalysis.DateTotalCurrentLiabilitiesId AND "
                "       lower(TblMainAnalysis.stockName) = lower('%s') AND "
                "       lower(TblMainAnalysis.StockSymbol) = lower('%s') "
                " ORDER BY (CAST(TblDateTotalCurrentLiabilitiesSubAnalysis.DateTotalCurrentLiabilities AS REAL)) ASC;",              // DESC";",
                                                                           stockName.toLocal8Bit().constData(),
                                                                           stockSymbol.toLocal8Bit().constData());


    qDebug() << str << "\n";

    qry.prepare(str);


    if( !qry.exec() )
    {
        if(m_disableMsgBoxes == false)
        {
            QMessageBox::critical(NULL, QString::fromUtf8("db error"), qry.lastError().text().toLatin1().constData());
        }
        qDebug() << qry.lastError();
        if(dbIsHandledExternly == false)
        {
            closeDb();
            m_mutex.unlock();
        }
        return false;
    }
    else
    {
        while(qry.next())
        {
            rec = qry.record();

            if((rec.value("DateTotalCurrentLiabilities").isNull() == true) ||  // Date
                (rec.value("DataTotalCurrentLiabilities").isNull() == true))   // Data
            {

                if(found == true)
                {
                    continue;
                }
                else
                {
                    qry.finish();
                    if(dbIsHandledExternly == false)
                    {
                        closeDb();
                        m_mutex.unlock();
                    }

                    return false;
                }
            }
            else
            {
                found = true;
                qDebug() << rec.value("DateTotalCurrentLiabilities").toString() << "\n";
                qDebug() << rec.value("stockSymbol").toString();
                qDebug() << rec.value("stockName").toString();




                // Date
                data.date.clear();
                if(rec.value("DateTotalCurrentLiabilities").isNull() == false)
                {
                    data.date = rec.value("DateTotalCurrentLiabilities").toString();
                }

                // Data
                data.totalCurrentLiabilities.clear();
                if(rec.value("DataTotalCurrentLiabilities").isNull() == false)
                {
                    data.totalCurrentLiabilities = rec.value("DataTotalCurrentLiabilities").toString();
                }

                dataArr[nofArrData] = data;
                nofArrData++;

            }
        }
    }


    qry.finish();
    if(dbIsHandledExternly == false)
    {
        closeDb();
        m_mutex.unlock();
    }

    if(found == true)
    {
        return true;
    }

    return false;
}







//=====================

//===================

#if 0






// 5
        //-----------------------------------------------------------------------
        // TblDateTotalLiabilitiesSubAnalysis (Totala skulder)
        //-----------------------------------------------------------------------
        tmp.sprintf("CREATE TABLE IF NOT EXISTS TblDateTotalLiabilitiesSubAnalysis "
                    " (DateTotalLiabilitiesId INTEGER PRIMARY KEY AUTOINCREMENT, "
                    " DateTotalLiabilities DATE, "
                    " MainAnalysisId INTEGER);");

        qDebug() << tmp;


        qry.prepare(tmp);

        res = execSingleCmd(qry);

        if(res == false)
        {
            qDebug() << qry.lastError();
            if(m_disableMsgBoxes == false)
            {
                QMessageBox::critical(NULL, QString::fromUtf8("TblDateAnalysis"), QString::fromUtf8("Fail create TblDateTotalLiabilitiesSubAnalysis"));
            }
            closeDb();
            m_mutex.unlock();
            return false;
        }

        qry.finish();


// Data
        //-----------------------------------------------------------------------
        // TblDataTotalLiabilitiesSubAnalysis (Totala skulder)
        //-----------------------------------------------------------------------
        tmp.sprintf("CREATE TABLE IF NOT EXISTS TblDataTotalLiabilitiesSubAnalysis "
                    " (DataTotalLiabilitiesId INTEGER PRIMARY KEY AUTOINCREMENT, "
                    " DateTotalLiabilitiesId INTEGER, "
                    " DataTotalLiabilities VARCHAR(255), "
                    " MainAnalysisId INTEGER);");

        qDebug() << tmp;


        qry.prepare(tmp);

        res = execSingleCmd(qry);

        if(res == false)
        {
            qDebug() << qry.lastError();
            if(m_disableMsgBoxes == false)
            {
                QMessageBox::critical(NULL, QString::fromUtf8("TblDateAnalysis"), QString::fromUtf8("Fail create TblDataTotalLiabilitiesSubAnalysis"));
            }
            closeDb();
            m_mutex.unlock();
            return false;
        }

        qry.finish();




// 6
// Date
        //-----------------------------------------------------------------------
        // TblDateSoliditySubAnalysis (soliditet)
        //-----------------------------------------------------------------------
        tmp.sprintf("CREATE TABLE IF NOT EXISTS TblDateSoliditySubAnalysis "
                    " (DateSolidityId INTEGER PRIMARY KEY AUTOINCREMENT, "
                    " DateSolidity DATE, "
                    " MainAnalysisId INTEGER);");

        qDebug() << tmp;


        qry.prepare(tmp);

        res = execSingleCmd(qry);

        if(res == false)
        {
            qDebug() << qry.lastError();
            if(m_disableMsgBoxes == false)
            {
                QMessageBox::critical(NULL, QString::fromUtf8("TblDateAnalysis"), QString::fromUtf8("Fail create TblDateSoliditySubAnalysis"));
            }
            closeDb();
            m_mutex.unlock();
            return false;
        }

        qry.finish();




// Data
        //-----------------------------------------------------------------------
        // TblDataSoliditySubAnalysis (soliditet)
        //-----------------------------------------------------------------------
        tmp.sprintf("CREATE TABLE IF NOT EXISTS TblDataSoliditySubAnalysis "
                    " (DataSolidityId INTEGER PRIMARY KEY AUTOINCREMENT, "
                    " DateSolidityId INTEGER, "
                    " DataSolidity VARCHAR(255), "
                    " MainAnalysisId INTEGER);");

        qDebug() << tmp;


        qry.prepare(tmp);

        res = execSingleCmd(qry);

        if(res == false)
        {
            qDebug() << qry.lastError();
            if(m_disableMsgBoxes == false)
            {
                QMessageBox::critical(NULL, QString::fromUtf8("TblDateAnalysis"), QString::fromUtf8("Fail create TblDataSoliditySubAnalysis"));
            }
            closeDb();
            m_mutex.unlock();
            return false;
        }

        qry.finish();



// 7

//Date
        //-----------------------------------------------------------------------
        // TblDateCoverageRatioSubAnalysis (räntetäckningsgraden)
        //-----------------------------------------------------------------------
        tmp.sprintf("CREATE TABLE IF NOT EXISTS TblDateCoverageRatioSubAnalysis "
                    " (CoverageRatioDateId INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "  CoverageRatioDate DATE, "
                    "  MainAnalysisId INTEGER);");

        qDebug() << tmp;


        qry.prepare(tmp);

        res = execSingleCmd(qry);

        if(res == false)
        {
            qDebug() << qry.lastError();
            if(m_disableMsgBoxes == false)
            {
                QMessageBox::critical(NULL, QString::fromUtf8("TblDateAnalysis"), QString::fromUtf8("Fail create TblDateCoverageRatioSubAnalysis"));
            }
            closeDb();
            m_mutex.unlock();
            return false;
        }

        qry.finish();


// Data
                //-----------------------------------------------------------------------
                // TblDataCoverageRatioSubAnalysis (räntetäckningsgraden)
                //-----------------------------------------------------------------------
                tmp.sprintf("CREATE TABLE IF NOT EXISTS TblDataCoverageRatioSubAnalysis "
                            " (CoverageRatioDataId INTEGER PRIMARY KEY AUTOINCREMENT, "
                            "  CoverageRatioDateId INTEGER, "
                            "  CoverageRatioData VARCHAR(255), "
                            "  MainAnalysisId INTEGER);");

                qDebug() << tmp;


                qry.prepare(tmp);

                res = execSingleCmd(qry);

                if(res == false)
                {
                    qDebug() << qry.lastError();
                    if(m_disableMsgBoxes == false)
                    {
                        QMessageBox::critical(NULL, QString::fromUtf8("TblDateAnalysis"), QString::fromUtf8("Fail create TblDataCoverageRatioSubAnalysis"));
                    }
                    closeDb();
                    m_mutex.unlock();
                    return false;
                }

                qry.finish();



// 8
// Date

        //-----------------------------------------------------------------------
        // TblDateCoreCapitalRatioSubAnalysis (kärnprimärkapitalrelation används ej)
        //-----------------------------------------------------------------------
        tmp.sprintf("CREATE TABLE IF NOT EXISTS TblDateCoreCapitalRatioSubAnalysis "
                    " (DateCoreCapitalRatioId INTEGER PRIMARY KEY AUTOINCREMENT, "
                    " DateCoreCapitalRatio DATE, "
                    " MainAnalysisId INTEGER);");

        qDebug() << tmp;


        qry.prepare(tmp);

        res = execSingleCmd(qry);

        if(res == false)
        {
            qDebug() << qry.lastError();
            if(m_disableMsgBoxes == false)
            {
                QMessageBox::critical(NULL, QString::fromUtf8("TblDateAnalysis"), QString::fromUtf8("Fail create TblDateCoreCapitalRatioSubAnalysis"));
            }
            closeDb();
            m_mutex.unlock();
            return false;
        }

        qry.finish();


// Data
        //-----------------------------------------------------------------------
        // TblDataCoreCapitalRatioSubAnalysis (kärnprimärkapitalrelation används ej)
        //-----------------------------------------------------------------------
        tmp.sprintf("CREATE TABLE IF NOT EXISTS TblDataCoreCapitalRatioSubAnalysis "
                    " (DataCoreCapitalRatioId INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "  DateCoreCapitalRatioId INTEGER, "
                    "  DataCoreCapitalRatio VARCHAR(255), "
                    "  MainAnalysisId INTEGER);");

        qDebug() << tmp;


        qry.prepare(tmp);

        res = execSingleCmd(qry);

        if(res == false)
        {
            qDebug() << qry.lastError();
            if(m_disableMsgBoxes == false)
            {
                QMessageBox::critical(NULL, QString::fromUtf8("TblDateAnalysis"), QString::fromUtf8("Fail create TblDataCoreCapitalRatioSubAnalysis"));
            }
            closeDb();
            m_mutex.unlock();
            return false;
        }

        qry.finish();


// 9

// Date
        //-----------------------------------------------------------------------
        // TblDateCoreTier1RatioSubAnalysis (primärkapitalrelation, Lundaluppen)
        //-----------------------------------------------------------------------
        tmp.sprintf("CREATE TABLE IF NOT EXISTS TblDateTier1RatioSubAnalysis "
                    " (DateTier1RatioId INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "  DateTier1Ratio DATE, "
                    "  MainAnalysisId INTEGER);");

        qDebug() << tmp;


        qry.prepare(tmp);

        res = execSingleCmd(qry);

        if(res == false)
        {
            qDebug() << qry.lastError();
            if(m_disableMsgBoxes == false)
            {
                QMessageBox::critical(NULL, QString::fromUtf8("TblDateAnalysis"), QString::fromUtf8("Fail create TblDateTier1RatioSubAnalysis"));
            }
            closeDb();
            m_mutex.unlock();
            return false;
        }

        qry.finish();

// Data

        //-----------------------------------------------------------------------
        // TblDataCoreTier1RatioSubAnalysis (primärkapitalrelation, Lundaluppen)
        //-----------------------------------------------------------------------
        tmp.sprintf("CREATE TABLE IF NOT EXISTS TblDataTier1RatioSubAnalysis "
                    " (DataTier1RatioId INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "  DateTier1RatioId INTEGER, "                             // YYYY-MM-DD ID
                    "  DataTier1Ratio VARCHAR(255), "
                    "  MainAnalysisId INTEGER);");

        qDebug() << tmp;


        qry.prepare(tmp);

        res = execSingleCmd(qry);

        if(res == false)
        {
            qDebug() << qry.lastError();
            if(m_disableMsgBoxes == false)
            {
                QMessageBox::critical(NULL, QString::fromUtf8("TblDateAnalysis"), QString::fromUtf8("Fail create TblDataTier1RatioSubAnalysis"));
            }
            closeDb();
            m_mutex.unlock();
            return false;
        }

        qry.finish();

#endif

//===================

#if 0










#endif
