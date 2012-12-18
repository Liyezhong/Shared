/****************************************************************************/
/*! \file KeyBoard.h
 *
 *  \brief Onscreen Keyboard definition. It also acts as an observable class or
           concrete subject class of the observer design pattern. Note-Only one
           Observer must be registered/attached at a time.
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
#include "KeyBoard/Include/KeyBoardObserver.h"
#include "MainMenu/Include/DialogFrame.h"
#include "MainMenu//Include/MessageDlg.h"
#include <QBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSignalMapper>
#include <QPainter>
#include <QLineEdit>
#include <QVector>

namespace KeyBoard {
//! < Forward Declarations
class CSubSpeller;
class CKeyBoardButton;
//! < Used to set resolution depending on whether model is SIZE_1 or SIZE_2
//eg. Colorado SIZE_1 , Sepia SIZE_2
typedef enum {
    SIZE_1 = 1,
    SIZE_2,
}Model_t;

//! < Normal or German Keyboard layout
typedef enum {
    QWERTY_KEYBOARD = 1,
    QWERTZ_KEYBOARD,
}KeyBoardType_t;

//! < Type of widget using Keyboard
typedef enum {
    EDIT_PROGRAM = 1,
    EDIT_REAGENT,
    USER_SETTINGS,
}WidgetType_t;

//! < Type of input from the Keyboard
typedef enum {
    VALIDATION_1 = 1,
    VALIDATION_2,
    VALIDATION_3,
}ValidationType_t;

namespace Ui {
    class CKeyBoard;
}

/****************************************************************************/
/**
 * \brief Keyboard widget -Provides user an virtual on-screen Keyboard.
 */
/****************************************************************************/
class CKeyBoard : public MainMenu::CDialogFrame
{
    Q_OBJECT
    friend class CTestKeyBoard;
private:
    QSignalMapper *mp_ClickedMapper;    //! < Signal mapper element used to map keys from all buttons
    QSignalMapper *mp_PressedMapper;    //! < Signal mapper element used when a button is pressed
    QList <CKeyBoardButton *> m_ButtonList; //! < used to store pointers to all buttons created
    QVBoxLayout *mp_BaseKeyLayout;      //! < Base layout for the keys
    QHBoxLayout *mp_QwertyLayout;       //! < Layout to lay qwerty keys horizontally
    QHBoxLayout *mp_AsdfLayout;         //! < Layout to lay asdf keys horizontally
    QHBoxLayout *mp_ZxcdLayout;         //! < Layout to lay zxcd keys horizontally
    QHBoxLayout *mp_EscLayout;          //! < Layout to lay ESC ... keys horizontally
    QHBoxLayout *mp_LineEditLayout;     //! < Layout to hold the LineEdit widget
    QVBoxLayout *mp_KeyBoardBaseLayout; //! < The Main/Base layout of Keyboard
    qint16 m_Counter;                   //! < count shift button press
    qint16 m_PreviouCount;              //! < used to hold previous count of shift presses
    bool m_IsCaps;                      //! < Check for caps on buttons
    bool m_BtnClicked;                  //! < Check for button press
    bool m_Shift;                       //! < True Shift Clicked
    bool m_AltToggled;                  //! < Check for Alpanumeric key toggle
    bool m_ShiftCaps;                   //! < Set when Shift key sets CAPS
    bool m_SubSpellerClicked;           //! < Set When SubSpeller is clicked;
    QString m_LineEditString;           //! < This will contain the string present in the lineEdit
    QSpacerItem *mp_SpacerQw;           //! < Used to resolve spacing issue between buttons in qwerty row
    QSpacerItem *mp_SpacerAs;           //! < Used to resolve spacing issue between buttons in Asdf row
    QSpacerItem *mp_SpacerZx;           //! < Used to resolve spacing issue between buttons in zxcd row
    QSpacerItem *mp_SpacerEsc;          //! < Used to resolve spacing issue between buttons in the last row
    CSubSpeller *mp_SubSpeller;         //! < Subspeller widget
    KeyBoardType_t m_KeyBoardType;      //! < Used to distuinguish between QWERTY and QWERTZ Keyboard
    WidgetType_t m_ParentWidgetType;    //! < Used to distuinguis the Widget  using the Keyboard
    Model_t m_Model;                    //! < Used to distuinguish between SIZE_1 and SIZE_2 models
    qint32 m_KeyBoardWidth;             //! < Width of keyboard in pixels
    qint32 m_KeyBoardHeight;            //! < Height of Keyboard in pixels
    qint32 m_LineEditWidth;             //! < Width of lineEdit in pixels
    qint32 m_LineEditHeight;            //! < Height of lineEdit in pixels
    qint32 m_KeyTrayWidth;              //! < Width of KeyTray in pixels
    qint32 m_KeyTrayHeight;             //! < Height of KeyTray in pixels
    qint32 m_LineEditMarginLeft;        //! < Margins for lineEdit Layout
    qint32 m_LineEditMarginTop;         //! < Margins for lineEdit Layout
    qint32 m_LineEditMarginRight;       //! < Margins for lineEdit Layout
    qint32 m_LineEditMarginBottom;      //! < Margins for lineEdit Layout
    qint32 m_KeyTrayMarginLeft;         //! < Margins for KeyTray Layout
    qint32 m_KeyTrayMarginTop;          //! < Margins for KeyTray Layout
    qint32 m_KeyTrayMarginRight;        //! < Margins for KeyTray Layout
    qint32 m_KeyTrayMarginBottom;       //! < Margins for KeyTray Layout
    qint32 m_MainMarginLeft;            //! < Margins for Base Layout
    qint32 m_MainMarginTop;             //! < Margins for Base Layout
    qint32 m_MainMarginRight;           //! < Margins for Base Layout
    qint32 m_MainMarginBottom;          //! < Margins for Base Layout
    qint32 m_SpaceKeyWidth;             //! < Widht of Space Key
    qint32 m_SpecialCharKeyWidth;       //! < Widht of Special Character Key
    qint32 m_OkKeyWidht;                //! < Width of Ok Key
    qint32 m_LineEditWidgetW;           //! < Width of lineEdit widget.
    qint32 m_LineEditWidgetH;           //! < Width of lineEdit Height.
    QString m_IconSize;                 //! < For SIZE_1 "Large" ,SIZE_2 "Small"
    CRoundWidget *mp_KeyTrayBackGround; //! < Widget to hold the keys
    CKeyBoardButton *mp_DeleteBtn;      //! < BackSpace Key
    QLineEdit *mp_LineEdit;             //! < Line Edit Widget to display characters
    QWidget *mp_LineEditWidget;         //! < A Widget to hold the lineEdit and Delete button
    QVector<KeyBoard::CKeyBoardObserver *> mp_KeyBoardObserver; //! < Container for observers
    unsigned int m_MaxEnteredCharLen;   //! < Max Character length, Show error mesg if exceeds
    unsigned int m_MinEnteredCharLen;   //! < Min Character lenght, Show error message if exceeds
    bool m_EnteredCharsValid;           //! < True if entered char is valid.
    QString m_EnteredString;
    bool m_EnteredStringValidation;     //! <True for LongName validation and False for ShortName validation
    ValidationType_t m_ValidationType;

