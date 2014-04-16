/****************************************************************************/
/*! \file SubSpeller.cpp
 *
 *  \brief File provides implementation of a subspeller.Subspeller is created
 *         when a button requiring subspeller is pressed (press & hold)
 *         on the keyboard.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-11-10
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

#include "KeyBoard/Include/SubSpeller.h"
#include "Application/Include/LeicaStyle.h"
#include "KeyBoard/Include/KeyBoard.h"
#include <QBitmap>
#include <QDebug>
#include <QTextCodec>

namespace KeyBoard {


//By default SIZE_1 values are set.
QString CSubSpeller::m_Model = "Large"; //!< Size 1 means "Large" or size 2 means "Small"
qint32 CSubSpeller::m_SubspellerBorderMarginLeft = 25; //!< Margins in pixel
qint32 CSubSpeller::m_SubspellerBorderMarginTop = 20; //!< Margins in pixel
qint32 CSubSpeller::m_SubspellerBorderMarginRight = 25; //!< Margins in pixel
qint32 CSubSpeller::m_SubspellerBorderMarginBottom = 25; //!< Margins in pixel
qint32 CSubSpeller::m_SubspellerMarginRight= 1; //!< Margins in pixel
qint32 CSubSpeller::m_SubspellerOffsetW = 20; //!< Margins in pixel
qint32 CSubSpeller::m_SubspellerOffsetH = 25; //!< Margins in pixel

// CSubSpeller::SubSpellerLookUp values refer to "ISO 8859-15" character encoding.
//   See: http://en.wikipedia.org/wiki/ISO/IEC_8859-15
const KeyBoard::SubSpellerLookUp CSubSpeller::m_SubSpellerLookUp[56] = {
    //Lower case subspellers
    //eg:
    //{ { !, á , à , â, ä, ã, å},SIZE_1 X coodrinate,SIZE_1 Y coordinate,
    //SIZE_2 X Coordinate,SIZE_2 Y Cordinate, No of subspeller keys }
    { { 33, 225, 224, 226, 228, 227, 229 }, 81, 267, 85, 286, 7},//a
    { { 41, 0, 0, 0, 0, 0, 0 }, 404, 337, 293, 361, 1 },//b
    { { 176, 231, 162, 0, 0, 0 , 0 }, 263, 337, 202, 361, 3 },//c
    { { 35, 0, 0, 0, 0, 0, 0 }, 223, 267, 177, 286, 1 },//d
    { { 51, 233, 232, 234, 235, 0, 0 }, 193, 197, 156, 210, 5 },//e
    { { 36, 0, 0, 0, 0, 0, 0 }, 293, 267, 222, 286, 1 },//f
    { { 37, 0, 0, 0, 0, 0, 0 }, 363, 267, 269, 286, 1 },//g
    { { 38, 0, 0, 0, 0, 0, 0 }, 431, 267, 315, 286, 1 },//h
    { { 56, 237, 236, 238, 239 , 0, 0 }, 263, 197, 202, 210, 5 },//i
    { { 42, 0, 0, 0, 0, 0, 0 }, 501, 267, 360, 286, 1 },//j
    { { 63, 0, 0, 0, 0, 0, 0 }, 572, 267, 407, 286, 1 },//k
    { { 47, 0, 0, 0, 0, 0, 0 }, 642, 267, 453, 286, 1 },//l
    { { 95, 0, 0, 0, 0, 0, 0 }, 543, 337, 385, 361, 1 },//m
    { { 45, 241, 0, 0, 0, 0, 0 }, 403, 337, 293, 361, 2 },//n
    { { 57, 243, 242, 244, 246, 245, 248 }, 193, 197, 156, 210, 7 },//o
    { { 48, 0, 0, 0, 0, 0, 0 }, 682, 197, 478, 210, 1 },//p
    { { 49, 0, 0, 0, 0, 0, 0 }, 55, 197, 66, 210, 1 },//q
    { { 52, 0, 0, 0, 0, 0, 0 }, 263, 197, 204, 210, 1 },//r
    { { 64, 168, 0, 0, 0, 0, 0 }, 151, 267, 131, 286, 2 },//s
    { { 53, 0, 0, 0, 0, 0, 0 }, 332, 197, 250, 210, 1 },//t
    { { 55, 250, 249, 251, 252, 0, 0 }, 194, 197, 156, 210, 5 },//u
    { { 40, 0, 0, 0, 0, 0, 0 }, 334, 337, 247, 361, 1 },//v
    { { 50, 0, 0, 0, 0, 0, 0 }, 123, 197, 111, 210, 1 },//w
    { { 34, 0, 0, 0, 0, 0, 0 }, 192, 337, 155, 361, 1 },//x
    { { 54, 253, 255, 0, 0, 0, 0 }, 265, 197, 203, 210, 3 },//y
    { {126, 184, 0, 0, 0, 0, 0 }, 123, 337, 109, 361, 2 },//z
    { { 59, 230, 0, 0, 0, 0, 0 }, 544, 337, 386, 361, 2 },//,
    { { 58, 0, 0, 0, 0, 0, 0 }, 683, 337, 477, 361, 1 },//.
    //Upper Case Subspellers
    { { 33, 193, 192, 194, 196, 195, 197 }, 81, 267, 85, 286, 7 },//A
    { { 41, 0, 0, 0, 0, 0, 0 }, 404, 337, 293, 361, 1 },//B
    { { 39, 199, 0, 0, 0, 0, 0 }, 263, 337, 202, 361, 2 },//C
    { { 35, 0, 0, 0, 0, 0, 0 }, 223, 267, 177, 286, 1 },//D
    { { 51, 201, 200, 202, 203, 0, 0 }, 193, 197, 156, 210, 5 },//E
    { { 36, 0, 0, 0, 0, 0, 0 }, 293, 267, 222, 286, 1 },//F
    { { 37, 0, 0, 0, 0, 0, 0 }, 363, 267, 269, 286, 1 },//G
    { { 38, 0, 0, 0, 0, 0, 0 }, 431, 267, 315, 286, 1 },//H
    { { 56, 205, 204, 206, 207, 0, 0 }, 263, 197, 202, 210, 5 }, //I
    { { 42, 0, 0, 0, 0, 0, 0 }, 501, 267, 360, 286, 1 },//J
    { { 63, 0, 0, 0, 0, 0, 0 }, 572, 267, 407, 286, 1 },//K
    { { 47, 0, 0, 0, 0, 0, 0 }, 642, 267, 453, 286, 1 },//L
    { { 95, 0, 0, 0, 0, 0, 0 }, 543, 337, 385, 361, 1 },//M
    { { 45, 209, 0, 0, 0, 0, 0 }, 403, 337, 293, 361, 2 },//N
    { { 57, 211, 210, 212, 214, 213, 216}, 193, 197, 156, 210, 7 }, //O
    { { 48, 0, 0, 0, 0, 0, 0 }, 683, 195, 478, 210, 1 },//P
    { { 49, 0, 0, 0, 0, 0, 0 }, 55, 197, 66, 210, 1 },//Q
    { { 52, 0, 0, 0, 0, 0, 0 }, 265, 197, 204, 210, 1 },//R
    { { 64, 166, 0, 0, 0, 0, 0 }, 151, 267, 131, 286, 2 },//S
    { { 53, 0, 0, 0, 0, 0, 0 }, 335, 197, 250, 210, 1 },//T
    { { 55, 218, 217, 219, 220 , 0, 0 }, 194, 197, 156, 210, 5 },//U
    { { 40, 0, 0, 0, 0, 0, 0 }, 334, 342, 247, 361, 1 },//V
    { { 50, 0, 0, 0, 0, 0, 0 }, 123, 197, 111, 210, 1 },//W
    { { 34, 0, 0, 0, 0, 0, 0 }, 195, 337, 155, 361, 1 },//X
    { { 54, 221, 0, 0, 0, 0, 0 }, 335, 197, 247, 210, 2 },//Y
    { { 126, 180, 0, 0, 0, 0, 0 }, 123, 337, 110, 361, 2 },//Z
    { { 59, 198, 0, 0, 0, 0, 0 }, 544, 337, 386, 361, 2 },//,
    { { 58, 0, 0, 0, 0, 0, 0 }, 683, 337, 477, 361, 1},//.
};


const int ONE_BUTTON_SUBSPELLER = 1; //!< Subspeller has only one button

/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam p_Parent = Parent widget (Keyboard widget)
 *  \iparam p_KeyBoardButton = Parent Button , i.e. the button from where subspeller
 *                             was opened.
 *  \iparam Offset = offset for look up table. 0 for lower case , 28 for upper case
 *  \iparam LookUpIndex = Index in the lookup table
 *  \iparam Model = Resolution selection (size1/size2)
 *  \iparam KeyBoardType = Layout selection (Normal/German)
 */
