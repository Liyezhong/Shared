/****************************************************************************/
/*! \file KeyBoardWidget.cpp
 *
 *  \brief KeyBoard implementation.
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
 *
 */
/****************************************************************************/
#include "Global/Include/Utils.h"
#include "Global/Include/Exception.h"
#include "KeyBoard/Include/KeyBoard.h"
#include "KeyBoard/Include/RoundBGWidget.h"
#include "KeyBoard/Include/SubSpeller.h"
#include "Application/Include/LeicaStyle.h"
#include <QPushButton>
#include <QLineEdit>
#include <QPalette>
#include <QDebug>


namespace KeyBoard {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CKeyBoard::CKeyBoard(KeyBoard::Model_t Model, KeyBoard::KeyBoardType_t KeyBoardType):
    MainMenu::CDialogFrame()
{
    m_Model = Model;
    m_KeyBoardType = KeyBoardType;
    m_Counter = 0;
    m_PreviouCount = 0;
    m_IsCaps = false;
    m_BtnClicked = false;
    m_ShiftCaps = false;
    m_SubSpellerClicked = false;
    mp_SubSpeller = 0;
    m_EnteredCharsValid = false;
    m_MaxEnteredCharLen = 32;
    m_MinEnteredCharLen = 1;
    m_EnteredStringValidation = true ;

    this->move(35,100);
    SetDialogTitle(tr("Dummy"));

    setAttribute(Qt::WA_TranslucentBackground);
    qint32 LineEditWidgetSpacing = 0;
    if (m_Model == KeyBoard::SIZE_1) {
        //considered additional black spaces in the png's.
        //Values were derived based on measurement using gimp.
        m_KeyBoardWidth = 756;
        m_KeyBoardHeight = 454;
        m_IconSize = "Large";
        GetContentFrame()->setFixedSize(756,434);
        CKeyBoardButton::m_ButtonHeight = 62;
        CKeyBoardButton::m_ButtonWidth = 70;
        m_LineEditHeight = 59;
        m_LineEditWidth = 629;
        m_LineEditMarginLeft = 9;
        m_LineEditMarginTop = 0;
        m_LineEditMarginRight = 0;
        m_LineEditMarginBottom = 0;
        m_KeyTrayMarginLeft = 10;
        m_KeyTrayMarginTop = 5;
        m_KeyTrayMarginRight = 0;
        m_KeyTrayMarginBottom = 9;
        m_KeyTrayHeight = 296;
        m_KeyTrayWidth = 721;
        m_MainMarginLeft = 15;
        m_MainMarginTop = 13;
        m_MainMarginRight = 0;
        m_MainMarginBottom = 36;
        m_SpaceKeyWidth = 280;
        m_SpecialCharKeyWidth = 100;
        m_OkKeyWidht = 110;
        m_LineEditWidgetW = 715;
        m_LineEditWidgetH = 63;
        LineEditWidgetSpacing = 1;
    }
    else {
        // KeyBoard::SIZE_2
        m_KeyBoardWidth = 539;
        m_KeyBoardHeight = 471;
        GetContentFrame()->setFixedSize(539, 464);
        CKeyBoardButton::m_ButtonHeight = 73;
        CKeyBoardButton::m_ButtonWidth = 46;
        CKeyBoardButton::m_NumericTextX = 15;
        CKeyBoardButton::m_NumericTextY = 45;
        CKeyBoardButton::m_AlphabetTextX = 9;
        CKeyBoardButton::m_AlphabetTextY = 52;
        CKeyBoardButton::m_SpecialCharX = 20;
        CKeyBoardButton::m_SpecialCharY = 27;
        CKeyBoardButton::m_SubSpellerTextX = 15;
        CKeyBoardButton::m_FontSize = 10;
        CKeyBoardButton::m_IconSize = "Small";
        CSubSpeller::m_Model = "Small";
        m_LineEditHeight = 61;
        m_LineEditWidth = 386;
        m_LineEditMarginLeft = 8;
        m_LineEditMarginTop = 0;
        m_LineEditMarginRight = 0;
        m_LineEditMarginBottom = 0;
        LineEditWidgetSpacing = 0;
        m_KeyTrayMarginLeft = 9;
        m_KeyTrayMarginTop = 8;
        m_KeyTrayMarginRight = 0;
        m_KeyTrayMarginBottom = 12;
        m_KeyTrayHeight = 315;
        m_KeyTrayWidth = 481;
        m_MainMarginLeft = 20;
        m_MainMarginTop = 10;
        m_MainMarginRight = 0;
        m_MainMarginBottom = 48;//45
        m_SpaceKeyWidth = 117;
        m_SpecialCharKeyWidth = 70;
        m_OkKeyWidht = 78;
        CSubSpeller::m_SubspellerOffsetW = 25;
        CSubSpeller::m_SubspellerOffsetH = 25;
        CSubSpeller::m_SubspellerBorderMarginLeft = 30;
        CSubSpeller::m_SubspellerBorderMarginTop = 27;
        CSubSpeller::m_SubspellerBorderMarginRight = 30;
        CSubSpeller::m_SubspellerBorderMarginBottom = 27;
        CSubSpeller::m_SubspellerMarginRight = 5;
        m_LineEditWidgetW = 481;
        m_LineEditWidgetH = 67;
    }

    mp_LineEditWidget = new QWidget(GetContentFrame());
    mp_LineEditWidget->setSizePolicy(QSizePolicy::Preferred,
                                    QSizePolicy::Fixed);
    mp_LineEditLayout = new QHBoxLayout(mp_LineEditWidget);
    mp_LineEditLayout->setContentsMargins(m_LineEditMarginLeft, m_LineEditMarginTop,
                                       m_LineEditMarginRight, m_LineEditMarginBottom);

    mp_LineEdit = new QLineEdit(mp_LineEditWidget);
    mp_LineEdit->setSizePolicy(QSizePolicy::Preferred,
                               QSizePolicy::Fixed);
    mp_LineEdit->setEchoMode(QLineEdit::Password);
    mp_LineEdit->setTextMargins(13, 0, 0, 0);
    //mp_LineEdit->setFocusPolicy(Qt::NoFocus);
    mp_LineEdit->setFrame(false);
    QFont LineEditFont = font();
    LineEditFont.setPointSize(CKeyBoardButton::m_FontSize);
    mp_LineEdit->setFont(LineEditFont);
    mp_LineEdit->setFixedSize(m_LineEditWidth, m_LineEditHeight);
    mp_LineEditLayout->addWidget(mp_LineEdit);

    mp_LineEditLayout->setSpacing(LineEditWidgetSpacing);

    mp_DeleteBtn = new CKeyBoardButton(mp_LineEditWidget,
                                        "Right", "Delete", "", true, 0x100);
    mp_DeleteBtn->setAccessibleName("0x01000003");
    mp_DeleteBtn->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Fixed);
    mp_LineEditLayout->addWidget(mp_DeleteBtn);
    mp_LineEditWidget->setFixedSize(m_LineEditWidgetW, m_LineEditWidgetH);

