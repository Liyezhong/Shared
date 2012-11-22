/****************************************************************************/
/*! \file KeyBoardButton.h
 *
 *  \brief Custom button definition.
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
#ifndef KEYBOARD_BUTTON_H
#define KEYBOARD_BUTTON_H

#include <Global/Include/Utils.h>
#include "Global/Include/Exception.h"
#include <QToolButton>
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>

namespace KeyBoard {

/****************************************************************************/
/**
 * \brief Button class for Keyboard widget
 */
/****************************************************************************/
class CKeyBoardButton : public QToolButton
{
    Q_OBJECT
private:
    QString m_IconType;//! <Left/Right or center
    QString m_BtnText1;//! <Bottom left text
    QString m_BtnText2; //!< Top Left text on keyboard button

    bool m_Clicked;//! < True button clicked
    bool m_Pressed; //!< Indicates if the widget is pressed
    bool m_CapsLock;//! < Caps lock on
    bool m_IconPresent;//! <True -Icon present on button
    bool m_Alpabet; //!< True alpabetical keys.False alpha numeric
    bool m_PressAndHold;//! < True if a button is pressed for long duration
    bool m_SubSpeller;//! < True if Subspeller is displayed
    bool m_Toggled;

    qint32 m_Size; //! < To determine if model is SIZE_1 /SIZE_2
    QList<QString> m_ButtonTextList;//! <This list holds the buttons for which subspeller is not required

    typedef enum {
        GENERIC = 0x0, SPACE = 1, SPECIAL_CHAR = 2, OK = 4, ESC = 8, SHIFT = 0x10, SHIFT_LOCK = 0x20,
        CURSOR_LEFT = 0x40, CURSOR_RIGHT = 0x80, BACKSPACE = 0x100,
    }ButtonType_t; //! <KeyBoard Modifiers
    ButtonType_t m_ButtonType; //! < Codes for keyboard modifiers

    QPixmap *mp_Pixmap; //! <pointers to button pixmap
    QWidget *mp_Parent;//! < pointer to parent widget
    QTimer *mp_Timer;//! < Timer object to implement press and hold

    QFont m_ButtonFont;//!< KeyBoard font
    QColor *mp_ColorWhite;//! <Text Color
    QColor *mp_ColorGrey;//! <Special Char Color
    QPixmap m_PixmapPressed;//! < Pixmap for button Pressed
    QPixmap m_PixmapEnabled;//! <Pixmap for button enabled
    QPixmap m_PixmapSelectedPressed;//! <Pixmap selected Pressed
    QPixmap m_PixmapSelectedEnabled;//! < Pixmap Selected and Enabled
    QPixmap m_PixmapTurnRed;//! < Red pixmap for Shiftlock also used during subspeller pops up
    QPixmap m_PixmapDefault;//! < Default pixmap

public:
    static qint32 m_ButtonWidth;//! < Width of the button
    static qint32 m_ButtonHeight;//! <Height of the button
    static qint32 m_NumericTextX;//! < X co-ordinate for Numeric Text
    static qint32 m_NumericTextY;//! <X Co-ordinate for Numeric Text
    static qint32 m_AlphabetTextX;//! < X Co-ordinate for Alphabet Text
    static qint32 m_AlphabetTextY;//! < Y Co-ordinate for Alphabet Text
    static qint32 m_SpecialCharX;//! <X Co-ordinate for SpecialChars
    static qint32 m_SpecialCharY;//! <Y Co-ordinate for Special chars
    static qint32 m_SubSpellerTextX;//! <X Co-ordinate for Subspeller text
    static qint32 m_FontSize;//! <Font Size of the text on button. 
    static QString m_IconSize;//! <Size of button
    explicit CKeyBoardButton(QWidget *parent = 0, QString IconType = "Center",
                             QString BtnText1= "1",QString BtnText2 ="!",
                             bool IconPresent = false, qint32 ButtonType = 0);
    ~CKeyBoardButton();
    void paintEvent(QPaintEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void SetText(QString Text1,QString Text2);
    void SetProperties(QString IconSize,QString IconType,bool IconPresent,bool m_Alpabet, qint32 ButtonType_t);
    void UpdateImage();

    /****************************************************************************/
    /*!
     *  \brief This function is called to set alpanumeric mode
     *
     *  \iparam IsAlphanumeric
     */
    /****************************************************************************/
    void IsAlpha(bool IsAlphanumeric) 
    {
 	m_Alpabet = IsAlphanumeric;
    }

    /****************************************************************************/
    /*!
     *  \brief This function is called to  get Button Text1
     *
     *  \return Text1
     */
    /****************************************************************************/
    QString GetText1()
    {
	return m_BtnText1;
    }

    /****************************************************************************/
    /*!
     *  \brief This function is called to  get Button Text2
     *  \iparam Text2
     */
    /****************************************************************************/
    QString GetText2()
    {
        return m_BtnText2;
    }

    /****************************************************************************/
    /*!
     *  \brief This function is called to inform that subspeller is enabled
     */
    /****************************************************************************/
    void SetPressAndHold()
    {
        m_PressAndHold = true;
    }

    void SetSubSpellerOff();

signals:
    void PressAndHold();
    void clicked();

private slots:
    void TimeOut();

};

}       //End of namespace KeyBoard

#endif // KEYBOARD_BUTTON_H