/****************************************************************************/
CSubSpeller::CSubSpeller(CKeyBoard *p_Parent,
                         CKeyBoardButton *p_KeyBoardButton,
                         qint32 Offset,
                         qint32 LookUpIndex,
                         KeyBoard::Model_t Model,
                         KeyBoard::KeyBoardType_t KeyBoardType)
    : QDialog(p_Parent,Qt::FramelessWindowHint)
{
    mp_Parent = p_Parent;
    mp_ParentButton = p_KeyBoardButton;
    //! See : http://en.wikipedia.org/wiki/ISO/IEC_8859-15
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("ISO 8859-15"));
    //10000 should be good enough
    setMaximumWidth(10000);
    setAutoFillBackground(false);
    setAttribute(Qt::WA_TranslucentBackground);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    (void)m_Pixmap.load(QString(":/%1/SpellerButtons/Subspeller_BG.png").arg(m_Model));

    mp_SpacerItem = 0;
    mp_HorizontalLayout = 0;
    mp_HorizontalLayout = new QHBoxLayout(this);
    mp_HorizontalLayout->setSpacing(1);

    qint32 XCoordinate = 0;
    qint32 YCoordinate = 0;
    GenerateCoordinates(LookUpIndex, Offset, Model, KeyBoardType, XCoordinate, YCoordinate);
    InitSubSpellerGUI(LookUpIndex, Offset, XCoordinate, YCoordinate, Model, KeyBoardType);

}

