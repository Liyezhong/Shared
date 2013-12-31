/****************************************************************************/
/*! \file KeyBoardButton.cpp
 *
 *  \brief Implementation of custom buttons for the keyboard.
 *         The CKeyBoardButton provides button feature for the  keyboard widget.
 *         Features of button:
 *                  - 1) Animates press and release
 *                  - 2) Animates Press and Hold functionality
 *                  - 3) Dual / single character support in a single key.
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

//lint -e655
namespace KeyBoard {
const int PRESS_AND_HOLD_TIMER_TIMEOUT = 1000; //!< 1 sec timer

//Defaut values are set for SIZE_1;
qint32 CKeyBoardButton::m_ButtonWidth = 70; //!< Button width in pixels
qint32 CKeyBoardButton::m_ButtonHeight = 62; //!< Button height in pixels
qint32 CKeyBoardButton::m_NumericTextX = 30; //!< X coordinate of a Numeric text in pixels
qint32 CKeyBoardButton::m_NumericTextY = 40; //!< Y coordingate of a Numeric text in pixels
qint32 CKeyBoardButton::m_AlphabetTextX = 14; //!< X coordinate of Alphabet text in pixels
qint32 CKeyBoardButton::m_AlphabetTextY = 44; //!< Y coordinate of Alphabet text in pixels
qint32 CKeyBoardButton::m_SpecialCharX = 40; //!< X Coodinate of special character in pixels
qint32 CKeyBoardButton::m_SpecialCharY = 25; //!< Y coordinate of speical character in pixel
qint32 CKeyBoardButton::m_SubSpellerTextX = 25; //!< X coordinate of subspeller text in pixel
qint32 CKeyBoardButton::m_FontSize = 15; //!< Font size
QString CKeyBoardButton::m_IconSize = "Large"; //!< Large icons

/*****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 *  \iparam IconType = Left/Center/Right
 *  \iparam PrimaryText = Botton left text on button
 *  \iparam SecondaryText =  Button left text on button
 *  \iparam IconPresent true -icon is present
 *  \iparam ButtonType = One of the button types mentioned in enum ButtonType_t
 *  \iparam SubSpellerButton = True if button is of subspeller type ,else false
 */
/****************************************************************************/
CKeyBoardButton::CKeyBoardButton(QWidget *p_Parent,
                                 QString IconType,
                                 QString PrimaryText,
                                 QString SecondaryText,
                                 bool IconPresent,
                                 ButtonType_t ButtonType,
                                 bool SubSpellerButton)
    : QToolButton(p_Parent),
      mp_Parent(p_Parent),
      mp_Pixmap(NULL),
      mp_Timer(NULL),
      mp_ColorWhite(NULL),
      mp_ColorGrey(NULL),
      m_IconType(IconType),
      m_ButtonTextPrimary(PrimaryText),
      m_ButtonTextSecondary(SecondaryText),
      m_IconPresent(IconPresent),
      m_Clicked(false),
      m_Pressed(false),
      m_Alpabet(true),
      m_PressAndHold(false),
      m_SubSpeller(false),
      m_Toggled(false),
      m_SubSpellerButton(SubSpellerButton),
      m_IgnoreReleaseEvent(false),
      m_MouseMoveEventProcessed(false),
      m_ButtonType(ButtonType)
{
   setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
   setMinimumHeight(m_ButtonHeight);
   setMaximumHeight(m_ButtonHeight);
   setFixedWidth(m_ButtonWidth);
   m_ButtonFont = font();
   m_ButtonFont.setPointSize(m_FontSize);

   m_Size2 = (m_IconSize == "Small") ? true : false;
   //List of buttons for which subspeller is not required
   m_ButtonTextList << "Delete"<< ""<<"#...@"<< "OK"<< "ESC"<< "Shift"<< "ShiftLock"
                    <<"CursorLeft"<<"CursorRight";

   //Timer to implement press and hold feature
   mp_Timer = new QTimer(this);
   CONNECTSIGNALSLOT(mp_Timer, timeout(), this, TimeOut());

   //Text colors
   mp_ColorWhite = new QColor(Qt::white);
   mp_ColorGrey = new QColor("#c0c0c0");
   //Pixmap are created at init so that paint event performance is better
   (void)m_PixmapPressed.load(QString(":/%1/SpellerButtons/SpellerButton_%2-Pressed.png").arg(m_IconSize).arg(m_IconType));
   (void)m_PixmapEnabled.load(QString(":/%1/SpellerButtons/SpellerButton_%2-Enabled.png").arg(m_IconSize).arg(m_IconType));
   (void)m_PixmapSelectedPressed.load(QString(":/%1/SpellerButtons/SpellerButton_Left-SelectedPressed.png").arg(m_IconSize));
   (void)m_PixmapSelectedEnabled.load(QString(":/%1/SpellerButtons/SpellerButton_Left-Selected.png").arg(m_IconSize));
   (void)m_PixmapTurnRed.load(QString(":/%1/SpellerButtons/SpellerButton_%2-SelectedPressed.png").arg(m_IconSize).arg(m_IconType));
   (void)m_PixmapDefault.load(QString(":/%1/SpellerButtons/SpellerButton_%2-Selected.png").arg(m_IconSize).arg(m_IconType));
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
    }
    catch (...){
        //to please lint
    }
}