    mp_KeyTrayBackGround = new CRoundWidget(GetContentFrame());
    mp_KeyTrayBackGround->setFixedSize(m_KeyTrayWidth, m_KeyTrayHeight);
    mp_KeyTrayBackGround->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mp_KeyTrayBackGround->Init();

    this->setFixedSize(m_KeyBoardWidth, m_KeyBoardHeight);

    mp_KeyBoardBaseLayout = new QVBoxLayout(GetContentFrame());
    mp_KeyBoardBaseLayout->addWidget(mp_LineEditWidget);
    mp_KeyBoardBaseLayout->addWidget(mp_KeyTrayBackGround);
    if(m_Model == KeyBoard::SIZE_2) {
       mp_KeyBoardBaseLayout->setSpacing(07);
    }
    mp_KeyBoardBaseLayout->setContentsMargins(m_MainMarginLeft, m_MainMarginTop,
                                           m_MainMarginRight, m_MainMarginBottom);
    CreateKeyboard();
    QApplication::setCursorFlashTime(0);//This stops cursor blinking.

    //Connect the signals from buttons to signal mapper, further to the button clicked
    //slot.
    m_ButtonList = findChildren<CKeyBoardButton *>();
    mp_ClickedMapper = new QSignalMapper(this);
    for (int i = 0;i < m_ButtonList.count(); i++) {
        CONNECTSIGNALSLOT(m_ButtonList.at(i), clicked(), mp_ClickedMapper, map());
        mp_ClickedMapper->setMapping(m_ButtonList.at(i), i);
    }
    CONNECTSIGNALSLOT(mp_ClickedMapper, mapped(int), this, BtnClicked(int));

    mp_PressedMapper = new QSignalMapper(this);
    for (int I = 1; I < 31; I++) {
           CONNECTSIGNALSLOT(m_ButtonList.at(I), PressAndHold(), mp_PressedMapper, map());
           mp_PressedMapper->setMapping(m_ButtonList.at(I), I);
    }
    CONNECTSIGNALSLOT(mp_PressedMapper, mapped(int), this, PressAndHold(int));

    //The below lines where added because ,setting focus to lineedit used to select
    //the text in lineedit when text was set using SetLineEditContent()function , even
    // QLineEdit's deselect function didnt work.
    mp_LineEdit->setCursorPosition(0);
    mp_LineEdit->setText(tr("Dummy"));
    mp_LineEdit->clear();
}

 /****************************************************************************/
 /*!
  *  \brief Destructor
  */
 /****************************************************************************/
CKeyBoard::~CKeyBoard()
{
    try {
        if(mp_SubSpeller) {
            delete mp_SubSpeller;
        }
        while(!m_ButtonList.isEmpty()) {
            delete m_ButtonList.takeFirst();
        }
        delete mp_EscLayout;
        delete mp_ZxcdLayout;
        delete mp_AsdfLayout;
        delete mp_QwertyLayout;
        delete mp_BaseKeyLayout;
        delete mp_PressedMapper;
        delete mp_ClickedMapper;
        delete mp_KeyTrayBackGround;
        delete mp_LineEditLayout;
        delete mp_LineEditWidget;
        delete mp_KeyBoardBaseLayout;
        Detach();
    }
    catch (...) {
        // to please Lint.
    }
}

/****************************************************************************/
/*!
 *  \brief This function is used to display asterisks when in password mode
 */
/****************************************************************************/