/****************************************************************************/
/*!
 *  \brief Generates the X & Y co-ordinates for the subspeller
 *  \iparam LookUpIndex = Index in the lookup table
 *  \iparam Offset = offset for look up table. 0 for lower case , 28 for upper case
 *  \iparam Model = Resolution selection (size1/size2)
 *  \iparam KeyBoardType = Layout selection (Normal/German)
 *  \xparam XCoordinate = XCoordinate of subspeller
 *  \xparam YCoordinate = YCoordinate of subspeller
 */
/****************************************************************************/
void CSubSpeller::GenerateCoordinates(qint32 LookUpIndex,
                                      qint32 Offset,
                                      KeyBoard::Model_t Model,
                                      KeyBoard::KeyBoardType_t KeyBoardType,
                                      qint32 &XCoordinate,
                                      qint32 &YCoordinate) {
    if (Model == KeyBoard::SIZE_1) {
        XCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex].Size1XPos;
        YCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex].Size1YPos;
    }
    else {
        XCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex].Size2XPos;
        YCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex].Size2YPos;
    }
    //Below code makes sure that keyboard position doesnt affect
    //Subspeller positions. This is needed since we have subspeller
    //positions stored in table for default position.
    qint32 KeyboardXPos = 0;
    qint32 KeyboardYPos = 0;
    mp_Parent->GetKeyboardCoordinates(KeyboardXPos, KeyboardYPos);
    qint32 XOffset = KeyboardXPos - DEFAULT_POSITION_X_COORDINATE;
    XCoordinate += XOffset;
    qint32 YOffset = KeyboardYPos - DEFAULT_POSITION_Y_COORDINATE;
    YCoordinate += YOffset;
    if (KeyBoardType == QWERTZ_KEYBOARD && Model == SIZE_1) {
       //if char is y/Y then swap coordinates with z/Z
       if (LookUpIndex == LETTER_Y -1 ) { // subtracting "1" because index starts with "0"
           XCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex + 1].Size1XPos;
           YCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex + 1].Size1YPos;
       }
       // check if z is uppercase or lowercase
       else if( LookUpIndex == (LETTER_Z -1) && Offset == 0 ) {
           XCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex - 1].Size1XPos + 70; // for lowercase z XCoordinate increased 70 pixels.
           YCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex - 1].Size1YPos;
       }
       else if (LookUpIndex == (LETTER_Z -1) && Offset == TOTAL_KEYS_USING_SUBSPELLER ) {
           XCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex - 1].Size1XPos;
           YCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex - 1].Size1YPos;
       }
    }
    else if (KeyBoardType == QWERTZ_KEYBOARD && Model == SIZE_2) {
        if (LookUpIndex == LETTER_Y -1 ) {
            XCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex + 1].Size2XPos;
            YCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex + 1].Size2YPos;
        }
        else if (LookUpIndex == (LETTER_Z -1) && Offset == 0) {
            XCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex - 1].Size2XPos + 44; // for lowercase z XCoordinate increased 44 pixels
            YCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex - 1].Size2YPos;
        }
        else if (LookUpIndex == (LETTER_Z -1) && Offset == TOTAL_KEYS_USING_SUBSPELLER) {
            XCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex - 1].Size1XPos - 88; // for uppercase z XCoordinate decreased 88 pixels.
            YCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex - 1].Size1YPos + 12; // for uppercase z YCoordinate increased 12 pixels.
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Creates Subspeller GUI
 *  \iparam LookUpIndex = Index in the lookup table
 *  \iparam Offset = offset for look up table. 0 for lower case , 28 for upper case
 *  \xparam XCoordinate = XCoordinate of subspeller
 *  \xparam YCoordinate = YCoordinate of subspeller
 *  \iparam Model = Resolution selection (size1/size2)
 *  \iparam KeyBoardType = Layout selection (Normal/German)
 */
