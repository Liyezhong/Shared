/****************************************************************************/
/*! \file KeyBoardWidget.cpp
 *
 *  \brief KeyBoard implementation.KeyBoard Widget provides the following
 *          features:
 *          -  1) Support for English & German language
 *          -  2) Support for Subspeller on press and hold of key
 *          -  3) Supports for alphanumeric keys
 *          -  4) Filtering of invalid entries entered by user
 *   \note Throughout the Keyboard component "Large" keyword is used, which
 *          means 800 X 600 resolution, similarly "Small" means 640 X 480
 *          resolution.
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

//lint -e641

namespace KeyBoard {

const int ASCII_Z           = 90; //!< ASCII Code
const int ASCII_z           = 122; //!< ASCII Code
const int ASCII_A           = 65; //!< ASCII Code
const int ASCII_a           = 97; //!< ASCII Code
const int ASCII_COMMA       = 44; //!< ASCII Code
const int ASCII_DOT         = 46; //!< ASCII Code
const int TOTAL_KEYS        = 36; //!< Total keys in keyboard
const int DEGREE_SYMBOL_ASCII_CODE = 176; //!< ASCII Code ISO 8859-15
const QString ESC_ACCESSIBLE_NAME = "0x01000000";   //!< Accessible names according to Qt::Key, refer QT Docs
const QString SPACE_ACCESSIBLE_NAME = "0x20";       //!< Accessible names according to Qt::Key, refer QT Docs
const QString SHIFT_ACCESSIBLE_NAME = "0x01000020"; //!< Accessible names according to Qt::Key, refer QT Docs
const QString OK_ACCESSIBLE_NAME = "0x01000005";    //!< Accessible names according to Qt::Key, refer QT Docs
const QString BACKSPACE_ACCESSIBLE_NAME = "0x01000003"; //!< Accessible names according to Qt::Key, refer QT Docs
const QString ALTERNATE_MODE_KEY_ACCESSIBLE_NAME = "0x01000023"; //!< Accessible names according to Qt::Key, refer QT Docs
const QString CURSOR_RIGHT_ACCESSIBLE_NAME  = "0x01000014";//!< Accessible names according to Qt::Key, refer QT Docs
const QString CURSOR_LEFT_ACCESSIBLE_NAME = "0x01000012";//!< Accessible names according to Qt::Key, refer QT Docs
const int BUTTON_WIDTH = 67; //!<  Width of button in pixel
const int DEFAULT_MAX_LENGTH_OF_CHARS = 32; //!< Default max length of entered characters
const int DEFAULT_MIN_LENGTH_OF_CHARS = 1; //!< Default min length of entered characters
//! \warning changing the below values will have affect on subspeller position.
//! Developer must add/subtract the same to subspeller coordinate if the below values are changed.

const int CURSOR_FLASH_TIME = 0; //!< Cursor doesnt blink. Increase value for blinking
qint32 CKeyBoard::m_KeyBoardXPosition = 35; //!< Default value for SIZE_1
qint32 CKeyBoard::m_KeyBoardYPosition = 100; //! Default value for SIZE_1
/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam Model = SIZE_1 (800 x 600) ,  SIZE_2 ( 640 X 480)
 *  \iparam KeyBoardType = QWERTY/ QWERTZ keyboard
 *  \iparam p_Parent = keyboard's parent
 *  \iparam pMainWindow = the main window
 */
/****************************************************************************/
CKeyBoard::CKeyBoard(KeyBoard::Model_t Model,
                     KeyBoard::KeyBoardType_t KeyBoardType,
                     QWidget *p_Parent,
                     QWidget *pMainWindow)
    : MainMenu::CDialogFrame(p_Parent, pMainWindow),
      mp_ClickedMapper(NULL),
      mp_PressedMapper(NULL),
      mp_BaseKeyLayout(NULL),
      mp_QwertyLayout(NULL),
      mp_AsdfLayout(NULL),
      mp_ZxcdLayout(NULL),
      mp_EscLayout(NULL),
      mp_LineEditLayout(NULL),
      mp_KeyBoardBaseLayout(NULL),
      mp_SpacerQw(NULL),
      mp_SpacerAs(NULL),
      mp_SpacerZx(NULL),
      mp_SpacerEsc(NULL),
      mp_SubSpeller(NULL),
      mp_KeyTrayBackGround(NULL),
      mp_DeleteBtn(NULL),
      mp_LineEdit(NULL),
      mp_RegValidator(NULL),
      mp_MessageDlg(NULL)
{
    m_Model = Model;
    m_KeyBoardType = KeyBoardType;
    m_ShiftKeyPressCounter = 0;
    m_PreviouCount = 0;
    m_IsCaps = false;
    m_BtnClicked = false;
    m_ShiftCaps = false;
    m_SubSpellerClicked = false;
    m_EnteredCharsValid = true;
    m_NumericMode = false;
    m_MaxEnteredCharLen = DEFAULT_MAX_LENGTH_OF_CHARS;
    m_MinEnteredCharLen = DEFAULT_MIN_LENGTH_OF_CHARS;
    m_EnteredStringValidation = true ;
    mp_RegValidator = NULL;
    m_EnableInternalValidation = true;
    m_DisplayNumericKeypadAtStart = false;
    //this->move(DEFAULT_POSITION_X_COORDINATE, DEFAULT_POSITION_Y_COORDINATE);

    setAttribute(Qt::WA_TranslucentBackground);
    //! \warning Don't change order of below function calls.
    InitSizeAttributes();
    CreateLineEditWidget();
    CreateBaseLayout();
    CreateKeyboard();
    ConnectButtons();
    QApplication::setCursorFlashTime(CURSOR_FLASH_TIME);//This stops cursor blinking.
    //lint -e64
    mp_MessageDlg = new MainMenu::CMessageDlg(this); // dialog for error messages

}

 /****************************************************************************/
 /*!
  *  \brief Destructor
  */
 /****************************************************************************/