void CKeyBoard::SetPasswordMode(bool Mode)
{
    if(Mode){
        mp_LineEdit->setEchoMode(QLineEdit::Password);
    }
    else {
        mp_LineEdit->setEchoMode(QLineEdit::Normal);
    }

}

/****************************************************************************/
/*!
 *  \brief This function is  called to reset the keyboard to default state
 */
/****************************************************************************/
void CKeyBoard::KeyBoardReset()
{
    m_Counter = 0;
    m_PreviouCount = 0;
    m_IsCaps = false;
    m_BtnClicked = false;
    m_ShiftCaps = false;
    m_SubSpellerClicked = false;
    SetCaps(false);
    if (m_EnteredCharsValid) {
        Detach();
    }
}


/****************************************************************************/
/*!
 *  \brief This slot is called if there is a press and hold on a button
 *
 *  \iparam Btn value of the button
 */
/****************************************************************************/
void CKeyBoard::CreateSubSpeller(int Btn)
{
    ;
    qint16 Offset = 0;
    qint16 Index = 0;
    bool SubSpeller = false;
    if ((m_SubSpellerClicked == true) && (mp_SubSpeller != 0 )) {
        delete mp_SubSpeller;
        mp_SubSpeller = 0;
    }
    qint16 AsciiVal = m_ButtonList.at(Btn)->GetText1().at(0).toAscii();
    if(m_ShiftCaps == false) {
        //LowerCase
        if (((AsciiVal >= 97) && (AsciiVal <= 122))) {
            Offset = 0;
            Index = AsciiVal - 97;
            SubSpeller = true;
        }
        else if ((AsciiVal == 44) ||(AsciiVal == 46)) {
            Offset = 26;
            Index = ((AsciiVal - 44) == 2) ? 1 :0;
            SubSpeller = true;
        }
    }
    else {
        //Upper case
        if ((AsciiVal >= 65) && (AsciiVal <= 90)) {
            Offset = 28;
            Index = AsciiVal - 65;
            SubSpeller = true;
        }
        else if ((AsciiVal == 44) ||(AsciiVal == 46)) {
            Offset = 54;
            Index = ((AsciiVal - 44) == 2) ? 1 :0;
            SubSpeller = true;
        }
    }

    if (SubSpeller == true) {
        mp_SubSpeller = new CSubSpeller(this, m_ButtonList.at(Btn), Offset, Index, m_Model, m_KeyBoardType);
        mp_SubSpeller->setWindowModality(Qt::ApplicationModal);
        mp_SubSpeller->show();
        CONNECTSIGNALSLOT(mp_SubSpeller, SubSpellerClicked(), this, SubSpellerClicked());
    }
}


/****************************************************************************/
/*!
 *  \brief This slot is called if there is a press and hold on a button
 *
 *  \iparam Btn value of the button
 */
/****************************************************************************/
void CKeyBoard::PressAndHold(int Btn)
{
    if(!m_AltToggled) {
        CreateSubSpeller(Btn);
    }
}

/****************************************************************************/
/*!
 *  \brief This slot is called if a button on subspeller is clicked
 */
/****************************************************************************/
void CKeyBoard::SubSpellerClicked()
{
    m_SubSpellerClicked = true;
    //if shift was pressed previously then reset the keys back to lower case.
    if ((!m_IsCaps) && (m_Shift == true) && (!m_AltToggled)) {
        SetCaps(false);
        m_Counter = 0;
    }
    m_Shift = false;
}

/****************************************************************************/
/*!
 *  \brief This slot is called when one of the Button is pressed
 *
 *  \iparam Btn value of the button
 */
/****************************************************************************/
void CKeyBoard::BtnClicked(int Btn)
{
    QString StrKeyId = m_ButtonList.at(Btn)->accessibleName();
    bool Isok;
    int KeyId = StrKeyId.toInt(&Isok,16);
    QString KeyChar;

    switch (KeyId) {
	//TODO : Check if switch is better than if else?
        default:
            break;
        case Qt::Key_Shift:
            m_Counter++;
            if(m_Counter == 2) {
                //CAPS LOCK
                m_IsCaps = true;
                m_Counter = 0;
                SetCaps(true);
                m_PreviouCount = 2;
            }
            else {
                if ((m_PreviouCount != 2) && (m_PreviouCount !=1)) {
                    qDebug("Shift toggle");
                    SetCaps(!m_IsCaps);
                }
                else if (m_PreviouCount == 2){
                    SetCaps(false);
                    m_PreviouCount = m_Counter;
                    m_Counter = 0;
                }
                else {
                    qDebug("Shift");
                    SetCaps(true);
                }
                m_Shift = true;
                m_IsCaps = false;
            }
            return;
        case Qt::Key_Alt:
            return;

        case Qt::Key_Escape:
        case Qt::Key_Enter:
            return;

        case Qt::Key_Backspace:
             mp_LineEdit->backspace();
            goto reset;
        case Qt::Key_Left:
            mp_LineEdit->cursorBackward(false, 1);
            goto reset;
        case Qt::Key_Right:
            mp_LineEdit->cursorForward(false, 1);
            //If Shift was clicked previously and a key is pressed then reset keys back to lowercase
        reset:if ((!m_IsCaps) && (m_Shift == true) && (!m_AltToggled)) {
                SetCaps(false);
                m_Counter = 0;
            }
            return;
    }

    //Check the mode of keyboard, alphabet mode or special char mode
    if(!m_AltToggled) {
        KeyChar = m_ButtonList.at(Btn)->GetText1();
    }
    else {
        KeyChar = m_ButtonList.at(Btn)->GetText2();
    }

    if (KeyId == Qt::Key_Space) {
            KeyChar = " ";
    }
    //Replaced KeyEvent and SendEvent with Insert.Gave better performance
    mp_LineEdit->insert(KeyChar);
    if (m_Shift) {
        if ((!m_IsCaps) && (!m_AltToggled)) {
            SetCaps(false);
            m_Counter = 0;
        }
    }
    m_Shift = false;
}

