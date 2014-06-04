/****************************************************************************/
/*! \file KeyBoard.h
 *
 *  \brief This file contains class for the KeyBoard widget. KeyBoard Widget
 *         provides the following features:
 *          -  1) Support for English & German language
 *          -  2) Support for Subspeller on press and hold of key
 *          -  3) Supports for alphanumeric keys
 *          -  4) Filtering of invalid entries entered by user
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

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "KeyBoard/Include/RoundBGWidget.h"
#include "MainMenu/Include/DialogFrame.h"
#include "MainMenu/Include/MessageDlg.h"
#include <QBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSignalMapper>
#include <QPainter>
#include <QLineEdit>
#include <QVector>
#include <QRegExpValidator>
#include "KeyBoard/Include/KeyBoardButton.h"
namespace KeyBoard {
const int DEFAULT_POSITION_X_COORDINATE = 35; //!< Position of keyboard, value in pixel
const int DEFAULT_POSITION_Y_COORDINATE = 100; //!< Position of keyboard , value in pixel

//!< Forward Declarations
class CSubSpeller;
class CKeyBoardButton;
//!< Used to set resolution depending on whether model is SIZE_1 or SIZE_2
typedef enum {
    SIZE_1 = 1,
    SIZE_2
}Model_t;

//! Normal or German Keyboard layout
typedef enum {
    QWERTY_KEYBOARD = 1,
    QWERTZ_KEYBOARD
}KeyBoardType_t;

//! Enumeration of Button indexes , the order in which they are laid on keyboard.
typedef enum {
    LETTER_Q_INDEX = 1,
    LETTER_W_INDEX,
    LETTER_E_INDEX,
    LETTER_R_INDEX,
    LETTER_T_INDEX,
    LETTER_Y_INDEX,
    LETTER_U_INDEX,
    LETTER_I_INDEX,
    LETTER_O_INDEX,
    LETTER_P_INDEX,
    LETTER_A_INDEX,
    LETTER_S_INDEX,
    LETTER_D_INDEX,
    LETTER_F_INDEX,
    LETTER_G_INDEX,
    LETTER_H_INDEX,
    LETTER_J_INDEX,
    LETTER_K_INDEX,
    LETTER_L_INDEX,
    LETTER_SHIFT_INDEX,
    LETTER_Z_INDEX,
    LETTER_X_INDEX,
    LETTER_C_INDEX,
    LETTER_V_INDEX,
    LETTER_B_INDEX,
    LETTER_N_INDEX,
    LETTER_M_INDEX,
    LETTER_COMMA_INDEX,
    LETTER_DOT_INDEX,
    LETTER_ESC_INDEX,
    LETTER_ABC_INDEX
}LetterIndex_t;

namespace Ui {
    class CKeyBoard;
}

/****************************************************************************/
/**
 * \brief Keyboard widget -Provides user an virtual on-screen Keyboard.
 *        Features : 1) Support for English & German language
 *                   2) Support for Subspeller on press and hold of key
 *                   3) Supports for alphanumeric keys
 *                   4) Filtering of invalid entries entered by user
 */
