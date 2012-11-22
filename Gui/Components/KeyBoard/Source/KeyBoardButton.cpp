/****************************************************************************/
/*! \file KeyBoardButton.cpp
 *
 *  \brief Implementation of custom buttons for the keyboard.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-10-19
 *   $Author:  $ N.Kamath
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/
#include "KeyBoard/Include/KeyBoardButton.h"
#include "KeyBoard/Include/KeyBoard.h"
#include "Application/Include/LeicaStyle.h"
#include <QDebug>

namespace KeyBoard {

//Defaut values are set for SIZE_1;
qint32 CKeyBoardButton::m_ButtonWidth = 70;
qint32 CKeyBoardButton::m_ButtonHeight = 62;
qint32 CKeyBoardButton::m_NumericTextX = 30;
qint32 CKeyBoardButton::m_NumericTextY = 40;
qint32 CKeyBoardButton::m_AlphabetTextX = 14;
qint32 CKeyBoardButton::m_AlphabetTextY = 44;
qint32 CKeyBoardButton::m_SpecialCharX = 40;
qint32 CKeyBoardButton::m_SpecialCharY = 25;
qint32 CKeyBoardButton::m_SubSpellerTextX = 25;
qint32 CKeyBoardButton::m_FontSize = 15;
QString CKeyBoardButton::m_IconSize = "Large";

/*****************************************************s***********************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 *  \iparam IconSize Large/small
 *  \iparam IconType Left/Center/Right
 *  \iparam BtnText1 Botton left text on button
 *  \iparam BtnText2 Button left text on button
 *  \iparam IconPresent true -icon is present
 */