/****************************************************************************/
/*!
 *  \brief This slot is called when alphanumeric key is toggled
 *
 *  \iparam Checked toggles b/w true and false
 */
/****************************************************************************/
void CKeyBoard::AltToggled(bool Checked)
{
    m_AltToggled = Checked;
    if (Checked) {
        m_ButtonList.at(20)->setAccessibleName("0x60");
        m_ButtonList.at(30)->setAccessibleName("0x1");//dummy name for ESC
    }
    else {
        //Shift Key
        m_ButtonList.at(20)->setAccessibleName("0x01000020");
        m_ButtonList.at(30)->setAccessibleName("0x01000000");
    }
    MoveCharacters(Checked);
    for (qint16 X = 1;X < 32 ; X++) {
        m_ButtonList.at(X)->IsAlpha(!Checked);
        m_ButtonList.at(X)->update();
    }
 }

/****************************************************************************/
/*!
 *  \brief This function is called to  set Button labels to caps
 *
 *  \iparam IsCaps True- Set CAPS
 */
/****************************************************************************/
void CKeyBoard::SetCaps(bool IsCaps) {
    QString String = QChar(176);
    if (IsCaps == true) {
        m_ShiftCaps = true;
        m_ButtonList.at(1)->SetText("Q", "1");
        m_ButtonList.at(2)->SetText("W", "2");
        m_ButtonList.at(3)->SetText("E", "3");
        m_ButtonList.at(4)->SetText("R", "4");
        m_ButtonList.at(5)->SetText("T", "5");
        if (m_KeyBoardType == QWERTY_KEYBOARD) {
            m_ButtonList.at(6)->SetText("Y", "6");
        }
        else {
            m_ButtonList.at(6)->SetText("Z", "6");
        }
        m_ButtonList.at(7)->SetText("U", "7");
        m_ButtonList.at(8)->SetText("I", "8");
        m_ButtonList.at(9)->SetText("O", "9");
        m_ButtonList.at(10)->SetText("P", "0");
        m_ButtonList.at(11)->SetText("A", "!");
        m_ButtonList.at(12)->SetText("S", "@");
        m_ButtonList.at(13)->SetText("D", "#");
        m_ButtonList.at(14)->SetText("F", "$");
        m_ButtonList.at(15)->SetText("G", "%");
        m_ButtonList.at(16)->SetText("H", "&");
        m_ButtonList.at(17)->SetText("J", "*");
        m_ButtonList.at(18)->SetText("K", "?");
        m_ButtonList.at(19)->SetText("L", "/");
        if (m_KeyBoardType == QWERTY_KEYBOARD) {
            m_ButtonList.at(21)->SetText("Z", "~");
        }
        else {
            m_ButtonList.at(21)->SetText("Y", "~");
        }
        m_ButtonList.at(22)->SetText("X", "\"");
        m_ButtonList.at(23)->SetText("C", String);
        m_ButtonList.at(24)->SetText("V", "(");
        m_ButtonList.at(25)->SetText("B", ")");
        m_ButtonList.at(26)->SetText("N", "-");
        m_ButtonList.at(27)->SetText("M", "_");

    }
    else {
        m_ShiftCaps = false;
        m_ButtonList.at(1)->SetText("q", "1");
        m_ButtonList.at(2)->SetText("w", "2");
        m_ButtonList.at(3)->SetText("e", "3");
        m_ButtonList.at(4)->SetText("r", "4");
        m_ButtonList.at(5)->SetText("t", "5");
        if (m_KeyBoardType == QWERTY_KEYBOARD) {
            m_ButtonList.at(6)->SetText("y", "6");
        }
        else {
            m_ButtonList.at(6)->SetText("z", "6");
        }
        m_ButtonList.at(7)->SetText("u", "7");
        m_ButtonList.at(8)->SetText("i", "8");
        m_ButtonList.at(9)->SetText("o", "9");
        m_ButtonList.at(10)->SetText("p", "0");
        m_ButtonList.at(11)->SetText("a", "!");
        m_ButtonList.at(12)->SetText("s", "@");
        m_ButtonList.at(13)->SetText("d", "#");
        m_ButtonList.at(14)->SetText("f", "$");
        m_ButtonList.at(15)->SetText("g", "%");
        m_ButtonList.at(16)->SetText("h", "&");
        m_ButtonList.at(17)->SetText("j", "*");
        m_ButtonList.at(18)->SetText("k", "?");
        m_ButtonList.at(19)->SetText("l", "/");
        if (m_KeyBoardType == QWERTY_KEYBOARD) {
            m_ButtonList.at(21)->SetText("z", "~");
        }
        else {
            m_ButtonList.at(21)->SetText("y", "~");
        }
        m_ButtonList.at(22)->SetText("x", "\"");
        m_ButtonList.at(23)->SetText("c", String);
        m_ButtonList.at(24)->SetText("v", "(");
        m_ButtonList.at(25)->SetText("b", ")");
        m_ButtonList.at(26)->SetText("n", "-");
        m_ButtonList.at(27)->SetText("m", "_");
    }

    for(qint16 X = 1;X < 32 ; X++) {
        m_ButtonList.at(X)->update();
    }
}