/****************************************************************************/
/*!
 *  \brief This function is called to  set Button Text
 *  \iparam PrimaryText = Main Text present at lower left of button
 *  \iparam SecondaryText = Text present at top left corner of button
 */
/****************************************************************************/
void CKeyBoardButton::SetText(QString PrimaryText, QString SecondaryText)
{
    m_ButtonTextPrimary = PrimaryText;
    m_ButtonTextSecondary = SecondaryText;
}

/****************************************************************************/
/*!
 *  \brief This function is called to inform that subspeller is disabled
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
 *  \iparam p_PaintEvent = The paint event
 */
/****************************************************************************/
void CKeyBoardButton::paintEvent(QPaintEvent *p_PaintEvent)
{
    p_PaintEvent->accept();
    qDebug()<<"PAINTING"<<m_ButtonTextPrimary;
    QPixmap *p_Pixmap = m_Pressed ? &m_PixmapPressed : &m_PixmapEnabled;
    QPainter Paint(this);
    Paint.setPen(*mp_ColorWhite);
    Paint.setFont(m_ButtonFont);

    QPixmap Target(size());
    switch (m_ButtonType) {
        case KeyBoard::SPACE:
            Target.fill(Qt::transparent);
            Application::CLeicaStyle::BorderPixmap(&Target, p_Pixmap, 5, 0, 6, 0);
            Paint.drawPixmap(0, 0, Target);
            break;
        case KeyBoard::SPECIAL_CHAR:
            Target.fill(Qt::transparent);
            Application::CLeicaStyle::BorderPixmap(&Target, p_Pixmap, 5, 0, 6, 0);
            Paint.drawPixmap(0, 0, Target);
            break;
        case KeyBoard::OK:
            Target.fill(Qt::transparent);
            Application::CLeicaStyle::BorderPixmap(&Target, p_Pixmap, 5, 20, 27, 20); //5,20,20,20
            Paint.drawPixmap(0, 0, Target);
            break;
        default:
            break;
    }
    bool EscScaled = false;
    bool CursorScaled = false;
    bool DeleteScaled = false;
    if ( m_Size2 ) {
        switch (m_ButtonType) {
            case KeyBoard::ESC:
                Target.fill(Qt::transparent);
                Application::CLeicaStyle::BorderPixmap(&Target, p_Pixmap, 20, 22, 10, 22);
                Paint.drawPixmap(0, 0, Target);
                EscScaled = true;
                break;
            case KeyBoard::CURSOR_LEFT:
                Target.fill(Qt::transparent);
                Application::CLeicaStyle::BorderPixmap(&Target, p_Pixmap, 20, 22, 10, 22);
                Paint.drawPixmap(0, 0, Target);
                CursorScaled = true;
                break;
            case KeyBoard::CURSOR_RIGHT :
                Target.fill(Qt::transparent);
                Application::CLeicaStyle::BorderPixmap(&Target, p_Pixmap, 20, 22, 10, 22);
                Paint.drawPixmap(0, 0, Target);
                CursorScaled = true;
                break;
            case KeyBoard::BACKSPACE:
                Target.fill(Qt::transparent);
                Application::CLeicaStyle::BorderPixmap(&Target, p_Pixmap, 5, 20, 29, 20);
                //Offset of 1 pixels for y co-ordinate because backspace was placed around
                //1 pixels high
                Paint.drawPixmap(0, 1, Target);
                DeleteScaled = true;
                break;
            default:
                break;
        }
    }
    bool CapsLock = ((CKeyBoard*)this->mp_Parent)->GetCapsStatus();
    bool AlphabetMode = !((CKeyBoard*)this->mp_Parent)->GetDisplayMode();
    //Shift key painting
    if (CapsLock && (AlphabetMode) && (m_ButtonTextPrimary == "Shift"
                                       || m_ButtonTextPrimary == "ShiftLock")) {
        m_ButtonType = KeyBoard::SHIFT_LOCK;
        m_ButtonTextPrimary = "ShiftLock";
        //Shift key is enabled
        Paint.drawPixmap(0, 0, m_Pressed ? m_PixmapSelectedPressed : m_PixmapSelectedEnabled);
    }
    else if ((!CapsLock) && m_ButtonTextPrimary == "ShiftLock"){
        m_ButtonType = KeyBoard::SHIFT;
        m_ButtonTextPrimary = "Shift";
        Paint.drawPixmap(0,0,*p_Pixmap);
    }
    else if ((!(m_ButtonType & (KeyBoard::SPACE |
                                KeyBoard::SPECIAL_CHAR |
                                KeyBoard::OK ))) && (!EscScaled)
                                && (!CursorScaled) && (!DeleteScaled)) {
        if (m_SubSpeller == false) {
            Paint.drawPixmap(0,0,*p_Pixmap);
        }
        else {
            Paint.drawPixmap(0,0, m_Pressed ? m_PixmapTurnRed : m_PixmapDefault);
        }
    }
    //Draw button text in alphabet mode
    if (!m_SubSpellerButton && (m_Alpabet == true) && (m_PressAndHold == false)) {
        if (m_IconPresent == false && m_SubSpeller == false) {
            Paint.drawText(QPoint(m_AlphabetTextX, m_AlphabetTextY), m_ButtonTextPrimary);
            if (m_ButtonTextSecondary != "") {
                Paint.setPen(*mp_ColorGrey);
                if((m_ButtonType != KeyBoard::SHIFT) && (m_ButtonType != KeyBoard::SPECIAL_CHAR)) {
                    Paint.drawText(QPoint(m_SpecialCharX, m_SpecialCharY), m_ButtonTextSecondary);
                }
            }
        }
        else {
            if((m_IconSize == "Small") && (m_ButtonTextPrimary == "Shift" || m_ButtonTextPrimary == "ShiftLock")) {
                Paint.drawPixmap(0, 0, QPixmap(QString(":/%1/Icons/%2.png").arg(m_IconSize).arg(m_ButtonTextPrimary)));
            }
            else {
                Paint.drawPixmap(5, 0, QPixmap(QString(":/%1/Icons/%2.png").arg(m_IconSize).arg(m_ButtonTextPrimary)));
            }
        }
    }
    else if (!m_SubSpellerButton && m_Alpabet == false){
        if ((m_IconPresent == false) || (m_ButtonTextPrimary == "Shift")) {
            //Draw button text in alphanumeric mode
            Paint.setPen(QColor(Qt::white));
            if(m_ButtonTextSecondary == "ABC") {
                Paint.drawText(QPoint(m_NumericTextX - 5, m_NumericTextY), m_ButtonTextSecondary);
            }
            else {
                Paint.drawText(QPoint(m_NumericTextX, m_NumericTextY), m_ButtonTextSecondary);
            }
        }
    }
    else if (m_SubSpellerButton) {
        //Draw subspeller text
        qDebug()<<"Drawing subspeller";
        Paint.drawText(QPoint(m_SubSpellerTextX, 40), m_ButtonTextPrimary);
    }
}

