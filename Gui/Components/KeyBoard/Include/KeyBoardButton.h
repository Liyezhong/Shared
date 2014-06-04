/****************************************************************************/
/*! \file KeyBoardButton.h
 *
 *  \brief Custom button definition.The CKeyBoardButton provides button feature
 *         for the keyboard widget.
 *         Features of button:
*                   - 1) Animates press and release
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
#ifndef KEYBOARD_BUTTON_H
#define KEYBOARD_BUTTON_H

#include <Global/Include/Utils.h>
#include "Global/Include/Exception.h"
#include <QToolButton>
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>

namespace KeyBoard {
//! enumeration of KeyBoard Modifiers
typedef enum {
    NORMAL_KEY = 0x0, SPACE = 1, SPECIAL_CHAR = 2, OK = 4, ESC = 8, SHIFT = 0x10, SHIFT_LOCK = 0x20,
    CURSOR_LEFT = 0x40, CURSOR_RIGHT = 0x80, BACKSPACE = 0x100
}ButtonType_t;


/****************************************************************************/
/**
 * \brief The CKeyBoardButton provides button feature for the keyboard widget.
 *        Features: 1) Animates press and release
 *                  2) Animates Press and Hold functionality
 *                  3) Dual / single character support in a single key.
 */
/****************************************************************************/
class CKeyBoardButton : public QToolButton
{
    Q_OBJECT
private:
    QWidget *mp_Parent;//!< pointer to parent widget
    QPixmap *mp_Pixmap; //!< pointers to button pixmap
    QTimer *mp_Timer;//!< Timer object to implement press and hold
    QColor *mp_ColorWhite;//!< Text Color
    QColor *mp_ColorGrey;//!< Special Char Color
    QString m_IconType;//!< Left/Right or center
    QString m_ButtonTextPrimary;//!< Bottom left text
    QString m_ButtonTextSecondary; //!< Top Left text on keyboard button
    bool m_IconPresent;//!< True -Icon present on button
    bool m_Clicked;//!< True button clicked
    bool m_Pressed; //!< Indicates if the widget is pressed
    bool m_Alpabet; //!< True alpabetical keys.False alpha numeric
    bool m_PressAndHold;//!< True if a button is pressed for long duration
    bool m_SubSpeller;//!< True if Subspeller is displayed
    bool m_Toggled; //!< Button toggle flag
    bool m_SubSpellerButton; //!< True  - Button on subspeller
    bool m_Size2; //!< To determine if model is SIZE_1 /SIZE_2
    bool m_IgnoreReleaseEvent; //!< Ignore release Event
    bool m_MouseMoveEventProcessed; //!< true indicates mouse leave event is processed and we can ignore it.
    ButtonType_t m_ButtonType; //!< keyboard modifiers type
    QFont m_ButtonFont;//!< KeyBoard font
    QPixmap m_PixmapPressed;//!< Pixmap for button Pressed
    QPixmap m_PixmapEnabled;//!< Pixmap for button enabled
    QPixmap m_PixmapSelectedPressed;//!< Pixmap selected Pressed
    QPixmap m_PixmapSelectedEnabled;//!< Pixmap Selected and Enabled
    QPixmap m_PixmapTurnRed;//!< Red pixmap for Shiftlock also used during subspeller pops up
    QPixmap m_PixmapDefault;//!< Default pixmap
    QList<QString> m_ButtonTextList;//!< This list holds the buttons for which subspeller is not required
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CKeyBoardButton)

public:
    static qint32 m_ButtonWidth;//!< Width of the button
    static qint32 m_ButtonHeight;//!< Height of the button
    static qint32 m_NumericTextX;//!< X co-ordinate for Numeric Text
    static qint32 m_NumericTextY;//!< X Co-ordinate for Numeric Text
    static qint32 m_AlphabetTextX;//!< X Co-ordinate for Alphabet Text
    static qint32 m_AlphabetTextY;//!< Y Co-ordinate for Alphabet Text
    static qint32 m_SpecialCharX;//!< X Co-ordinate for SpecialChars
    static qint32 m_SpecialCharY;//!< Y Co-ordinate for Special chars
    static qint32 m_SubSpellerTextX;//!< X Co-ordinate for Subspeller text
    static qint32 m_FontSize;//!< Font Size of the text on button.
    static QString m_IconSize;//!< Size of button

    explicit CKeyBoardButton(QWidget *parent = 0, QString IconType = "Center",
                             QString BtnText1= "1",QString BtnText2 ="!",
                             bool IconPresent = false, ButtonType_t ButtonType = NORMAL_KEY,
                             bool SubSpellerButton = false);
    ~CKeyBoardButton();
    void paintEvent(QPaintEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *p_MouseEvent);
    void SetText(QString PrimaryText,QString SecondaryText);

    /****************************************************************************/
    /*!
     *  \brief This function is called to set button to alphabet mode
     *
     *  \iparam AlphabetMode = true for alphabet mode, false for numeric mode
     */
    /****************************************************************************/
    void SetAlphabetMode(bool AlphabetMode)
    {
        m_Alpabet = AlphabetMode;
    }

    /****************************************************************************/
    /*!
     *  \brief This function is called to  get Button Text1
     *
     *  \return Primary text
     */
    /****************************************************************************/
    QString GetPrimaryText()
    {
        return m_ButtonTextPrimary;
    }

    /****************************************************************************/
    /*!
     *  \brief This function is called to  get the secondary text of button
     *  \return Secondary text
     */
    /****************************************************************************/
    QString GetSecondaryText()
    {
        return m_ButtonTextSecondary;
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

    /****************************************************************************/
    /*!
     *  \brief Sets the toggled state of button
     *  \iparam Toggled = Toggles between true and false every time button
     *                    is clicked
     */
    /****************************************************************************/
    void SetToggledState(const bool Toggled)
    {
        m_Toggled = Toggled;
    }

    void SetSubSpellerOff();

signals:
    /****************************************************************************/
    /*!
     *  \brief This function is called that user has pressed button for more than
     *         1 second
     */
    /****************************************************************************/
    void PressAndHold();
    /****************************************************************************/
    /*!
     *  \brief signal emitted when button is clicked
     */
    /****************************************************************************/
    void Buttonclicked();

private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void TimeOut();

};

}       //End of namespace KeyBoard

#endif // KEYBOARD_BUTTON_H