/****************************************************************************/
class CKeyBoard : public MainMenu::CDialogFrame
{
    Q_OBJECT
    friend class CTestKeyBoard;         //!< Unit test class
private:
    QSignalMapper *mp_ClickedMapper;    //!< Signal mapper element used to map keys from all buttons
    QSignalMapper *mp_PressedMapper;    //!< Signal mapper element used when a button is pressed
    QList <CKeyBoardButton *> m_ButtonList; //!< used to store pointers to all buttons created
    QVBoxLayout *mp_BaseKeyLayout;      //!< Base layout for the keys
    QHBoxLayout *mp_QwertyLayout;       //!< Layout to lay qwerty keys horizontally
    QHBoxLayout *mp_AsdfLayout;         //!< Layout to lay asdf keys horizontally
    QHBoxLayout *mp_ZxcdLayout;         //!< Layout to lay zxcd keys horizontally
    QHBoxLayout *mp_EscLayout;          //!< Layout to lay ESC ... keys horizontally
    QHBoxLayout *mp_LineEditLayout;     //!< Layout to hold the LineEdit widget
    QVBoxLayout *mp_KeyBoardBaseLayout; //!< The Main/Base layout of Keyboard
    qint16 m_ShiftKeyPressCounter;                   //!< count shift button press
    qint16 m_PreviouCount;              //!< used to hold previous count of shift presses
    bool m_IsCaps;                      //!< Check for caps on buttons
    bool m_BtnClicked;                  //!< Check for button press
    bool m_Shift;                       //!< True Shift Clicked
    bool m_NumericMode;                  //!< Check for Alpanumeric key toggle
    bool m_ShiftCaps;                   //!< Set when Shift key sets CAPS
    bool m_SubSpellerClicked;           //!< Set When SubSpeller is clicked;
    QString m_LineEditString;           //!< This will contain the string present in the lineEdit
    QSpacerItem *mp_SpacerQw;           //!< Used to resolve spacing issue between buttons in qwerty row
    QSpacerItem *mp_SpacerAs;           //!< Used to resolve spacing issue between buttons in Asdf row
    QSpacerItem *mp_SpacerZx;           //!< Used to resolve spacing issue between buttons in zxcd row
    QSpacerItem *mp_SpacerEsc;          //!< Used to resolve spacing issue between buttons in the last row
    CSubSpeller *mp_SubSpeller;         //!< Subspeller widget
    KeyBoardType_t m_KeyBoardType;      //!< Used to distuinguish between QWERTY and QWERTZ Keyboard
    Model_t m_Model;                    //!< Used to distuinguish between SIZE_1 and SIZE_2 models
    qint32 m_KeyBoardWidth;             //!< Width of keyboard in pixels
    qint32 m_KeyBoardHeight;            //!< Height of Keyboard in pixels
    qint32 m_LineEditWidth;             //!< Width of lineEdit in pixels
    qint32 m_LineEditHeight;            //!< Height of lineEdit in pixels
    qint32 m_KeyTrayWidth;              //!< Width of KeyTray in pixels
    qint32 m_KeyTrayHeight;             //!< Height of KeyTray in pixels
    qint32 m_LineEditMarginLeft;        //!< Margins for lineEdit Layout
    qint32 m_LineEditMarginTop;         //!< Margins for lineEdit Layout
    qint32 m_LineEditMarginRight;       //!< Margins for lineEdit Layout
    qint32 m_LineEditMarginBottom;      //!< Margins for lineEdit Layout
    qint32 m_KeyTrayMarginLeft;         //!< Margins for KeyTray Layout
    qint32 m_KeyTrayMarginTop;          //!< Margins for KeyTray Layout
    qint32 m_KeyTrayMarginRight;        //!< Margins for KeyTray Layout
    qint32 m_KeyTrayMarginBottom;       //!< Margins for KeyTray Layout
    qint32 m_MainMarginLeft;            //!< Margins for Base Layout
    qint32 m_MainMarginTop;             //!< Margins for Base Layout
    qint32 m_MainMarginRight;           //!< Margins for Base Layout
    qint32 m_MainMarginBottom;          //!< Margins for Base Layout
    qint32 m_SpaceKeyWidth;             //!< Widht of Space Key
    qint32 m_SpecialCharKeyWidth;       //!< Widht of Special Character Key
    qint32 m_OkKeyWidth;                //!< Width of Ok Key
    qint32 m_LineEditWidgetW;           //!< Width of lineEdit widget.
    qint32 m_LineEditWidgetH;           //!< Width of lineEdit Height.
    QString m_IconSize;                 //!< For SIZE_1 "Large" ,SIZE_2 "Small"
    CRoundWidget *mp_KeyTrayBackGround; //!< Widget to hold the keys
    CKeyBoardButton *mp_DeleteBtn;      //!< BackSpace Key
    QLineEdit *mp_LineEdit;             //!< Line Edit Widget to display characters
    QWidget *mp_LineEditWidget;         //!< A Widget to hold the lineEdit and Delete button
    unsigned int m_MaxEnteredCharLen;   //!< Max Character length, Show error mesg if exceeds
    unsigned int m_MinEnteredCharLen;   //!< Min Character lenght, Show error message if exceeds
    bool m_EnteredCharsValid;           //!< True if entered char is valid.
    QString m_EnteredString;            //!< String Entered via keyboard
    bool m_EnteredStringValidation;     //!< True for LongName validation and False for ShortName validation
    QRegExpValidator* mp_RegValidator;  //!< To store the validator
    MainMenu::CMessageDlg *mp_MessageDlg; //!< Message dialog object
    bool m_EnableInternalValidation;              //!< True if entered string from keyborad  has to be validated else False
    bool m_DisplayNumericKeypadAtStart;                 //!< true for numeric mode, false for alphabet mode
    static qint32 m_KeyBoardXPosition; //!< X coordinate of the Keyboard widget
    static qint32 m_KeyBoardYPosition; //!< Y coordinate of the keyboard widget

    CKeyBoardButton *CreateNewKey(QString IconType, QString PrimaryText,
                                  QString BtnText2, bool IconPresent,
                                  int ButtonType);
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CKeyBoard)
    void CreateKeyboard(void);
    void SetCaps(bool IsCaps);
    void MoveCharacters(bool Checked);
    void CreateSubSpeller(int Button);
    void KeyBoardReset();
    void SetKeyBoardType(KeyBoard::KeyBoardType_t m_KeyBoardType);
    void InitSizeAttributes();
    void CreateLineEditWidget();
    void CreateBaseLayout();
    void ConnectButtons();