/****************************************************************************/
/*!
 *  \brief Event handler for mouse press events
 *
 *  \iparam p_MouseEvent = Event for Mouse press
 */
/****************************************************************************/
void CKeyBoardButton::mousePressEvent(QMouseEvent *p_MouseEvent)
{
    p_MouseEvent->accept();
    m_Pressed = true;
    m_Toggled = !m_Toggled;
    if (m_MouseMoveEventProcessed) {
        m_MouseMoveEventProcessed = false;
    }
    //No press and hold feature for button on subspeller
    if (!m_SubSpellerButton) {
        mp_Timer->start(PRESS_AND_HOLD_TIMER_TIMEOUT); // 1 sec
    }
    repaint();
}


/****************************************************************************/
/*!
 *  \brief Handles leave event for the button
 *
 *  \iparam p_MouseEvent = Event for Mouse release
 */
/****************************************************************************/
void CKeyBoardButton::mouseMoveEvent(QMouseEvent *p_MouseEvent)
{
    p_MouseEvent->accept();
    if (!m_MouseMoveEventProcessed) {
        qint32 CurrentXPos = p_MouseEvent->x();
        qint32 CurrentYPos = p_MouseEvent->y();
        if (CurrentXPos < 0 || CurrentYPos < 0 || (CurrentXPos - (this->width()) > 0) || ((CurrentYPos - (this->height()) > 0))) {
            mp_Timer->stop();
            m_Pressed = false;
            m_PressAndHold  = false;
            m_IgnoreReleaseEvent = true;
            m_Toggled = !m_Toggled;
            m_MouseMoveEventProcessed = true;
            update();
        }
    }

}