/****************************************************************************/
void CSubSpeller::InitSubSpellerGUI(qint32 LookUpIndex,
                                    qint32 Offset,
                                    qint32 XCoordinate,
                                    qint32 YCoordinate,
                                    KeyBoard::Model_t Model,
                                    KeyBoard::KeyBoardType_t KeyBoardType) {
    QString ButtonText;
    if (KeyBoardType == QWERTZ_KEYBOARD) {
        //Need to swap first key in subspeller for Y & Z if QWERTZ keyboard
         if (LookUpIndex == LETTER_Y -1 ) { // subtracting "1" because index starts with "0"
            ButtonText = QChar::fromAscii(m_SubSpellerLookUp[Offset + LookUpIndex + 1].AsciiCodeOfKey[0]);
         }
         else if( LookUpIndex == (LETTER_Z -1)) { // z or Z
             ButtonText = QChar::fromAscii(m_SubSpellerLookUp[Offset + LookUpIndex -1].AsciiCodeOfKey[0]);
         }
         else { // Other chars
            ButtonText = QChar::fromAscii(m_SubSpellerLookUp[Offset + LookUpIndex].AsciiCodeOfKey[0]);
         }
    }
    else {
        ButtonText = QChar::fromAscii(m_SubSpellerLookUp[Offset + LookUpIndex].AsciiCodeOfKey[0]);
    }
    if (m_SubSpellerLookUp[Offset + LookUpIndex].NoOfSubSpeller == ONE_BUTTON_SUBSPELLER) {
        setGeometry(XCoordinate, YCoordinate, mp_ParentButton->width() + m_SubspellerOffsetW,
                    mp_ParentButton->height() + m_SubspellerOffsetH);
        QPixmap Target(size());
        Target.fill(Qt::transparent);
        Application::CLeicaStyle::BorderPixmap(&Target, &m_Pixmap, m_SubspellerBorderMarginLeft, m_SubspellerBorderMarginTop,
                                               m_SubspellerBorderMarginRight, m_SubspellerBorderMarginBottom);

        m_Pixmap = Target;

        mp_HorizontalLayout->addWidget(CSubSpeller::CreateNewKey("", ButtonText, "", false));
        mp_HorizontalLayout->setContentsMargins(0, 0, m_SubspellerMarginRight, 0);
    }
    else {
        qint16 NoOfSubSpellerKeys = m_SubSpellerLookUp[Offset + LookUpIndex].NoOfSubSpeller;
        qint32 SubSpellerOffsetW;
        if (Model == SIZE_1) {
            SubSpellerOffsetW = 20;
        }
        else {
            SubSpellerOffsetW = 25;
        }

        qint16 Width = (mp_ParentButton->width() * NoOfSubSpellerKeys ) + SubSpellerOffsetW;
        setGeometry(XCoordinate, YCoordinate, Width, mp_ParentButton->height() + m_SubspellerOffsetH);
        QPixmap Target(size());
        Target.fill(Qt::transparent);
        Application::CLeicaStyle::BorderPixmap(&Target, &m_Pixmap, 35, 35, 35, 35); //25,25 top and bottom
        m_Pixmap = Target;
        mp_HorizontalLayout->setSpacing(0);
        mp_HorizontalLayout->addWidget(CSubSpeller::CreateNewKey("Left",ButtonText,"",false));
        if (Model == SIZE_2) {
            mp_HorizontalLayout->setContentsMargins(10, 2, 14, 0);
        }
        else {
            mp_HorizontalLayout->setContentsMargins(10, 0, 10, 0);
        }
        for( qint32 I = 1; I < m_SubSpellerLookUp[Offset + LookUpIndex].NoOfSubSpeller ; I++) {
            ButtonText = QChar::fromAscii(m_SubSpellerLookUp[Offset + LookUpIndex].AsciiCodeOfKey[I]);
            QString IconType = (I != m_SubSpellerLookUp[Offset + LookUpIndex].NoOfSubSpeller - 1) ? "Center" : "Right";
            CKeyBoardButton *p_Button = CSubSpeller::CreateNewKey(IconType, ButtonText, "", false);
            mp_HorizontalLayout->addWidget(p_Button);
        }
    }

    //Find buttons and connect them
    m_SubSpellerList = findChildren<CKeyBoardButton *>();
    mp_SubSpellerClickedMapper = new QSignalMapper(this);
    for (int I = 0;I < m_SubSpellerList.count(); I++) {
        CONNECTSIGNALSLOT(m_SubSpellerList.at(I), Buttonclicked(),mp_SubSpellerClickedMapper, map());
        mp_SubSpellerClickedMapper->setMapping(m_SubSpellerList.at(I), I);
    }
    CONNECTSIGNALSLOT(mp_SubSpellerClickedMapper, mapped(int), this, OnButtonClicked(int));
    mp_LineEdit = mp_Parent->GetLineEditPtr();

}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CSubSpeller::~CSubSpeller()
{
    //Layout , Buttons and , signal mapper is deleted automatically by QT memory
    //management system.

}