public:
    explicit CKeyBoard(Model_t Model = SIZE_1,
                       KeyBoardType_t KeyBoardType = QWERTY_KEYBOARD,
                       QWidget *p_Parent = 0,
                       QWidget *pMainWindow = 0);
    ~CKeyBoard();

    void SetPasswordMode(bool Mode);
    void SetKeyBoardDialogTitle(QString Title);
    void SetLineEditValidatorExpression(const QString& RegExpForValidation);
    QString GetLineEditString();
    bool ValidateString(QString InputString);
    void HideKeyBoard();

    /****************************************************************************/
    /*!
     *  \brief Enable or Disable Keyboard's internal validation of entered string
     *         against Max, Min characters set and also of empty string. By
     *         default validation is enabled.
     *
     *  \iparam EnableValidation - true if validation has to be done else false
     */
    /****************************************************************************/
    void SetValidation(bool EnableValidation) { m_EnableInternalValidation = EnableValidation; }

    /****************************************************************************/
    /*!
     *  \brief This function returns if Caps lock is set or not
     *  \return true if keyboard characters are in upper case, else false
     */
    /****************************************************************************/
    bool GetCapsStatus() const
    {
        return m_IsCaps;
    }

    /****************************************************************************/
    /*!
     *  \brief This function returns whether keyboard is alphabet or numeric
     *           mode
     *  \return true if keyboard is in numeric mode, else false for alphabet mode
     */
    /****************************************************************************/
    bool GetDisplayMode() const
    {
        return m_NumericMode;
    }

    /****************************************************************************/
    /*!
     *  \brief This function is used to set content of the the line edit
     *  \iparam  LineEditContent
     */
    /****************************************************************************/
    void SetLineEditContent(QString LineEditContent)
    {
        mp_LineEdit->insert(LineEditContent);
    }

    /****************************************************************************/
    /*!
     *  \brief  Returns pointer to line edit widget
     *  \return mp_LineEdit
     */
    /****************************************************************************/
    QLineEdit *GetLineEditPtr() {return mp_LineEdit;}


    /****************************************************************************/
    /*!
     *  \brief This function is used to set the Max length of characters that should
     *         be entered.
     *
     *  \iparam  MaxCharLen = Maximum length constraint
     */
    /****************************************************************************/
    void SetMaxCharLength(unsigned int MaxCharLen) { m_MaxEnteredCharLen = MaxCharLen;}


    /****************************************************************************/
    /*!
     *  \brief This function is used to set the Min length of characters that should
     *         be entered.
     *
     *  \iparam  MinCharLen = Minimum length constraint
     */
    /****************************************************************************/
    void SetMinCharLength(unsigned int MinCharLen) { m_MinEnteredCharLen = MinCharLen;}


    /****************************************************************************/
    /*!
     *  \brief This function is used to set input mask for the line edit
     *  \iparam  InputMask
     */
    /****************************************************************************/
    void SetLineEditInputMask(const QString& InputMask) {
        mp_LineEdit->setInputMask(InputMask);
    }

    /****************************************************************************/
    /*!
     *  \brief Displays keyboard in Numeric mode. Useful when user has to enter
     *         only numbers.
     */
    /****************************************************************************/
    void DisplayNumericKeyBoard() {
        m_DisplayNumericKeypadAtStart = true;
        //lint -e641
        m_ButtonList.at(LETTER_ABC_INDEX)->SetToggledState(true);
        SetNumericMode(true);
    }

    /****************************************************************************/
    /*!
     *  \brief Move the keyboard to the required position.
     *  \iparam XCoordinate = x coordinate to move to.
     *  \iparam YCoordinate = Y coordinate to move to.
     */
    /****************************************************************************/
    void MoveKeyboard(qint32 XCoordinate, qint32 YCoordinate) {
        m_KeyBoardXPosition =  XCoordinate;
        m_KeyBoardYPosition =  YCoordinate;
        this->move(XCoordinate, YCoordinate);
    }

    /****************************************************************************/
    /*!
     *  \brief Retrieve keyboard position
     *  \xparam XCoordinate = x coordinate of Keyboard
     *  \xparam YCoordinate = Y coordinate of Keyboard
     */
    /****************************************************************************/
    void GetKeyboardCoordinates(qint32 &XCoordinate, qint32 &YCoordinate) {
        XCoordinate = m_KeyBoardXPosition;
        YCoordinate = m_KeyBoardYPosition;
    }

signals:
    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when ESC button on keyboard is clicked.
     */
    /****************************************************************************/
    void EscButtonClicked();
    
	/****************************************************************************/
    /*!
     *  \brief This signal is emitted when Ok button on keyboard is clicked.
     *         
     *  \iparam  EnteredString = Entered string,
     */
    /****************************************************************************/	
	void OkButtonClicked(QString EnteredString);
    
	/****************************************************************************/
    /*!
     *  \brief This signal is emitted when Ok button on keyboard is clicked.
     *
     *  \iparam  EnteredString = Entered string,
     */
    /****************************************************************************/	
	 void ValidateEnteredString(QString EnteredString);

private slots:
    void OnButtonClicked(int Button);
    void SetNumericMode(bool Checked);
    void PressAndHold(int Button);
    void SubSpellerClicked();
    void EscClicked();
    void OnOkClicked();
    void OnValidationComplete(const bool ValidationResult);
    void SwitchLayout(const QString Language);

};

} // End of namespace KeyBoard

#endif //KEYBOARD_H