    CKeyBoardButton *CreateNewKey(QString IconType, QString BtnText1,
                                  QString BtnText2,bool IconPresent,qint32 ButtonType);
    void CreateKeyboard(void);
    void SetCaps(bool IsCaps);
    void MoveCharacters(bool Checked);
    void CreateSubSpeller(int Btn);
    void KeyBoardReset();
    void SetKeyBoardType(KeyBoard::KeyBoardType_t m_KeyBoardType);
    void NotifyObserver();
    void changeEvent(QEvent *p_Event);

public:
    explicit CKeyBoard(Model_t Model = SIZE_1, KeyBoardType_t KeyBoardType = QWERTY_KEYBOARD);
    ~CKeyBoard();

    void SetPasswordMode(bool Mode);
    void SetKeyBoardDialogTitle(QString Title);

    /****************************************************************************/
    /*!
     *  \brief This function is used to set the validation type
     *
     *  \iparam ValidationType - Validation_1/Validation_2/Validation_3
     */
    /****************************************************************************/

    void SetValidationType(ValidationType_t ValidationType){ m_ValidationType = ValidationType; }

    /****************************************************************************/
    /*!
     *  \brief This function returns if Caps lock is set or not
     */
    /****************************************************************************/
    bool GetCapsStatus()
    {
	return m_IsCaps;
    }

    /****************************************************************************/
    /*!
     *  \brief This function returns whether keyboard is alphabet or alphanumeric
     *           mode
     */
    /****************************************************************************/
    bool GetALTStatus() 
    {
	return m_AltToggled;
    }

    /****************************************************************************/
    /*!
     *  \brief This function is used to set content of the the line edit
     *  \iparam  LineEditContent
     */
    /****************************************************************************/
    void SetLineEditContent(QString LineEditContent) {
        mp_LineEdit->setText(LineEditContent);
    }

    /****************************************************************************/
    /*!
     *  \brief  Returns pointer to line edit widget
     *  \return mp_LineEdit
     */
    /****************************************************************************/
    QLineEdit *GetLineEditPtr() {return mp_LineEdit;}
    QString GetLineEditString();

    /****************************************************************************/
    /*!
     *  \brief This function is used to set the Max length of characters that should
     *         be entered.
     *
     *  \iparam  MaxCharLen
     */
    /****************************************************************************/
    void SetMaxCharLength(unsigned int MaxCharLen) { m_MaxEnteredCharLen =
                                                     MaxCharLen;}

    /****************************************************************************/
    /*!
     *  \brief This function is used to set the Min length of characters that should
     *         be entered.
     *
     *  \iparam  MaxCharLen
     */
    /****************************************************************************/
    void SetMinCharLength(unsigned int MinCharLen) { m_MinEnteredCharLen =
                                                     MinCharLen;}

    void Attach(KeyBoard::CKeyBoardObserver *p_KeyBoardObserver);
    void Detach();

    void ValidateString(QString m_Name);
   

private slots:
    void BtnClicked(int Btn);
    void AltToggled(bool Checked);
    void PressAndHold(int Btn);
    void SubSpellerClicked();
    void EscClicked();
    void OnOkClicked();
};

} // End of namespace KeyBoard

#endif //KEYBOARD_H
