#include "../../inc/guiUtil/guiFinanceColor.h"
#include "util.h"
#include <QColor>



/**********************************************************************************
 *
 * Function:    ()
 *
 *
 * Description:
 *
 *
 *
 ********************************************************************************/
GuiFinanceColor::GuiFinanceColor()
{
    int i;

    for(i = 0; i < FA_NOF_DATA; i++)
    {
        m_faDataPalette[i] = new QPalette();
    }
}


/**********************************************************************************
 *
 * Function:    ()
 *
 *
 * Description:
 *
 *
 *
 ********************************************************************************/
GuiFinanceColor::~GuiFinanceColor()
{
    int i;

    for(i = 0; i < FA_NOF_DATA; i++)
    {
        delete m_faDataPalette[i];
    }
}


/**********************************************************************************
 *
 * Function:    SetTxtColorEarningsDivDividend()
 *
 *
 * Description: Vinst/Utdelning
 *
 *
 *
 ********************************************************************************/
void GuiFinanceColor::
SetTxtColorEarningsDivDividend(QString inValue, QLineEdit *outValue)
{
    CUtil cu;
    QColor color;

    // Vinst/Utdelning
    if((inValue.toDouble() >= 1) && (inValue.toDouble() < 2))
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::ORANGE);
    }
    else if(inValue.toDouble() >= 2)
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::GREEN);
    }
    else
    {
        color = Qt::red;
    }

    MyLineEdit::setTxtColor(outValue, m_faDataPalette[FA_ERNING_DIV_DIVIDEN], color);

}



/**********************************************************************************
 *
 * Function:    setTxtColorNavDivStockPrice()
 *
 *
 * Description: Substans/Aktiepris
 *
 *
 *
 ********************************************************************************/
void GuiFinanceColor::
setTxtColorNavDivStockPrice(QString inValue, QLineEdit *outValue)
{
    CUtil cu;
    QColor color;

    // Substans/Aktiepris
    if((inValue.toDouble() >= 0.7) && (inValue.toDouble() < 1))
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::YELLOW_3);
    }
    else if((inValue.toDouble() >= 1) && (inValue.toDouble() < 2))
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::ORANGE);
    }
    else if(inValue.toDouble() >= 2)
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::GREEN);
    }
    else
    {
        color = Qt::red;
    }

    MyLineEdit::setTxtColor(outValue, m_faDataPalette[FA_NAV_DIV_LAST_PRICE], color);

}




/**********************************************************************************
 *
 * Function:    setTxtColorNavDivStockPrice()
 *
 *
 * Description: Substans/Aktiepris
 *
 *
 *
 ********************************************************************************/
void GuiFinanceColor::
setTxtColorPe(QString inValue, QLineEdit *outValue)
{
    CUtil cu;
    QColor color;

    // PE Vaue
    if(inValue.toDouble() < 5)
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::ORANGE);
    }
    else if((inValue.toDouble() >= 5) && (inValue.toDouble() <= 15))
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::GREEN);
    }
    else if((inValue.toDouble() > 15) && (inValue.toDouble() <= 18))
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::YELLOW_3);
    }
    else if((inValue.toDouble() > 18) && (inValue.toDouble() <= 25))
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::ORANGE);
    }
    else
    {
        color = Qt::red;
    }


    MyLineEdit::setTxtColor(outValue, m_faDataPalette[FA_PE], color);

}



#if 0
/**********************************************************************************
 *
 * Function:    clear()
 *
 *
 * Description:
 *
 *
 *
 ********************************************************************************/
GuiFinanceColor::clear(QLineEdit *earningsDivByDividend,  // Vinst/Utdelning
                           QLineEdit *navDivStockPrice,       // Substans/Aktiepris
                           QLineEdit *pe,
                           QLineEdit *ps,
                           QLineEdit *yield)                 // Direct avkastning
{
    earningsDivByDividend->clear();
    navDivStockPrice->clear();
    pe->clear();
    ps->clear();
    yield->clear();
}