/****************************************************************************/
/*!
 *  \brief This function sets KeyBoard Type
 *
 *  \iparam m_KeyBoardType QWERTY/QWERTZ KeyBoard.
 */
/****************************************************************************/
void CKeyBoard::SetKeyBoardType(KeyBoard::KeyBoardType_t KeyBoardType)
{
    //Remove below line after testing.This should be done in changeevent
    m_KeyBoardType = KeyBoardType ;
    if (KeyBoardType == QWERTY_KEYBOARD ) {
        if (m_IsCaps || m_ShiftCaps) {
            m_ButtonList.at(6)->SetText("Y", "6");
            m_ButtonList.at(21)->SetText("Z", "~");
        }
        else {
            m_ButtonList.at(6)->SetText("y", "6");
            m_ButtonList.at(21)->SetText("z", "~");
        }
    }
    else {
        //QWERTZ_KEYBOARD
        if (m_IsCaps || m_ShiftCaps) {
            m_ButtonList.at(6)->SetText("Z", "6");
            m_ButtonList.at(21)->SetText("Y", "~");
        }
        else {
            m_ButtonList.at(6)->SetText("z", "6");
            m_ButtonList.at(21)->SetText("y", "~");
        }
    }
    m_ButtonList.at(6)->update();
    m_ButtonList.at(21)->update();
}

/****************************************************************************/
/*!
 *  \brief This function moves button text by one button left or right
 *
 *  \iparam Checked - if true move characters
 */
/****************************************************************************/
void CKeyBoard::MoveCharacters(bool Checked) {
    if (Checked) {
        m_ButtonList.at(20)->SetText("Shift", "~");
        if (m_KeyBoardType == QWERTY_KEYBOARD) {
            m_ButtonList.at(21)->SetText("Z", "\"");
        }
        else {
            m_ButtonList.at(21)->SetText("Y", "\"");
        }
        QString String = QChar(176);
        m_ButtonList.at(22)->SetText("X", String);
        m_ButtonList.at(23)->SetText("C", "(");
        m_ButtonList.at(24)->SetText("V", ")");
        m_ButtonList.at(25)->SetText("B", "-");
        m_ButtonList.at(26)->SetText("N", "_");
        m_ButtonList.at(27)->SetText("M", ";");
        m_ButtonList.at(28)->SetText(",", ":");
        m_ButtonList.at(29)->SetText(".", ",");
        m_ButtonList.at(30)->SetText("", ".");
    }
    else {
        if (m_ShiftCaps) {
            m_ButtonList.at(20)->SetText("Shift", "");
            if (m_KeyBoardType == QWERTY_KEYBOARD) {
                m_ButtonList.at(21)->SetText("Z", "~");
            }
            else {
                m_ButtonList.at(21)->SetText("Y", "~");
            }
            m_ButtonList.at(22)->SetText("X", "\"");
            QString String = QChar(176);
            m_ButtonList.at(23)->SetText("C", String);
            m_ButtonList.at(24)->SetText("V", "(");
            m_ButtonList.at(25)->SetText("B", ")");
            m_ButtonList.at(26)->SetText("N", "-");
            m_ButtonList.at(27)->SetText("M", "_");
            m_ButtonList.at(28)->SetText(",", ";");
            m_ButtonList.at(29)->SetText(".", ":");
            m_ButtonList.at(30)->SetText("ESC", "");
        }
        else {
            m_ButtonList.at(20)->SetText("Shift", "");
            if (m_KeyBoardType == QWERTY_KEYBOARD) {
                m_ButtonList.at(21)->SetText("z", "~");
            }
            else {
                m_ButtonList.at(21)->SetText("y", "~");
            }
            m_ButtonList.at(22)->SetText("x", "\"");
            QString String = QChar(176);
            m_ButtonList.at(23)->SetText("c", String);
            m_ButtonList.at(24)->SetText("v", "(");
            m_ButtonList.at(25)->SetText("b", ")");
            m_ButtonList.at(26)->SetText("n", "-");
            m_ButtonList.at(27)->SetText("m", "_");
            m_ButtonList.at(28)->SetText(",", ";");
            m_ButtonList.at(29)->SetText(".", ":");
            m_ButtonList.at(30)->SetText("ESC", "");
        }
    }
}

/****************************************************************************/
/*!
 *  \brief This function returns the string present in lineEdit
 *
 */
/****************************************************************************/
QString CKeyBoard::GetLineEditString()
{
    // Added simplified method to remove unnecessary spaces in the entered text.
    m_LineEditString = mp_LineEdit->text().simplified();
    mp_LineEdit->clear();
    hide();
    KeyBoardReset();
    AltToggled(false);
    return m_LineEditString;

}