/****************************************************************************/
CKeyBoardButton::CKeyBoardButton(QWidget *p_Parent,QString IconType,QString BtnText1,QString BtnText2,
    bool IconPresent,qint32 ButtonType):QToolButton(p_Parent),mp_Parent(p_Parent)
{
   setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
   setMinimumHeight(m_ButtonHeight);
   setFixedWidth(m_ButtonWidth);
   m_IconType = IconType;
   m_BtnText1 = BtnText1;
   m_BtnText2 = BtnText2;
   m_IconPresent = IconPresent;
   m_ButtonType = static_cast<ButtonType_t> (ButtonType);
   m_Clicked = false;
   m_Alpabet = true;
   m_Pressed = false;
   m_PressAndHold = false;
   m_SubSpeller = false;
   m_Toggled = false;
   m_Size = (m_IconSize == "Small") ? 0x80: 0;
   m_ButtonTextList << "Delete"<< ""<<"#...@"<< "OK"<< "ESC"<< "Shift"<< "ShiftLock"
                    <<"CursorLeft"<<"CursorRight";
   m_ButtonFont = font();
   m_ButtonFont.setPointSize(m_FontSize);
   mp_Timer = 0;
   mp_Timer = new QTimer(this);
   CONNECTSIGNALSLOT(mp_Timer, timeout(), this, TimeOut());
   mp_ColorWhite = new QColor(Qt::white);
   mp_ColorGrey = new QColor("#c0c0c0");
   m_PixmapPressed.load(QString(":/%1/SpellerButtons/SpellerButton_%2-Pressed.png").arg(m_IconSize).arg(m_IconType));
   m_PixmapEnabled.load(QString(":/%1/SpellerButtons/SpellerButton_%2-Enabled.png").arg(m_IconSize).arg(m_IconType));
   m_PixmapSelectedPressed.load(QString(":/%1/SpellerButtons/SpellerButton_Left-SelectedPressed.png").arg(m_IconSize));
   m_PixmapSelectedEnabled.load(QString(":/%1/SpellerButtons/SpellerButton_Left-Selected.png").arg(m_IconSize));
   m_PixmapTurnRed.load(QString(":/%1/SpellerButtons/SpellerButton_%2-SelectedPressed.png").arg(m_IconSize).arg(m_IconType));
   m_PixmapDefault.load(QString(":/%1/SpellerButtons/SpellerButton_%2-Selected.png").arg(m_IconSize).arg(m_IconType));
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CKeyBoardButton::~CKeyBoardButton()
{
    try {
        delete mp_ColorGrey;
        delete mp_ColorWhite;
        if(mp_Timer != 0) {
            delete mp_Timer;
        }
    }
    catch (...){
        //to please lint
    }
}

/****************************************************************************/
/*!
 *  \brief This function is called to  set Button Text
 *  \iparam Text1
 *  \iparam Text2
 */
/****************************************************************************/
void CKeyBoardButton::SetText(QString Text1, QString Text2)
{
    m_BtnText1 = Text1;
    m_BtnText2 = Text2;
}

/****************************************************************************/
/*!
 *  \brief This function is called to  set Button properties
 *
 *  \iparam IconSize
 *  \iparam IconType
 *  \iparam IconPresent
 *  \iparam Alpabet
 */
/****************************************************************************/
void CKeyBoardButton::SetProperties(QString IconSize, QString IconType, bool IconPresent,
                                    bool Alpabet,qint32 ButtonType)
{
    m_IconSize = IconSize;
    m_Size = (m_IconSize == "Small") ? 0x80: 0;
    m_IconPresent = IconPresent;
    m_IconType = IconType;
    m_Alpabet = Alpabet;
    m_ButtonType = static_cast<ButtonType_t> (ButtonType);
}

/****************************************************************************/
/*!
 *  \brief This function is called to inform that subspeller is enabled
 */
/****************************************************************************/
void CKeyBoardButton::SetSubSpellerOff()
{
    m_SubSpeller = false;
    m_Pressed = false;
    update();
}
/****************************************************************************/
/*!
 *  \brief Draws the background image of the button
 */
/****************************************************************************/
void CKeyBoardButton::paintEvent(QPaintEvent *p_PaintEvent)
{
    p_PaintEvent->accept();
    QPixmap *p_Pixmap = m_Pressed ? &m_PixmapPressed : &m_PixmapEnabled;
    QPainter Paint(this);
    Paint.setPen(*mp_ColorWhite);
    Paint.setFont(m_ButtonFont);

    QPixmap Target(size());
    switch (m_ButtonType) {
        case CKeyBoardButton::SPACE:
            Target.fill(Qt::transparent);
            Application::CLeicaStyle::BorderPixmap(&Target, p_Pixmap, 5, 0, 5, 0);
            Paint.drawPixmap(0,0,Target);
            break;
        case CKeyBoardButton::SPECIAL_CHAR:
            Target.fill(Qt::transparent);
            Application::CLeicaStyle::BorderPixmap(&Target, p_Pixmap, 5, 0, 5, 0);
            Paint.drawPixmap(0,0,Target);
            break;
        case CKeyBoardButton::OK:
            Target.fill(Qt::transparent);
            Application::CLeicaStyle::BorderPixmap(&Target, p_Pixmap, 5, 20, 20, 20);
            Paint.drawPixmap(0,0,Target);
            break;
        default:
            break;
    }
    bool EscScaled = false;
    bool CursorScaled = false;
    bool DeleteScaled = false;
    if ( m_Size ) {
        switch (m_ButtonType) {
            case CKeyBoardButton::ESC:
                Target.fill(Qt::transparent);
                Application::CLeicaStyle::BorderPixmap(&Target, p_Pixmap, 20, 22, 10, 22);
                Paint.drawPixmap(0,0,Target);
                EscScaled = true;
                break;
            case (CKeyBoardButton::CURSOR_LEFT):
                Target.fill(Qt::transparent);
                Application::CLeicaStyle::BorderPixmap(&Target, p_Pixmap, 20, 22, 10, 22);
                Paint.drawPixmap(0,0,Target);
                CursorScaled = true;
                break;
            case  (CKeyBoardButton::CURSOR_RIGHT) :
                Target.fill(Qt::transparent);
                Application::CLeicaStyle::BorderPixmap(&Target, p_Pixmap, 20, 22, 10, 22);
                Paint.drawPixmap(0,0,Target);
                CursorScaled = true;
                break;
            case CKeyBoardButton::BACKSPACE:
                Target.fill(Qt::transparent);
                Application::CLeicaStyle::BorderPixmap(&Target, p_Pixmap, 5, 20, 29, 20);
		//Offset of 2 pixels for y co-ordinate because backspace was placed around
		//2 pixels high
                Paint.drawPixmap(0, 1, Target);
                DeleteScaled = true;
                break;
            default:
                break;
        }
    }

    if((m_CapsLock = ((CKeyBoard*)this->mp_Parent)->GetCapsStatus() &&
                     (!((CKeyBoard*)this->mp_Parent)->GetALTStatus())) &&
                     (m_BtnText1 == "Shift" || m_BtnText1 == "ShiftLock")) {
        m_ButtonType = CKeyBoardButton::SHIFT_LOCK;
        m_BtnText1 = "ShiftLock";
        Paint.drawPixmap(0, 0, m_Pressed ? m_PixmapSelectedPressed : m_PixmapSelectedEnabled);
    }
    else if (m_CapsLock == false && m_BtnText1 == "ShiftLock"){
        m_ButtonType = CKeyBoardButton::SHIFT;
        m_BtnText1 = "Shift";
        Paint.drawPixmap(0,0,*p_Pixmap);
    }
    else if ((!(m_ButtonType & (CKeyBoardButton::SPACE |
                                CKeyBoardButton::SPECIAL_CHAR |
                                CKeyBoardButton::OK ))) && (!EscScaled)
                                && (!CursorScaled) && (!DeleteScaled)) {
        if (m_SubSpeller == false) {
           Paint.drawPixmap(0,0,*p_Pixmap);
        }
        else {
            Paint.drawPixmap(0,0, m_Pressed ? m_PixmapTurnRed : m_PixmapDefault);
        }
    }
    //Draw button text in alphabet mode
    if ((m_Alpabet == true) && (m_PressAndHold == false)) {
        if (m_IconPresent == false && m_SubSpeller == false) {
            Paint.drawText(QPoint(m_AlphabetTextX, m_AlphabetTextY),m_BtnText1);
            if (m_BtnText2 != "") {
                Paint.setPen(*mp_ColorGrey);
                if((m_ButtonType != CKeyBoardButton::SHIFT) && (m_ButtonType != CKeyBoardButton::SPECIAL_CHAR)) {
                    Paint.drawText(QPoint(m_SpecialCharX, m_SpecialCharY), m_BtnText2);
                }
            }
        }
        else {
            if((m_IconSize == "Small") && (m_BtnText1 == "Shift" || m_BtnText1 == "ShiftLock")) {
                Paint.drawPixmap(0, 0, QPixmap(QString(":/%1/Icons/%2.png").arg(m_IconSize).arg(m_BtnText1)));
            }
            else {
                Paint.drawPixmap(5, 0, QPixmap(QString(":/%1/Icons/%2.png").arg(m_IconSize).arg(m_BtnText1)));
            }
        }
    }
    else if (m_Alpabet == false){
        if ((m_IconPresent == false) || (m_BtnText1 == "Shift")) {
            //Draw button text in alphanumeric mode
            Paint.setPen(QColor(Qt::white));
            if(m_BtnText2 == "ABC") {
                Paint.drawText(QPoint(m_NumericTextX - 5, m_NumericTextY), m_BtnText2);
            }
            else {
                Paint.drawText(QPoint(m_NumericTextX, m_NumericTextY), m_BtnText2);
            }
        }
    }
    else if (m_PressAndHold == true) {
        //Draw subspeller text
        Paint.drawText(QPoint(m_SubSpellerTextX, 40), m_BtnText1);//25,40
    }
}

/****************************************************************************/
/*!
 *  \brief Event handler for mouse press events
 *
 *  \iparam p_Event = Mouse event
 */
/****************************************************************************/
void CKeyBoardButton::mousePressEvent(QMouseEvent *p_MouseEvent)
{
    p_MouseEvent->accept();
    m_Pressed = true;
    m_Toggled = !m_Toggled;
    mp_Timer->start(1000); // 1 sec
    toggled(m_Toggled);
    repaint();
}

/****************************************************************************/
/*!
 *  \brief Event handler for mouse release events
 *
 *  \iparam p_MouseEvent = Mouse event
 */
/****************************************************************************/
void CKeyBoardButton::mouseReleaseEvent(QMouseEvent *p_MouseEvent)
{
    p_MouseEvent->accept();
    m_Pressed = false;
    if (mp_Timer->isActive()) {
        mp_Timer->stop();
        m_PressAndHold = false;
        emit clicked();
    }
    update();
}

/****************************************************************************/
/*!
 *  \brief This function is called when timer has timed out
 */
/****************************************************************************/
void CKeyBoardButton::TimeOut()
{   
    if ((m_Alpabet) && (!(m_ButtonTextList.contains(m_BtnText1)))) {
        m_SubSpeller = true;
        update();
        emit PressAndHold();
        mp_Timer->stop();
    }
}

}//End of namespace KeyBoard