/****************************************************************************/
/*!
 *  \brief This slot is called when a button on subspeller is clicked
 *
 *  \iparam Button - button index
 */
/****************************************************************************/
void CSubSpeller::OnButtonClicked(int Button)
{
    //hide subspeller and display clicked character on line edit box
    hide();
    QString KeyChar = m_SubSpellerList.at(Button)->GetPrimaryText();
    mp_LineEdit->insert(KeyChar);
    mp_ParentButton->SetSubSpellerOff();
    emit SubSpellerClicked();
}

/****************************************************************************/
/*!
 *  \brief This function is called to create a new key on the keyboard
 *
 *  \iparam IconType = Large/small
 *  \iparam PrimaryText = Bottom left text on button
 *  \iparam SecondaryText = Top left text on button (Not used for now)
 *  \iparam IconPresent = true -icon is present
 *  \return Pointer to the button created
 */
/****************************************************************************/
CKeyBoardButton *CSubSpeller::CreateNewKey(QString IconType,
                                           QString PrimaryText, QString SecondaryText,
                                           bool IconPresent)
{
    //! \warning Dont delete this object explicitly. QT parent child mechanism will
    //! take care of deletion. When subspeller is deleted, all the buttons get deleted.
    mp_KeyBoardButton = new CKeyBoardButton(this, IconType, PrimaryText, SecondaryText, IconPresent, NORMAL_KEY, true);
    mp_KeyBoardButton->SetPressAndHold();
    return mp_KeyBoardButton;
}

/****************************************************************************/
/*!
 *  \brief Draws the subspeller background
 *  \iparam p_PaintEvent = Paint event
 */
/****************************************************************************/
void CSubSpeller::paintEvent(QPaintEvent *p_PaintEvent)
{
      Q_UNUSED(p_PaintEvent);
      QPainter Painter(this);
      Painter.drawPixmap(0, 0, m_Pixmap);
}

/****************************************************************************/
/*!
 *  \brief This fucntion is called when subspeller is disabled.
 *
 */
/****************************************************************************/
void CSubSpeller::SetSubSpellerOff()
{
    if (mp_ParentButton) {
        mp_ParentButton->SetSubSpellerOff();
    }
}

}//End of namespace KeyBoard