/****************************************************************************/
/*!
 *  \brief This function hides the keyboard when Esc is clicked
 */
/****************************************************************************/
void CKeyBoard::EscClicked()
{
    if(m_ButtonList.at(30)->accessibleName() == "0x01000000") {
       mp_LineEdit->clear();
       hide();
       KeyBoardReset();
       AltToggled(false);
    }
}

/****************************************************************************/
/*!
 *  \brief This function sets the dialog title of the keyboard
 */
/****************************************************************************/
void CKeyBoard::SetKeyBoardDialogTitle(QString Title)
{
    SetDialogTitle(tr("%1").arg(Title));
}


/****************************************************************************/
/*!
 *  \brief This function is called to create a new key on the keyboard
 *
 *  \iparam IconSize Large/small
 *  \iparam IconType Left/Center/Right
 *  \iparam BtnText1 Bottom left text on button
 *  \iparam BtnText2 Top left text on button
 *  \iparam IconPresent true -icon is present
 *  \iparam ButtonType
 */
/****************************************************************************/
CKeyBoardButton *CKeyBoard::CreateNewKey(QString IconType,
    QString BtnText1,QString BtnText2,bool IconPresent, qint32 ButtonType)
{
    CKeyBoardButton *TmpButton = new CKeyBoardButton(this, IconType,
                                                     BtnText1, BtnText2, IconPresent, ButtonType);
    return TmpButton;
}

/****************************************************************************/
/*!
 *  \brief This function is called to create a Keyboard Layout.
 *
 */