GuiFinanceColor::setValue(CDbHndl::snapshotStockData_ST keyData,    // Input data
                              QLineEdit *earningsDivByDividend,         // Vinst/Utdelning
                              QLineEdit *navDivStockPrice,              // Substans/Aktiepris
                              QLineEdit *pe,
                              QLineEdit *ps,
                              QLineEdit *yield,                    // Direct avkastning
                              QString assetType)
{
    CUtil cu;
    QColor color;

    // Vinst/Utdelning
    if(keyData.earningsDividedByDividend.toDouble() >= 1 && keyData.earningsDividedByDividend.toDouble() < 2)
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::ORANGE);
    }
    else if(keyData.earningsDividedByDividend.toDouble() >= 2)
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::GREEN);
    }
    else
    {
        color = Qt::red;
    }

    setFaEditControlTxtColor(earningsDivByDividend, m_faDataPalette[FA_ERNING_DIV_DIVIDEN], color);



    // Substans/Aktiepris
    if(keyData.navDivLastStockPrice.toDouble() >= 0.7 && keyData.navDivLastStockPrice.toDouble() < 1)
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::YELLOW_3);
    }
    else if(keyData.navDivLastStockPrice.toDouble() >= 1 && keyData.navDivLastStockPrice.toDouble() < 2)
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::ORANGE);
    }
    else if(keyData.navDivLastStockPrice.toDouble() >= 2)
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::GREEN);
    }
    else
    {
        color = Qt::red;
    }

    setFaEditControlTxtColor(navDivStockPrice, m_faDataPalette[FA_NAV_DIV_LAST_PRICE], color);


    // PE Vaue
    if(keyData.keyValuePE.toDouble() < 5)
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::ORANGE);
    }
    else if(keyData.keyValuePE.toDouble() >= 5 && keyData.keyValuePE.toDouble() <= 15)
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::GREEN);
    }
    else if(keyData.keyValuePE.toDouble() > 15 && keyData.keyValuePE.toDouble() <= 18)
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::YELLOW_3);
    }
    else if(keyData.keyValuePE.toDouble() > 18 && keyData.keyValuePE.toDouble() <= 25)
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::ORANGE);
    }
    else
    {
        color = Qt::red;
    }


    // Disable P/E when company type is finance and real estate
    if(assetType.compare("Finans och fastighet")== 0)
    {
        pe->setDisabled(true);
    }
    else
    {
        pe->setDisabled(false);
    }

    setFaEditControlTxtColor(ui->lineEditPELSqrt, m_faDataPalette[FA_PE], color);


    if(keyData.keyValuePS.toDouble() <= 0.75)
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::GREEN);
    }
    else if(keyData.keyValuePS.toDouble() > 0.75 && keyData.keyValuePS.toDouble() <= 1.5)
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::YELLOW_3);
    }
    else if(keyData.keyValuePS.toDouble() > 1.5 && keyData.keyValuePS.toDouble() <= 3.0)
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::ORANGE);
    }
    else
    {
        color = Qt::red;
    }

    setFaEditControlTxtColor(ps, m_faDataPalette[FA_YIELD], color);



    double doubleNumber;
    if(false == cu.number2double(keyData.earningsDividedByDividend, doubleNumber))
    {
        if(keyData.keyValueYield.toDouble() >= 3 && keyData.keyValueYield.toDouble() < 4)
        {
            color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::ORANGE);
        }
        else if(keyData.keyValueYield.toDouble() >= 4 && keyData.keyValueYield.toDouble() <= 6)
        {
            color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::GREEN);
        }
        else if(keyData.keyValueYield.toDouble() > 6 && keyData.keyValueYield.toDouble() < 7)
        {
            color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::ORANGE);
        }
        else if(keyData.keyValueYield.toDouble() > 7 && keyData.keyValueYield.toDouble() < 10)
        {
            color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::YELLOW_3);
        }
        else if(keyData.keyValueYield.toDouble() > 10)
        {
            color = Qt::red;
        }
        else
        {
            color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::YELLOW_3);
        }
    }
    else
    {
        //  Grön Om vinst/Utdelning är >= 2 && direktavkastningn = 4 - 6 %
        if(keyData.earningsDividedByDividend.toDouble() > 1 && keyData.earningsDividedByDividend.toDouble() < 2)
        {
            if(keyData.keyValueYield.toDouble() >= 3 && keyData.keyValueYield.toDouble() < 4)
            {
                color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::YELLOW_3);
            }
            else if(keyData.keyValueYield.toDouble() >= 4 )
            {
                color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::ORANGE);
            }
            else
            {
                color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::YELLOW_3);
            }
        }
        else if(keyData.earningsDividedByDividend.toDouble() >= 2)
        {
            if(keyData.keyValueYield.toDouble() >= 3 && keyData.keyValueYield.toDouble() < 4)
            {
                color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::ORANGE);
            }
            else if(keyData.keyValueYield.toDouble() >= 4)
            {
                color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::GREEN);
            }
            else
            {
                color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::YELLOW_3);
            }
        }
        else
        {
            color = Qt::red;
        }
    }



    setFaEditControlTxtColor(yield, m_faDataPalette[FA_YIELD], color);

}

#endif
