CKeyBoard::~CKeyBoard()
{
    try {
        delete mp_RegValidator;
        delete mp_EscLayout;
        delete mp_ZxcdLayout;
        delete mp_AsdfLayout;
        delete mp_QwertyLayout;
    }
    catch (...) {
        // to please Lint.
    }
}

/****************************************************************************/
/*!
 *  \brief  Initialize all the Keyboard widget's size attributes. Sets values
 *          according to Keyboard mode - SIZE1 or SIZE 2
 */
/****************************************************************************/
void CKeyBoard::InitSizeAttributes()
{
    if (m_Model == KeyBoard::SIZE_1) {
        //considered additional black spaces in the png's.
        //Values were derived based on measurement using gimp.
        //All are pixel values
        m_KeyBoardWidth = 756;
        m_KeyBoardHeight = 454;
        m_IconSize = "Large";
        GetContentFrame()->setFixedSize(756, 434);
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
        m_OkKeyWidth = 110;
        m_LineEditWidgetW = 715;
        m_LineEditWidgetH = 63;
    }
    else {
        //considered additional black spaces in the png's.
        //Values were derived based on measurement using gimp.
        //All are pixel values
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
        m_OkKeyWidth = 78;
        CSubSpeller::m_SubspellerOffsetW = 25;
        CSubSpeller::m_SubspellerOffsetH = 25;
        CSubSpeller::m_SubspellerBorderMarginLeft = 35;
        CSubSpeller::m_SubspellerBorderMarginTop = 35;
        CSubSpeller::m_SubspellerBorderMarginRight = 35;
        CSubSpeller::m_SubspellerBorderMarginBottom = 35;
        CSubSpeller::m_SubspellerMarginRight = 5;
        m_LineEditWidgetW = 481;
        m_LineEditWidgetH = 67;
    }
}

/****************************************************************************/
/*!
 *  \brief Create and initialize LineEdit Widget- Contains Line Edit box and
 *         a delete button next to it
 */
/****************************************************************************/
void CKeyBoard::CreateLineEditWidget()
{
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

    mp_LineEditLayout->setSpacing((m_Model == SIZE_1) ? 1 : 0);
    mp_DeleteBtn = new CKeyBoardButton(mp_LineEditWidget,
                                        "Right", "Delete", "", true, KeyBoard::BACKSPACE);
    mp_DeleteBtn->setAccessibleName(BACKSPACE_ACCESSIBLE_NAME);
    mp_DeleteBtn->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Fixed);
    mp_LineEditLayout->addWidget(mp_DeleteBtn);
    mp_LineEditWidget->setFixedSize(m_LineEditWidgetW, m_LineEditWidgetH);
    mp_LineEdit->setCursorPosition(0);
}

/****************************************************************************/
/*!
 *  \brief Create and initialize base layout on which other layouts are laid
 */
/****************************************************************************/
void CKeyBoard::CreateBaseLayout()
{
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
}

/****************************************************************************/
/*!
 *  \brief Connects the Button's clicked signals to the required slots
 */
/****************************************************************************/
void CKeyBoard::ConnectButtons()
{
    //Connect the signals from buttons to signal mapper, further to the button clicked
    //slot.
    m_ButtonList = findChildren<CKeyBoardButton *>();
    mp_ClickedMapper = new QSignalMapper(this);
    for (int i = 0;i < m_ButtonList.count(); i++) {
        CONNECTSIGNALSLOT(m_ButtonList.at(i), Buttonclicked(), mp_ClickedMapper, map());
        mp_ClickedMapper->setMapping(m_ButtonList.at(i), i);
    }
    CONNECTSIGNALSLOT(mp_ClickedMapper, mapped(int), this, OnButtonClicked(int));

    mp_PressedMapper = new QSignalMapper(this);
    for (int I = 1; I < (TOTAL_KEYS - 5); I++) { // Subtracting 5 because those keys are directly connected.
           CONNECTSIGNALSLOT(m_ButtonList.at(I), PressAndHold(), mp_PressedMapper, map());
           mp_PressedMapper->setMapping(m_ButtonList.at(I), I);
    }
    CONNECTSIGNALSLOT(mp_PressedMapper, mapped(int), this, PressAndHold(int));
}

/****************************************************************************/
/*!
 *  \brief This function is used to display asterisks when in password mode
 *  \iparam Mode - True to show asterix , else false for normal character display
 */