/****************************************************************************/
void CKeyBoard::CreateKeyboard()
{

    mp_BaseKeyLayout = new QVBoxLayout(mp_KeyTrayBackGround);
    mp_BaseKeyLayout->setSpacing(6);

    mp_QwertyLayout = new QHBoxLayout();
    mp_AsdfLayout = new QHBoxLayout();
    mp_ZxcdLayout = new QHBoxLayout();
    mp_EscLayout = new QHBoxLayout();
    mp_BaseKeyLayout->addLayout(mp_QwertyLayout);
    mp_BaseKeyLayout->addLayout(mp_AsdfLayout);
    mp_BaseKeyLayout->addLayout(mp_ZxcdLayout);
    mp_BaseKeyLayout->addLayout(mp_EscLayout);
    mp_BaseKeyLayout->setContentsMargins(m_KeyTrayMarginLeft, m_KeyTrayMarginTop,
                                         m_KeyTrayMarginRight, m_KeyTrayMarginBottom);//considered black shadows in png images

    mp_QwertyLayout->setSpacing(0);
    mp_QwertyLayout->addWidget(CreateNewKey("Left", "q", "1", false, 0), 1);
    mp_QwertyLayout->addWidget(CreateNewKey("Center", "w", "2", false, 0), 1);
    mp_QwertyLayout->addWidget(CreateNewKey("Center", "e", "3", false, 0), 1);
    mp_QwertyLayout->addWidget(CreateNewKey("Center", "r", "4", false, 0), 1);
    mp_QwertyLayout->addWidget(CreateNewKey("Center", "t", "5", false, 0), 1);

    if(m_KeyBoardType == QWERTY_KEYBOARD) {
        mp_QwertyLayout->addWidget(CreateNewKey("Center", "y", "6",false, 0), 1);
    }
    else {
        mp_QwertyLayout->addWidget(CreateNewKey("Center", "z", "6",false, 0), 1);
    }
    mp_QwertyLayout->addWidget(CreateNewKey("Center", "u", "7", false, 0), 1);
    mp_QwertyLayout->addWidget(CreateNewKey("Center", "i", "8", false, 0), 1);
    mp_QwertyLayout->addWidget(CreateNewKey("Center", "o", "9", false, 0), 1);
    mp_QwertyLayout->addWidget(CreateNewKey("Right", "p", "0", false, 0), 1);


    mp_AsdfLayout->addStretch(1);
    mp_AsdfLayout->setSpacing(0);

    mp_AsdfLayout->addWidget(CreateNewKey("Left", "a", "!", false, 0), 1);
    mp_AsdfLayout->addWidget(CreateNewKey("Center", "s",  "@", false, 0), 1);
    mp_AsdfLayout->addWidget(CreateNewKey("Center", "d", "#", false, 0), 1);
    mp_AsdfLayout->addWidget(CreateNewKey("Center", "f", "$", false, 0), 1);
    mp_AsdfLayout->addWidget(CreateNewKey("Center", "g", "%", false, 0), 1);
    mp_AsdfLayout->addWidget(CreateNewKey("Center", "h", "&", false, 0), 1);
    mp_AsdfLayout->addWidget(CreateNewKey("Center", "j", "*", false, 0), 1);
    mp_AsdfLayout->addWidget(CreateNewKey("Center", "k", "?", false, 0), 1);
    mp_AsdfLayout->addWidget(CreateNewKey("Right", "l", "/", false, 0), 1);
    mp_AsdfLayout->addStretch(1);

    mp_ZxcdLayout->setSpacing(0);
    CKeyBoardButton *p_TempButton = NULL;
    p_TempButton = CreateNewKey("Left", "Shift", "~", true, 0x20);
    mp_ZxcdLayout->addWidget(p_TempButton,0);
    //Accessible names set according to  enum Qt::Key.Refer qt docs
    p_TempButton->setAccessibleName("0x01000020");
    if (m_KeyBoardType == QWERTY_KEYBOARD) {
        mp_ZxcdLayout->addWidget(CreateNewKey("Center", "z", "~", false, 0), 0);
    }
    else {
        mp_ZxcdLayout->addWidget(CreateNewKey("Center", "y", "~", false, 0), 0);
    }

    mp_ZxcdLayout->addWidget(CreateNewKey("Center", "x", "\"", false, 0), 0);
    QString String = QChar(176);//ASCII for degree sign
    mp_ZxcdLayout->addWidget(CreateNewKey("Center","c", String, false, 0), 0);
    mp_ZxcdLayout->addWidget(CreateNewKey("Center", "v", "(", false, 0), 0);
    mp_ZxcdLayout->addWidget(CreateNewKey("Center", "b", ")", false, 0), 0);
    mp_ZxcdLayout->addWidget(CreateNewKey("Center", "n", "-", false, 0), 0);
    mp_ZxcdLayout->addWidget(CreateNewKey("Center", "m", "_", false, 0), 0);
    mp_ZxcdLayout->addWidget(CreateNewKey("Center", ",", ";", false, 0), 0);
    mp_ZxcdLayout->addWidget(CreateNewKey("Right", ".", ":", false, 0), 0);


    mp_EscLayout->setSpacing(0);
    p_TempButton = CreateNewKey("Left", "ESC", "", false, 8);
    p_TempButton->setAccessibleName("0x01000000");
    if (m_Model == KeyBoard::SIZE_2) {
        p_TempButton->setMinimumWidth(68);
        p_TempButton->setMaximumWidth(68);
    }
    mp_EscLayout->addWidget(p_TempButton);
    CONNECTSIGNALSLOT(p_TempButton,clicked(),this,EscClicked());
    p_TempButton = CreateNewKey("Center", "#...@", "ABC", false, 2);
    p_TempButton->setAccessibleName("0x01000023");
    p_TempButton->setCheckable(true);
    p_TempButton->setMaximumWidth(m_SpecialCharKeyWidth);
    p_TempButton->setMinimumWidth(m_SpecialCharKeyWidth);
    CONNECTSIGNALSLOT(p_TempButton,toggled(bool),this,AltToggled(bool));
    mp_EscLayout->addWidget(p_TempButton);


    p_TempButton = CreateNewKey("Center", "", "", false, 1);
    p_TempButton->setMaximumWidth(m_SpaceKeyWidth);
    p_TempButton->setMinimumWidth(m_SpaceKeyWidth);
    p_TempButton->setAccessibleName("0x20");
    mp_EscLayout->addWidget(p_TempButton);

    p_TempButton = CreateNewKey("Center", "CursorLeft", "", true, 0x40);
    p_TempButton->setAccessibleName("0x01000012");
    if (m_Model == KeyBoard::SIZE_2) {
       p_TempButton->setMinimumWidth(67);
       p_TempButton->setMaximumWidth(67);
    }
    mp_EscLayout->addWidget(p_TempButton);
    p_TempButton = CreateNewKey("Center", "CursorRight", "", true, 0x80);
    p_TempButton->setAccessibleName("0x01000014");
    if (m_Model == KeyBoard::SIZE_2) {
       p_TempButton->setMinimumWidth(67);
       p_TempButton->setMaximumWidth(67);
       mp_DeleteBtn->setFixedSize(78, 71);
       mp_QwertyLayout->setContentsMargins(1, 0, 4, 0);
       mp_ZxcdLayout->setContentsMargins(1, 0, 4, 0);
       mp_EscLayout->setContentsMargins(1, 0, 2, 0);
       mp_AsdfLayout->setContentsMargins(1, 0, 13, 0);
       mp_BaseKeyLayout->setSpacing(6);
    }
    mp_EscLayout->addWidget(p_TempButton);

    p_TempButton = CreateNewKey("Right", "OK", "", false, 4);
    //Have set accessible name of "enter"
    p_TempButton->setAccessibleName("0x01000005");
    CONNECTSIGNALSLOT(p_TempButton, clicked(), this, OnOkClicked());
    p_TempButton->setMinimumWidth(m_OkKeyWidht);
    p_TempButton->setMaximumWidth(m_OkKeyWidht);
    mp_EscLayout->addWidget(p_TempButton);

    if (m_Model ==  KeyBoard::SIZE_1) {
	mp_QwertyLayout->setContentsMargins(0, 0, 7, 0);
	mp_ZxcdLayout->setContentsMargins(0, 0, 7,0);
        mp_EscLayout->setContentsMargins(0, 0, 7, 0);
        mp_AsdfLayout->setContentsMargins(0, 0, 24, 0);
    }

}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/
void CKeyBoard::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            //TODO :Get current language (Assuming a static member variable
            //shall provide current language) and change KeyBoard type accordingly.
            SetKeyBoardType(QWERTZ_KEYBOARD);
            break;
        default:
            break;
    }
}

/****************************************************************************/
/*!
 *  \brief This function is called to register the Observer. Note-Don't
 *         register more than observer.
 */
