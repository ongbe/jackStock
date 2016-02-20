#include "../../inc/guiUtil/guiFinanceColor.h"
#include "util.h"
#include <QColor>




/**********************************************************************************
 *
 * Function:        GuiFinanceColor()
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
 * Function:    setTxtColorEarningsDivDividend()
 *
 *
 * Description: Vinst/Utdelning
 *
 *
 *
 ********************************************************************************/
void GuiFinanceColor::
setTxtColorEarningsDivDividend(QString inValue, QLineEdit *outValue)
{
    CUtil cu;
    QColor color;
    double inValueDouble;


    if(false == cu.number2double(inValue, inValueDouble))
    {
        color = Qt::red;
        MyLineEdit::setTxtColor(outValue, m_faDataPalette[FA_ERNING_DIV_DIVIDEN], color);

        return;
    }

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
    double inValueDouble;


    if(false == cu.number2double(inValue, inValueDouble))
    {
        color = Qt::red;
        MyLineEdit::setTxtColor(outValue, m_faDataPalette[FA_NAV_DIV_LAST_PRICE], color);

        return;
    }

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
 * Function:    setTxtColorPe()
 *
 *
 * Description:
 *
 *
 *
 ********************************************************************************/
void GuiFinanceColor::
setTxtColorPe(QString inValue, QLineEdit *outValue, QString assetType)
{
    CUtil cu;
    QColor color;
    double inValueDouble;


    if(false == cu.number2double(inValue, inValueDouble))
    {
        color = Qt::red;
        MyLineEdit::setTxtColor(outValue, m_faDataPalette[FA_PE], color);

        return;
    }

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

    // Disable P/E when company type is finance and real estate
    if(assetType.compare("Finans och fastighet")== 0)
    {
        outValue->setDisabled(true);
    }
    else
    {
        outValue->setDisabled(false);
    }

    MyLineEdit::setTxtColor(outValue, m_faDataPalette[FA_PE], color);

}


/**********************************************************************************
 *
 * Function:    setTxtColorPs()
 *
 *
 * Description:
 *
 *
 *
 ********************************************************************************/
void GuiFinanceColor::
setTxtColorPs(QString inValue, QLineEdit *outValue)
{
    CUtil cu;
    QColor color;
    double inValueDouble;


    if(false == cu.number2double(inValue, inValueDouble))
    {
        color = Qt::red;
        MyLineEdit::setTxtColor(outValue, m_faDataPalette[FA_PS], color);

        return;
    }


    if(inValue.toDouble() <= 0.75)
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::GREEN);
    }
    else if((inValue.toDouble() > 0.75) && (inValue.toDouble() <= 1.5))
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::YELLOW_3);
    }
    else if((inValue.toDouble() > 1.5) && (inValue.toDouble() <= 3.0))
    {
        color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::ORANGE);
    }
    else
    {
        color = Qt::red;
    }

    MyLineEdit::setTxtColor(outValue, m_faDataPalette[FA_PS], color);

}


/**********************************************************************************
 *
 * Function:    setTxtColorYield()
 *
 *
 * Description:
 *
 *
 *
 ********************************************************************************/
void GuiFinanceColor::
setTxtColorYield(QString yield, QString earningsDivDividend, QLineEdit *outValue)
{
    CUtil cu;
    QColor color;
    double doubleEarningsDivDividend;
    double doubleYield;



    if(false == cu.number2double(yield, doubleYield))
    {
        color = Qt::red;
        MyLineEdit::setTxtColor(outValue, m_faDataPalette[FA_YIELD], color);
        return;
    }


    if(false == cu.number2double(earningsDivDividend, doubleEarningsDivDividend))
    {
        if((yield.toDouble() >= 3) && (yield.toDouble() < 4))
        {
            color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::ORANGE);
        }
        else if((yield.toDouble() >= 4) && (yield.toDouble() <= 6))
        {
            color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::GREEN);
        }
        else if((yield.toDouble() > 6) && (yield.toDouble() < 7))
        {
            color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::ORANGE);
        }
        else if((yield.toDouble() > 7) && (yield.toDouble() < 10))
        {
            color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::YELLOW_3);
        }
        else if(yield.toDouble() > 10)
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
        if((doubleEarningsDivDividend > 1) && (doubleEarningsDivDividend < 2))
        {
            if((yield.toDouble() >= 3) && (yield.toDouble() < 4))
            {
                color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::YELLOW_3);
            }
            else if(yield.toDouble() >= 4 )
            {
                color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::ORANGE);
            }
            else
            {
                color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::YELLOW_3);
            }
        }
        else if(doubleEarningsDivDividend >= 2)
        {
            if((yield.toDouble() >= 3) && (yield.toDouble() < 4))
            {
                color = cu.getQColor((CUtil::ColorRgb_ET) CUtil::ORANGE);
            }
            else if(yield.toDouble() >= 4)
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

    MyLineEdit::setTxtColor(outValue, m_faDataPalette[FA_YIELD], color);

}