/****************************************************************************/
void CKeyBoard::SetPasswordMode(bool Mode)
{
    if (Mode) {
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
    m_ShiftKeyPressCounter = 0;
    m_PreviouCount = 0;
    m_IsCaps = false;
    m_BtnClicked = false;
    m_ShiftCaps = false;
    m_SubSpellerClicked = false;
    m_DisplayNumericKeypadAtStart = false;
    m_EnableInternalValidation = true;
    m_EnteredCharsValid = true;
    m_MaxEnteredCharLen = DEFAULT_MAX_LENGTH_OF_CHARS;
    m_MinEnteredCharLen = DEFAULT_MIN_LENGTH_OF_CHARS;
    SetCaps(false);
    mp_LineEdit->clear();
    mp_LineEdit->setInputMask("");
    mp_LineEdit->setValidator(NULL);
}

/****************************************************************************/
/*!
 *  \brief This slot is called if there is a press and hold on a button
 *
 *  \iparam Button =  value of the button
 */
/****************************************************************************/
void CKeyBoard::CreateSubSpeller(int Button)
{
    qint16 Offset = 0;
    qint16 Index = 0;
    bool SubSpeller = false;
    if ((m_SubSpellerClicked == true) && (mp_SubSpeller != 0 )) {
        delete mp_SubSpeller;
        mp_SubSpeller = 0;
    }
    qint16 AsciiVal = m_ButtonList.at(Button)->GetPrimaryText().at(0).toAscii();
    if(m_ShiftCaps == false) {
        //LowerCase
        if (((AsciiVal >= ASCII_a) && (AsciiVal <= ASCII_z))) {
            Offset = 0;
            Index = AsciiVal - ASCII_a;
            SubSpeller = true;
        }
        else if ((AsciiVal == ASCII_COMMA) ||(AsciiVal == ASCII_DOT)) {
            Offset = ENGLISH_ALPHABET_COUNT;
            Index = ((AsciiVal - ASCII_COMMA) == 2) ? 1 :0;
            SubSpeller = true;
        }
    }
    else {
        //Upper case
        if ((AsciiVal >= ASCII_A) && (AsciiVal <= ASCII_Z)) {
            Offset = TOTAL_KEYS_USING_SUBSPELLER;
            Index = AsciiVal - ASCII_A;
            SubSpeller = true;
        }
        else if ((AsciiVal == ASCII_COMMA) ||(AsciiVal == ASCII_DOT)) {
            Offset = (TOTAL_KEYS_USING_SUBSPELLER * 2) - 2; // multiplied by 2 , because of upper and lower case character, subtracted by 2 for "." & ","
            Index = ((AsciiVal - ASCII_COMMA) == 2) ? 1 :0;
            SubSpeller = true;
        }
    }

    if (SubSpeller == true) {
        mp_SubSpeller = new CSubSpeller(this, m_ButtonList.at(Button), Offset, Index, m_Model, m_KeyBoardType);
        mp_SubSpeller->setWindowModality(Qt::ApplicationModal);
        mp_SubSpeller->show();
        CONNECTSIGNALSLOT(mp_SubSpeller, SubSpellerClicked(), this, SubSpellerClicked());
    }
}


/****************************************************************************/
/*!
 *  \brief This slot is called if there is a press and hold on a button
 *
 *  \iparam Button value of the button
 */
/****************************************************************************/
void CKeyBoard::PressAndHold(int Button)
{
    if(!m_NumericMode) {
        CreateSubSpeller(Button);
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
    //if shift was pressed previously & keyboard is not displaying numeric keys
    //then reset the keys back to lower case.
    if ((!m_IsCaps) && (m_Shift == true) && (!m_NumericMode)) {
        SetCaps(false);
        m_ShiftKeyPressCounter = 0;
    }
    m_Shift = false;
}

/****************************************************************************/
/*!
 *  \brief This slot is called when one of the Button is pressed
 *
 *  \iparam Button value of the button
 */
/****************************************************************************/
void CKeyBoard::OnButtonClicked(int Button)
{
    QString StrKeyId = m_ButtonList.at(Button)->accessibleName();
    bool Isok;
    int KeyId = StrKeyId.toInt(&Isok,16);
    QString KeyChar;

    switch (KeyId) {
        default:
            break;
        case Qt::Key_Shift:
            m_ShiftKeyPressCounter++;
            if(m_ShiftKeyPressCounter == 2) {
                //CAPS LOCK
                m_IsCaps = true;
                m_ShiftKeyPressCounter = 0;
                SetCaps(true);
                m_PreviouCount = 2;
            }
            else {
                if ((m_PreviouCount != 2) && (m_PreviouCount !=1)) {
                    SetCaps(!m_IsCaps);
                }
                else if (m_PreviouCount == 2){
                    //No more CAPS LOCK
                    SetCaps(false);
                    m_PreviouCount = m_ShiftKeyPressCounter;
                    m_ShiftKeyPressCounter = 0;
                }
                else {
                    SetCaps(true);
                }
                m_Shift = true;
                m_IsCaps = false;
            }
            return;
        case Qt::Key_Alt:
        case Qt::Key_Escape:
        case Qt::Key_Enter:
        //nothing to display for the above keys
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
        reset:if ((!m_IsCaps) && (m_Shift == true) && (!m_NumericMode)) {
                SetCaps(false);
                m_ShiftKeyPressCounter = 0;
            }
            return;
    }
    //Flow has reached here means its not keyboard modfier like shift/backspace etc it a alphabet/number
    //Check the mode of keyboard, alphabet mode or special char mode
    if(!m_NumericMode) {
        KeyChar = m_ButtonList.at(Button)->GetPrimaryText();
    }
    else {
        KeyChar = m_ButtonList.at(Button)->GetSecondaryText();
    }

    if (KeyId == static_cast<int>(Qt::Key_Space)) {
            KeyChar = " ";
    }
    //Replaced KeyEvent and SendEvent with Insert.Gave better performance
    mp_LineEdit->insert(KeyChar);
    if (m_Shift) {
        if ((!m_IsCaps) && (!m_NumericMode)) {
            SetCaps(false);
            m_ShiftKeyPressCounter = 0;
        }
    }
    m_Shift = false;
}

/****************************************************************************/
/*!
 *  \brief This slot is called either when the "#...@" key is toggled  or when
 *         Keyboard needs to startup with Numeric keypad , e.g. to enter IP
 *         address
 *
 *  \iparam Checked = True for Numeric Keypad, false for Normal Keypad
 */
/****************************************************************************/
void CKeyBoard::SetNumericMode(bool Checked)
{
    static int Count = 0;
    if (m_DisplayNumericKeypadAtStart && (!Count)) {
        Count++;
        m_ButtonList.at(LETTER_ABC_INDEX)->SetToggledState(true);
    }
    m_NumericMode = Checked;
    if (m_NumericMode) {
        // Added below check to change keys from uppercase to lower case incase
        // "Shift" key was pressed before "#...@".
        if (m_ShiftKeyPressCounter == 1) {
            m_ShiftKeyPressCounter = 0;
            SetCaps(false);
        }
        m_ButtonList.at(LETTER_SHIFT_INDEX)->setAccessibleName("0x60");
        m_ButtonList.at(LETTER_ESC_INDEX)->setAccessibleName("0x1");//dummy name for ESC
    }
    else {
        //Shift Key
        m_ButtonList.at(LETTER_SHIFT_INDEX)->setAccessibleName(SHIFT_ACCESSIBLE_NAME);
        m_ButtonList.at(LETTER_ESC_INDEX)->setAccessibleName(ESC_ACCESSIBLE_NAME);
    }
    MoveCharacters(m_NumericMode);
    for (qint16 X = 1;X < 32 ; X++) {
        m_ButtonList.at(X)->SetAlphabetMode(!m_NumericMode);
        m_ButtonList.at(X)->update();
    }
}

/****************************************************************************/
/*!
 *  \brief This function is called to  set Button labels to upper case
 *
 *  \iparam IsCaps = true- make characters upper case, else lower case
 */
/****************************************************************************/
void CKeyBoard::SetCaps(bool IsCaps) {
    QString DegreeSymbolString = QChar(DEGREE_SYMBOL_ASCII_CODE);
    if (IsCaps == true) {
        m_ShiftCaps = true;
        m_ButtonList.at(LETTER_Q_INDEX)->SetText("Q", "1");
        m_ButtonList.at(LETTER_W_INDEX)->SetText("W", "2");
        m_ButtonList.at(LETTER_E_INDEX)->SetText("E", "3");
        m_ButtonList.at(LETTER_R_INDEX)->SetText("R", "4");
        m_ButtonList.at(LETTER_T_INDEX)->SetText("T", "5");
        if (m_KeyBoardType == QWERTY_KEYBOARD) {
            m_ButtonList.at(LETTER_Y_INDEX)->SetText("Y", "6");
        }
        else {
            m_ButtonList.at(LETTER_Y_INDEX)->SetText("Z", "6");
        }
        m_ButtonList.at(LETTER_U_INDEX)->SetText("U", "7");
        m_ButtonList.at(LETTER_I_INDEX)->SetText("I", "8");
        m_ButtonList.at(LETTER_O_INDEX)->SetText("O", "9");
        m_ButtonList.at(LETTER_P_INDEX)->SetText("P", "0");
        m_ButtonList.at(LETTER_A_INDEX)->SetText("A", "!");
        m_ButtonList.at(LETTER_S_INDEX)->SetText("S", "@");
        m_ButtonList.at(LETTER_D_INDEX)->SetText("D", "#");
        m_ButtonList.at(LETTER_F_INDEX)->SetText("F", "$");
        m_ButtonList.at(LETTER_G_INDEX)->SetText("G", "%");
        m_ButtonList.at(LETTER_H_INDEX)->SetText("H", "&");
        m_ButtonList.at(LETTER_J_INDEX)->SetText("J", "*");
        m_ButtonList.at(LETTER_K_INDEX)->SetText("K", "?");
        m_ButtonList.at(LETTER_L_INDEX)->SetText("L", "/");
        if (m_KeyBoardType == QWERTY_KEYBOARD) {
            m_ButtonList.at(LETTER_Z_INDEX)->SetText("Z", "~");
        }
        else {
            m_ButtonList.at(LETTER_Z_INDEX)->SetText("Y", "~");
        }
        m_ButtonList.at(LETTER_X_INDEX)->SetText("X", "\"");
        m_ButtonList.at(LETTER_C_INDEX)->SetText("C", DegreeSymbolString);
        m_ButtonList.at(LETTER_V_INDEX)->SetText("V", "(");
        m_ButtonList.at(LETTER_B_INDEX)->SetText("B", ")");
        m_ButtonList.at(LETTER_N_INDEX)->SetText("N", "-");
        m_ButtonList.at(LETTER_M_INDEX)->SetText("M", "_");

    }
    else {
        m_ShiftCaps = false;
        m_ButtonList.at(LETTER_Q_INDEX)->SetText("q", "1");
        m_ButtonList.at(LETTER_W_INDEX)->SetText("w", "2");
        m_ButtonList.at(LETTER_E_INDEX)->SetText("e", "3");
        m_ButtonList.at(LETTER_R_INDEX)->SetText("r", "4");
        m_ButtonList.at(LETTER_T_INDEX)->SetText("t", "5");
        if (m_KeyBoardType == QWERTY_KEYBOARD) {
            m_ButtonList.at(LETTER_Y_INDEX)->SetText("y", "6");
        }
        else {
            m_ButtonList.at(LETTER_Y_INDEX)->SetText("z", "6");
        }
        m_ButtonList.at(LETTER_U_INDEX)->SetText("u", "7");
        m_ButtonList.at(LETTER_I_INDEX)->SetText("i", "8");
        m_ButtonList.at(LETTER_O_INDEX)->SetText("o", "9");
        m_ButtonList.at(LETTER_P_INDEX)->SetText("p", "0");
        m_ButtonList.at(LETTER_A_INDEX)->SetText("a", "!");
        m_ButtonList.at(LETTER_S_INDEX)->SetText("s", "@");
        m_ButtonList.at(LETTER_D_INDEX)->SetText("d", "#");
        m_ButtonList.at(LETTER_F_INDEX)->SetText("f", "$");
        m_ButtonList.at(LETTER_G_INDEX)->SetText("g", "%");
        m_ButtonList.at(LETTER_H_INDEX)->SetText("h", "&");
        m_ButtonList.at(LETTER_J_INDEX)->SetText("j", "*");
        m_ButtonList.at(LETTER_K_INDEX)->SetText("k", "?");
        m_ButtonList.at(LETTER_L_INDEX)->SetText("l", "/");
        if (m_KeyBoardType == QWERTY_KEYBOARD) {
            m_ButtonList.at(LETTER_Z_INDEX)->SetText("z", "~");
        }
        else {
            m_ButtonList.at(LETTER_Z_INDEX)->SetText("y", "~");
        }
        m_ButtonList.at(LETTER_X_INDEX)->SetText("x", "\"");
        m_ButtonList.at(LETTER_C_INDEX)->SetText("c", DegreeSymbolString);
        m_ButtonList.at(LETTER_V_INDEX)->SetText("v", "(");
        m_ButtonList.at(LETTER_B_INDEX)->SetText("b", ")");
        m_ButtonList.at(LETTER_N_INDEX)->SetText("n", "-");
        m_ButtonList.at(LETTER_M_INDEX)->SetText("m", "_");
    }

    for(qint16 X = 1;X < 32 ; X++) {
        m_ButtonList.at(X)->update();
    }
}

/****************************************************************************/
/*!
 *  \brief This function sets KeyBoard Type
 *
 *  \iparam KeyBoardType = QWERTY/QWERTZ KeyBoard.
 */
/****************************************************************************/
void CKeyBoard::SetKeyBoardType(KeyBoard::KeyBoardType_t KeyBoardType)
{
    //Remove below line after testing.This should be done in changeevent
    m_KeyBoardType = KeyBoardType ;
    if (KeyBoardType == QWERTY_KEYBOARD ) {
        if (m_IsCaps || m_ShiftCaps) {
            m_ButtonList.at(LETTER_Y_INDEX)->SetText("Y", "6");
            m_ButtonList.at(LETTER_Z_INDEX)->SetText("Z", "~");
        }
        else {
            m_ButtonList.at(LETTER_Y_INDEX)->SetText("y", "6");
            m_ButtonList.at(LETTER_Z_INDEX)->SetText("z", "~");
        }
    }
    else {
        //QWERTZ_KEYBOARD
        if (m_IsCaps || m_ShiftCaps) {
            m_ButtonList.at(LETTER_Y_INDEX)->SetText("Z", "6");
            m_ButtonList.at(LETTER_Z_INDEX)->SetText("Y", "~");
        }
        else {
            m_ButtonList.at(LETTER_Y_INDEX)->SetText("z", "6");
            m_ButtonList.at(LETTER_Z_INDEX)->SetText("y", "~");
        }
    }
    m_ButtonList.at(LETTER_Y_INDEX)->update();
    m_ButtonList.at(LETTER_Z_INDEX)->update();
}

/****************************************************************************/
/*!
 *  \brief This function moves button text by one button left or right
 *
 *  \iparam Checked - if true move characters
 */
/****************************************************************************/
void CKeyBoard::MoveCharacters(bool Checked) {
    QString DegreeSymbolString = QChar(DEGREE_SYMBOL_ASCII_CODE);
    if (Checked) {
        m_ButtonList.at(LETTER_SHIFT_INDEX)->SetText("Shift", "~");
        if (m_KeyBoardType == QWERTY_KEYBOARD) {
            m_ButtonList.at(LETTER_Z_INDEX)->SetText("Z", "\"");
        }
        else {
            m_ButtonList.at(LETTER_Z_INDEX)->SetText("Y", "\"");
        }
        m_ButtonList.at(LETTER_X_INDEX)->SetText("X", DegreeSymbolString);
        m_ButtonList.at(LETTER_C_INDEX)->SetText("C", "(");
        m_ButtonList.at(LETTER_V_INDEX)->SetText("V", ")");
        m_ButtonList.at(LETTER_B_INDEX)->SetText("B", "-");
        m_ButtonList.at(LETTER_N_INDEX)->SetText("N", "_");
        m_ButtonList.at(LETTER_M_INDEX)->SetText("M", ";");
        m_ButtonList.at(LETTER_COMMA_INDEX)->SetText(",", ":");
        m_ButtonList.at(LETTER_DOT_INDEX)->SetText(".", ",");
        m_ButtonList.at(LETTER_ESC_INDEX)->SetText("", ".");
    }
    else {
        if (m_ShiftCaps) {
            m_ButtonList.at(LETTER_SHIFT_INDEX)->SetText("Shift", "");
            if (m_KeyBoardType == QWERTY_KEYBOARD) {
                m_ButtonList.at(LETTER_Z_INDEX)->SetText("Z", "~");
            }
            else {
                m_ButtonList.at(LETTER_Z_INDEX)->SetText("Y", "~");
            }
            m_ButtonList.at(LETTER_X_INDEX)->SetText("X", "\"");
            m_ButtonList.at(LETTER_C_INDEX)->SetText("C", DegreeSymbolString);
            m_ButtonList.at(LETTER_V_INDEX)->SetText("V", "(");
            m_ButtonList.at(LETTER_B_INDEX)->SetText("B", ")");
            m_ButtonList.at(LETTER_N_INDEX)->SetText("N", "-");
            m_ButtonList.at(LETTER_M_INDEX)->SetText("M", "_");
            m_ButtonList.at(LETTER_COMMA_INDEX)->SetText(",", ";");
            m_ButtonList.at(LETTER_DOT_INDEX)->SetText(".", ":");
            m_ButtonList.at(LETTER_ESC_INDEX)->SetText("ESC", "");
        }
        else {
            m_ButtonList.at(LETTER_SHIFT_INDEX)->SetText("Shift", "");
            if (m_KeyBoardType == QWERTY_KEYBOARD) {
                m_ButtonList.at(LETTER_Z_INDEX)->SetText("z", "~");
            }
            else {
                m_ButtonList.at(LETTER_Z_INDEX)->SetText("y", "~");
            }
            m_ButtonList.at(LETTER_X_INDEX)->SetText("x", "\"");
            m_ButtonList.at(LETTER_C_INDEX)->SetText("c", DegreeSymbolString);
            m_ButtonList.at(LETTER_V_INDEX)->SetText("v", "(");
            m_ButtonList.at(LETTER_B_INDEX)->SetText("b", ")");
            m_ButtonList.at(LETTER_N_INDEX)->SetText("n", "-");
            m_ButtonList.at(LETTER_M_INDEX)->SetText("m", "_");
            m_ButtonList.at(LETTER_COMMA_INDEX)->SetText(",", ";");
            m_ButtonList.at(LETTER_DOT_INDEX)->SetText(".", ":");
            m_ButtonList.at(LETTER_ESC_INDEX)->SetText("ESC", "");
        }
    }
}

/****************************************************************************/
/*!
 *  \brief This function returns the string present in lineEdit
 *  \return Returns string entered by user
 */
/****************************************************************************/
QString CKeyBoard::GetLineEditString()
{
    // Added simplified method to remove white spaces in the entered text.
    m_LineEditString = mp_LineEdit->text().simplified();
    return m_LineEditString;
}

/****************************************************************************/
/*!
 *  \brief This function hides the keyboard when Esc is clicked
 */
/****************************************************************************/
void CKeyBoard::EscClicked()
{
    if(m_ButtonList.at(LETTER_ESC_INDEX)->accessibleName() == ESC_ACCESSIBLE_NAME) {
       hide();
       emit EscButtonClicked();
       KeyBoardReset();
       SetNumericMode(false);
    }
}

/****************************************************************************/
/*!
 *  \brief This function sets the dialog title of the keyboard
 *  \iparam Title = Dialog title of the keyboard
 */
/****************************************************************************/
void CKeyBoard::SetKeyBoardDialogTitle(QString Title)
{
    SetDialogTitle(QString("%1").arg(Title));
}

/****************************************************************************/
/*!
 *  \brief This function is called to create a new key on the keyboard
 *
 *  \iparam IconType Left/Center/Right
 *  \iparam PrimaryText Bottom left text on button
 *  \iparam SecondaryText Top left text on button
 *  \iparam IconPresent true -icon is present
 *  \iparam ButtonType
 *  \return Created button
 */
/****************************************************************************/
CKeyBoardButton *CKeyBoard::CreateNewKey(QString IconType,
                                         QString PrimaryText,
                                         QString SecondaryText,
                                         bool IconPresent,
                                         int ButtonType)
{
    //Memory management taken care by Qt Parent child mechanism. when KeyboardWidget
    // is deleted all Button objects are deleted
    return  (new CKeyBoardButton(this, IconType, PrimaryText, SecondaryText, IconPresent, static_cast<KeyBoard::ButtonType_t>(ButtonType)));
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
    mp_QwertyLayout->addWidget(CreateNewKey("Left", "q", "1", false, KeyBoard::NORMAL_KEY), 1);
    mp_QwertyLayout->addWidget(CreateNewKey("Center", "w", "2", false, KeyBoard::NORMAL_KEY), 1);
    mp_QwertyLayout->addWidget(CreateNewKey("Center", "e", "3", false, KeyBoard::NORMAL_KEY), 1);
    mp_QwertyLayout->addWidget(CreateNewKey("Center", "r", "4", false, KeyBoard::NORMAL_KEY), 1);
    mp_QwertyLayout->addWidget(CreateNewKey("Center", "t", "5", false, KeyBoard::NORMAL_KEY), 1);

    if(m_KeyBoardType == QWERTY_KEYBOARD) {
        mp_QwertyLayout->addWidget(CreateNewKey("Center", "y", "6",false, KeyBoard::NORMAL_KEY), 1);
    }
    else {
        mp_QwertyLayout->addWidget(CreateNewKey("Center", "z", "6",false, KeyBoard::NORMAL_KEY), 1);
    }
    mp_QwertyLayout->addWidget(CreateNewKey("Center", "u", "7", false, KeyBoard::NORMAL_KEY), 1);
    mp_QwertyLayout->addWidget(CreateNewKey("Center", "i", "8", false, KeyBoard::NORMAL_KEY), 1);
    mp_QwertyLayout->addWidget(CreateNewKey("Center", "o", "9", false, KeyBoard::NORMAL_KEY), 1);
    mp_QwertyLayout->addWidget(CreateNewKey("Right", "p", "0", false, KeyBoard::NORMAL_KEY), 1);


    mp_AsdfLayout->addStretch(1);
    mp_AsdfLayout->setSpacing(0);

    mp_AsdfLayout->addWidget(CreateNewKey("Left", "a", "!", false, KeyBoard::NORMAL_KEY), 1);
    mp_AsdfLayout->addWidget(CreateNewKey("Center", "s",  "@", false, KeyBoard::NORMAL_KEY), 1);
    mp_AsdfLayout->addWidget(CreateNewKey("Center", "d", "#", false, KeyBoard::NORMAL_KEY), 1);
    mp_AsdfLayout->addWidget(CreateNewKey("Center", "f", "$", false, KeyBoard::NORMAL_KEY), 1);
    mp_AsdfLayout->addWidget(CreateNewKey("Center", "g", "%", false, KeyBoard::NORMAL_KEY), 1);
    mp_AsdfLayout->addWidget(CreateNewKey("Center", "h", "&", false, KeyBoard::NORMAL_KEY), 1);
    mp_AsdfLayout->addWidget(CreateNewKey("Center", "j", "*", false, KeyBoard::NORMAL_KEY), 1);
    mp_AsdfLayout->addWidget(CreateNewKey("Center", "k", "?", false, KeyBoard::NORMAL_KEY), 1);
    mp_AsdfLayout->addWidget(CreateNewKey("Right", "l", "/", false, KeyBoard::NORMAL_KEY), 1);
    mp_AsdfLayout->addStretch(1);

    mp_ZxcdLayout->setSpacing(0);
    CKeyBoardButton *p_Button = NULL;
    p_Button = CreateNewKey("Left", "Shift", "~", true, KeyBoard::SHIFT_LOCK);
    mp_ZxcdLayout->addWidget(p_Button, 0);
    //Accessible names set according to  enum Qt::Key.Refer qt docs
    p_Button->setAccessibleName(SHIFT_ACCESSIBLE_NAME);
    if (m_KeyBoardType == QWERTY_KEYBOARD) {
        mp_ZxcdLayout->addWidget(CreateNewKey("Center", "z", "~", false, KeyBoard::NORMAL_KEY), 0);
    }
    else {
        mp_ZxcdLayout->addWidget(CreateNewKey("Center", "y", "~", false, KeyBoard::NORMAL_KEY), 0);
    }

    mp_ZxcdLayout->addWidget(CreateNewKey("Center", "x", "\"", false, KeyBoard::NORMAL_KEY), 0);
    QString String = QChar(DEGREE_SYMBOL_ASCII_CODE);//ASCII for degree sign
    mp_ZxcdLayout->addWidget(CreateNewKey("Center","c", String, false, KeyBoard::NORMAL_KEY), 0);
    mp_ZxcdLayout->addWidget(CreateNewKey("Center", "v", "(", false, KeyBoard::NORMAL_KEY), 0);
    mp_ZxcdLayout->addWidget(CreateNewKey("Center", "b", ")", false, KeyBoard::NORMAL_KEY), 0);
    mp_ZxcdLayout->addWidget(CreateNewKey("Center", "n", "-", false, KeyBoard::NORMAL_KEY), 0);
    mp_ZxcdLayout->addWidget(CreateNewKey("Center", "m", "_", false, KeyBoard::NORMAL_KEY), 0);
    mp_ZxcdLayout->addWidget(CreateNewKey("Center", ",", ";", false, KeyBoard::NORMAL_KEY), 0);
    mp_ZxcdLayout->addWidget(CreateNewKey("Right", ".", ":", false, KeyBoard::NORMAL_KEY), 0);


    mp_EscLayout->setSpacing(0);
    p_Button = CreateNewKey("Left", "ESC", "", false, KeyBoard::ESC);
    p_Button->setAccessibleName(ESC_ACCESSIBLE_NAME);
    if (m_Model == KeyBoard::SIZE_2) {
        p_Button->setMinimumWidth(68);
        p_Button->setMaximumWidth(68);
    }
    mp_EscLayout->addWidget(p_Button);
    CONNECTSIGNALSLOT(p_Button, Buttonclicked(),this,EscClicked());
    p_Button = CreateNewKey("Center", "#...@", "ABC", false, KeyBoard::SPECIAL_CHAR);
    p_Button->setAccessibleName(ALTERNATE_MODE_KEY_ACCESSIBLE_NAME);
    p_Button->setCheckable(true);
    p_Button->setMaximumWidth(m_SpecialCharKeyWidth);
    p_Button->setMinimumWidth(m_SpecialCharKeyWidth);
    CONNECTSIGNALSLOT(p_Button, toggled(bool), this, SetNumericMode(bool));
    mp_EscLayout->addWidget(p_Button);


    p_Button = CreateNewKey("Center", "", "", false, KeyBoard::SPACE);
    p_Button->setMaximumWidth(m_SpaceKeyWidth);
    p_Button->setMinimumWidth(m_SpaceKeyWidth);
    p_Button->setAccessibleName(SPACE_ACCESSIBLE_NAME);
    mp_EscLayout->addWidget(p_Button);

    p_Button = CreateNewKey("Center", "CursorLeft", "", true, KeyBoard::CURSOR_LEFT);
    p_Button->setAccessibleName(CURSOR_LEFT_ACCESSIBLE_NAME);
    if (m_Model == KeyBoard::SIZE_2) {
       p_Button->setMinimumWidth(BUTTON_WIDTH);
       p_Button->setMaximumWidth(BUTTON_WIDTH);
    }
    mp_EscLayout->addWidget(p_Button);
    p_Button = CreateNewKey("Center", "CursorRight", "", true, KeyBoard::CURSOR_RIGHT);
    p_Button->setAccessibleName(CURSOR_RIGHT_ACCESSIBLE_NAME);
    if (m_Model == KeyBoard::SIZE_2) {
       p_Button->setMinimumWidth(BUTTON_WIDTH);
       p_Button->setMaximumWidth(BUTTON_WIDTH);
       mp_DeleteBtn->setFixedSize(78, 71);
       mp_QwertyLayout->setContentsMargins(1, 0, 4, 0);
       mp_ZxcdLayout->setContentsMargins(1, 0, 4, 0);
       mp_EscLayout->setContentsMargins(1, 0, 2, 0);
       mp_AsdfLayout->setContentsMargins(1, 0, 13, 0);
       mp_BaseKeyLayout->setSpacing(6);
    }
    mp_EscLayout->addWidget(p_Button);

    p_Button = CreateNewKey("Right", "OK", "", false, KeyBoard::OK);
    //Have set accessible name of "enter"
    p_Button->setAccessibleName(OK_ACCESSIBLE_NAME);
    CONNECTSIGNALSLOT(p_Button, Buttonclicked(), this, OnOkClicked());
    p_Button->setMinimumWidth(m_OkKeyWidth);
    p_Button->setMaximumWidth(m_OkKeyWidth);
    mp_EscLayout->addWidget(p_Button);

    if (m_Model ==  KeyBoard::SIZE_1) {
        mp_QwertyLayout->setContentsMargins(0, 0, 7, 0);
        mp_ZxcdLayout->setContentsMargins(0, 0, 7,0);
        mp_EscLayout->setContentsMargins(0, 0, 7, 0);
        mp_AsdfLayout->setContentsMargins(0, 0, 24, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief This function is called when OK is clicked
 */
/****************************************************************************/
void CKeyBoard::OnOkClicked()
{
    QString EnteredText = mp_LineEdit->text();
    if (m_EnableInternalValidation) {
        m_EnteredCharsValid = ValidateString(EnteredText);
        if (!m_EnteredCharsValid) {
            mp_LineEdit->setSelection(0, EnteredText.length());
            return;
        }
    }
    //the flow has reached here means validation success or internal validation was disabled
    //Call widget specific validation. The below emit signal would act as a call back function.
    //If widget(which called keyboard) has implemented a validation function, that would be executed. The widget,on
    //completion of validation must set m_EnteredCharsValid to true or false based on validation result.
    emit ValidateEnteredString(EnteredText);

    //m_EnteredCharsValid is true by default. If there is no widget specific Validation, then its assumed that
    //string entered by user is valid .
    if (m_EnteredCharsValid) {
        //Hide & Reset Keyboard
        hide();
        emit OkButtonClicked(EnteredText);
        m_ButtonList.at(LETTER_ABC_INDEX)->SetToggledState(false);
        KeyBoardReset();
        SetNumericMode(false);
    }
    else {
        mp_LineEdit->setSelection(0, EnteredText.length());
    }
}

/****************************************************************************/
/*!
 *  \brief This function validates the input string from the keyboard.
 *
 *  \iparam InputString = Entered String from keyboard.
 *  \return true if validation success , else false
 */
/****************************************************************************/
bool CKeyBoard::ValidateString(QString InputString)
{
    unsigned int EnteredCharLen = InputString.length();
    if (mp_MessageDlg) {
        delete mp_MessageDlg;
        mp_MessageDlg = NULL;
    }
    mp_MessageDlg = new MainMenu::CMessageDlg(this);
    mp_MessageDlg->SetTitle(QApplication::translate("KeyBoard::CKeyBoard", "Information Message",
                                                 0, QApplication::UnicodeUTF8));
    mp_MessageDlg->SetIcon(QMessageBox::Information);
   mp_MessageDlg->SetButtonText(1, QApplication::translate("KeyBoard::CKeyBoard", "Ok",
                                                           0, QApplication::UnicodeUTF8));
    mp_MessageDlg->HideButtons();

    //Emtpy string not allowed
    if (InputString == "") {
        mp_MessageDlg->SetText(QApplication::translate("KeyBoard::CKeyBoard", "Please enter text",
                                                     0, QApplication::UnicodeUTF8));
        (void) mp_MessageDlg->exec();
        return false;
    }
    else if (EnteredCharLen < m_MinEnteredCharLen) {
        mp_MessageDlg->SetText(QApplication::translate("KeyBoard::CKeyBoard",
        "The entered text is too short. The length must be at least %1 characters.",
                        0, QApplication::UnicodeUTF8).arg(m_MinEnteredCharLen));
         (void) mp_MessageDlg->exec();
         return false;
    }
    else if (EnteredCharLen > m_MaxEnteredCharLen) {
        mp_MessageDlg->SetText(QApplication::translate("KeyBoard::CKeyBoard",
        "The entered text is too long. The length must not be greater than %1 characters.",
        0, QApplication::UnicodeUTF8).arg(m_MaxEnteredCharLen));
         (void) mp_MessageDlg->exec();
         return false;
    }

    return true;

}

/****************************************************************************/
/*!
 *  \brief This slot is called when validation of string is completed.
 *
 *  \iparam ValidationResult =  True if validation is success else False.
 */
/****************************************************************************/
void CKeyBoard::OnValidationComplete(const bool ValidationResult)
{
    if (ValidationResult == true) {
        m_EnteredCharsValid = true;
    }
    else {
        m_EnteredCharsValid = false;
    }
}

/****************************************************************************/
/*!
 *  \brief This function is used to swithc between QWERTZ and QWERTY layout
 *         based on layout change.
 *  \iparam Language = e.g. "de" or "en"
 */
/****************************************************************************/
void CKeyBoard::SwitchLayout(const QString Language)
{
    if (Language == "de") {
        SetKeyBoardType(QWERTZ_KEYBOARD);
    }
    else {
        SetKeyBoardType(QWERTY_KEYBOARD);
    }
}

/****************************************************************************/
/*!
 *  \brief Set a regualar expression for validating entered text. Keyboard
 *         will accept characters which match regular expression.
 *  \iparam  RegExpForValidation = regular expression for validation
 */
/****************************************************************************/
void CKeyBoard::SetLineEditValidatorExpression(const QString& RegExpForValidation) {
    // "^[0-9]*$'
    // ^ and $ is used for any character. * is used to enter multiple characters
    // [0-9] is used to allow user to enter only 0 to 9 digits

    // check whether validator is created or not
    if (mp_RegValidator == NULL) {
        mp_RegValidator = new QRegExpValidator(QRegExp(RegExpForValidation), this);
    }
    else {
        mp_RegValidator->setRegExp(QRegExp(RegExpForValidation));
    }
    mp_LineEdit->setValidator(mp_RegValidator);
}

/****************************************************************************/
/*!
 *  \brief Hides the keyboard.
 *
 */
/****************************************************************************/
void CKeyBoard::HideKeyBoard() {

    if (mp_SubSpeller) {
        mp_SubSpeller->SetSubSpellerOff();
        delete mp_SubSpeller;
        mp_SubSpeller = NULL;
    }
    SubSpellerClicked();
    KeyBoardReset();
    hide();
}

} // End of namespace KeyBoard