/****************************************************************************/
void CKeyBoard::Attach(KeyBoard::CKeyBoardObserver *p_KeyBoardObserver)
{
    mp_KeyBoardObserver.push_back(p_KeyBoardObserver);
}

/****************************************************************************/
/*!
 *  \brief This function is called to unregister the Observers
 */
/****************************************************************************/
void CKeyBoard::Detach()
{
    for(qint32 I = 0; I < mp_KeyBoardObserver.size(); I++) {
        if (!mp_KeyBoardObserver.isEmpty()) {
            mp_KeyBoardObserver.pop_back();
        }
    }
}

/****************************************************************************/
/*!
 *  \brief This function is called to Notify the observers when OK is clicked.
 */
/****************************************************************************/
void CKeyBoard::NotifyObserver()
{
    for(qint32 I = 0; I < mp_KeyBoardObserver.size(); I++) {
        mp_KeyBoardObserver[I]->Update();
    }
}

/****************************************************************************/
/*!
 *  \brief This function is called when OK is clicked
 */
/****************************************************************************/
void CKeyBoard::OnOkClicked()
{
  //  unsigned int EnteredCharLen = mp_LineEdit->text().length();
    QString EnteredText = mp_LineEdit->text();
    ValidateString(EnteredText);
    MainMenu::CMessageDlg MessageDlg(this);
    MessageDlg.SetTitle(tr("Information Message"));
    MessageDlg.SetIcon(QMessageBox::Information);
    MessageDlg.SetButtonText(1, tr("Ok"));
    MessageDlg.HideButtons();
//    if (MessageDlg.exec()) {
//        // to please Lint..
//    }
    qDebug()<<"After Validation";
    mp_LineEdit->clear();
    m_EnteredCharsValid = false;
    KeyBoardReset();
    AltToggled(false);
    return;

}

/****************************************************************************/
/*!
 *  \brief This function validates the input string from the keyboard.
 */
/****************************************************************************/
void CKeyBoard::ValidateString(QString InputString)
{
   // unsigned int EnteredCharLen = mp_LineEdit->text().length();
    unsigned int EnteredCharLen = InputString.length();
    MainMenu::CMessageDlg MessageDlg(this);
    MessageDlg.SetTitle(tr("Information Message"));
    MessageDlg.SetIcon(QMessageBox::Information);
    MessageDlg.SetButtonText(1, tr("Ok"));
    MessageDlg.HideButtons();

    if(InputString == "" ){
        MessageDlg.SetText(QString(tr("The entered text is null.")));
        MessageDlg.exec();
        m_EnteredCharsValid = false ;
    }
    else if(InputString.at(0) == ' ') {
        MessageDlg.SetText(QString(tr("The first character must not be a space")));
        MessageDlg.exec();
        m_EnteredCharsValid = false ;
    }
    else if (EnteredCharLen < m_MinEnteredCharLen) {
        MessageDlg.SetText(QString(tr("The text you entered is too short. "
                                      "The length must be at least %1.")
                                   .arg(m_MinEnteredCharLen)));
         MessageDlg.exec();
         m_EnteredCharsValid = false ;
    }
    else if (EnteredCharLen > m_MaxEnteredCharLen) {
        MessageDlg.SetText(QString(tr("The text you entered is too long. "
                                      "The length must not be greater than %1.")
                                   .arg(m_MaxEnteredCharLen)));
         MessageDlg.exec();
         m_EnteredCharsValid = false ;
    }
    else {
       switch(int (m_ValidationType)) {
    case 1:
            if(EnteredCharLen >= 2){
                if(InputString.at(1) == ' ') {
                    MessageDlg.SetText(QString(tr("The second element of the text must be a character")));
                    MessageDlg.exec();
                    m_EnteredCharsValid = false ;
                }
                else {
                    m_EnteredCharsValid = true;
                    NotifyObserver();
                }
            }
            else {
                m_EnteredCharsValid = true;
                NotifyObserver();
            }
            break;

    case 2:
        if(m_MaxEnteredCharLen == 3) {
            if(EnteredCharLen == 2){
                if(InputString.at(1) == ' ') {
                    MessageDlg.SetText(QString(tr("The second element of the text must be a character")));
                    MessageDlg.exec();
                    m_EnteredCharsValid = false ;
                } else {
                    m_EnteredCharsValid = true;
                    NotifyObserver();
                }

            } else if(EnteredCharLen == m_MaxEnteredCharLen){
                if(InputString.at(1) == ' ') {
                    MessageDlg.SetText(QString(tr("The second element of the text must be a character")));
                    MessageDlg.exec();
                    m_EnteredCharsValid = false ;
                } else if(InputString.at(2) == ' ') {
                    MessageDlg.SetText(QString(tr("The third element of the text must be a character")));
                    MessageDlg.exec();
                    m_EnteredCharsValid = false ;
                } else {
                    m_EnteredCharsValid = true;
                    NotifyObserver();
                }
            } else {
                m_EnteredCharsValid = true ;
            }
            if(m_EnteredCharsValid) {
                m_EnteredCharsValid = true;
                NotifyObserver();
            }
        }
        else {
            m_EnteredCharsValid = true;
            NotifyObserver();
        }
        break;
    default:
         m_EnteredCharsValid = true;
         NotifyObserver();
        break;
    }
 }
}

} // End of namespace KeyBoard