/****************************************************************************/
/*!
 *  \brief Event handler for mouse release events
 *
 *  \iparam p_MouseEvent = Event for Mouse release
 */
/****************************************************************************/
void CKeyBoardButton::mouseReleaseEvent(QMouseEvent *p_MouseEvent)
{
    p_MouseEvent->accept();
    qint32 CurrentXPos = p_MouseEvent->x();
    qint32 CurrentYPos = p_MouseEvent->y();
    if ((CurrentXPos < 0 ) || (CurrentXPos - (this->width()) > 0)) {
        m_IgnoreReleaseEvent = false;
        return;
    }
    else if ((CurrentYPos < 0 )|| (CurrentYPos - (this->height()) > 0)) {
        m_IgnoreReleaseEvent = false;
        return;
    }
    if (!m_IgnoreReleaseEvent) {
        m_Pressed = false;
        if (mp_Timer->isActive() || m_SubSpellerButton) {
            mp_Timer->stop();
            m_PressAndHold = false;
            emit Buttonclicked();
        }
        toggled(m_Toggled);
        update();
    }
    else {
        //Next time don't ignore
        m_IgnoreReleaseEvent = false;
    }
}

/****************************************************************************/
/*!
 *  \brief This function is called when press and hold timer has timed out
 */
/****************************************************************************/
void CKeyBoardButton::TimeOut()
{   
    if ((m_Alpabet) && (!(m_ButtonTextList.contains(m_ButtonTextPrimary)))) {
        m_SubSpeller = true;
        update();
        emit PressAndHold();
        mp_Timer->stop();
    }
}


}//End of namespace KeyBoard
