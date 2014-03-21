/****************************************************************************/
/*! \file SubSpeller.cpp
 *
 *  \brief SubSpeller implementation.
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
QString CSubSpeller::m_Model = "Large";
qint32 CSubSpeller::m_SubspellerBorderMarginLeft = 25;
qint32 CSubSpeller::m_SubspellerBorderMarginTop = 20;
qint32 CSubSpeller::m_SubspellerBorderMarginRight = 25;
qint32 CSubSpeller::m_SubspellerBorderMarginBottom = 20;
qint32 CSubSpeller::m_SubspellerMarginRight= 2;
qint32 CSubSpeller::m_SubspellerOffsetW = 20;
qint32 CSubSpeller::m_SubspellerOffsetH = 25;

// CSubSpeller::SubSpellerLookUp values refer to "ISO 8859-15" character encoding.
//   See: http://en.wikipedia.org/wiki/ISO/IEC_8859-15
// TODO: insert comments with printable characters in order to make this maintainable.
const CSubSpeller::SubSpellerLookUp CSubSpeller::m_SubSpellerLookUp[56] = {
    //Lower case subspellers
    //eg:
    //{ { !, á , à , â, ä, ã, å},SIZE_1 X coodrinate,SIZE_1 Y coordinate,
    //SIZE_2 X Coordinate,SIZE_2 Y Cordinate, No of subspeller keys }
    { { 33, 225, 224, 226, 228, 227, 229 }, 81, 267, 85, 286, 7},
    { { 41, 0, 0, 0, 0, 0, 0 }, 404, 337, 293, 361, 1 },
    { { 176, 231, 162, 0, 0, 0 , 0 }, 263, 337, 202, 361, 3 },
    { { 35, 0, 0, 0, 0, 0, 0 }, 223, 267, 177, 286, 1 },
    { { 51, 233, 232, 234, 235, 0, 0 }, 193, 197, 156, 210, 5 },
    { { 36, 0, 0, 0, 0, 0, 0 }, 293, 267, 222, 286, 1 },
    { { 37, 0, 0, 0, 0, 0, 0 }, 363, 267, 269, 286, 1 },
    { { 38, 0, 0, 0, 0, 0, 0 }, 431, 267, 315, 286, 1 },
    { { 56, 237, 236, 238, 239 , 0, 0 }, 263, 197, 202, 210, 5 },
    { { 42, 0, 0, 0, 0, 0, 0 }, 501, 267, 360, 286, 1 },
    { { 63, 0, 0, 0, 0, 0, 0 }, 572, 267, 407, 286, 1 },
    { { 47, 0, 0, 0, 0, 0, 0 }, 642, 267, 453, 286, 1 },
    { { 95, 0, 0, 0, 0, 0, 0 }, 545, 337, 385, 361, 1 },
    { { 45, 241, 0, 0, 0, 0, 0 }, 403, 337, 293, 361, 2 },
    { { 57, 243, 242, 244, 246, 245, 248 }, 193, 197, 156, 210, 7 },
    { { 48, 0, 0, 0, 0, 0, 0 }, 685, 197, 478, 210, 1 },
    { { 49, 0, 0, 0, 0, 0, 0 }, 55, 197, 66, 210, 1 },
    { { 52, 0, 0, 0, 0, 0, 0 }, 265, 197, 204, 210, 1 },
    { { 64, 168, 0, 0, 0, 0, 0 }, 151, 267, 131, 286, 2 },
    { { 53, 0, 0, 0, 0, 0, 0 }, 335, 197, 250, 210, 1 },
    { { 55, 250, 249, 251, 252, 0, 0 }, 194, 197, 156, 210, 5 },
    { { 40, 0, 0, 0, 0, 0, 0 }, 336, 337, 247, 361, 1 },
    { { 50, 0, 0, 0, 0, 0, 0 }, 125, 197, 111, 210, 1 },
    { { 34, 0, 0, 0, 0, 0, 0 }, 195, 337, 155, 361, 1 },
    { { 54, 253, 255, 0, 0, 0, 0 }, 265, 197, 203, 210, 3 },
    { {126, 184, 0, 0, 0, 0, 0 }, 123, 337, 109, 361, 2 },
    { { 59, 230, 0, 0, 0, 0, 0 }, 544, 337, 386, 361, 2 },
    { { 58, 0, 0, 0, 0, 0, 0 }, 685, 337, 477, 361, 1 },
    //Upper Case Subspellers
    { { 33, 193, 192, 194, 196, 195, 197 }, 81, 267, 85, 286, 7 },
    { { 41, 0, 0, 0, 0, 0, 0 }, 404, 337, 293, 361, 1 },
    { { 39, 199, 0, 0, 0, 0, 0 }, 263, 337, 202, 361, 2 },
    { { 35, 0, 0, 0, 0, 0, 0 }, 223, 267, 177, 286, 1 },
    { { 51, 201, 200, 202, 203, 0, 0 }, 193, 197, 156, 210, 5 },
    { { 36, 0, 0, 0, 0, 0, 0 }, 293, 267, 222, 286, 1 },
    { { 37, 0, 0, 0, 0, 0, 0 }, 363, 267, 269, 286, 1 },
    { { 38, 0, 0, 0, 0, 0, 0 }, 431, 267, 315, 286, 1 },
    { { 56, 205, 204, 206, 207, 0, 0 }, 263, 197, 202, 210, 5 },
    { { 42, 0, 0, 0, 0, 0, 0 }, 501, 267, 360, 286, 1 },
    { { 63, 0, 0, 0, 0, 0, 0 }, 572, 267, 407, 286, 1 },
    { { 47, 0, 0, 0, 0, 0, 0 }, 642, 267, 453, 286, 1 },
    { { 95, 0, 0, 0, 0, 0, 0 }, 545, 337, 385, 361, 1 },
    { { 45, 209, 0, 0, 0, 0, 0 }, 403, 337, 293, 361, 2 },
    { { 57, 211, 210, 212, 214, 213, 216}, 193, 197, 156, 210, 7 },
    { { 48, 0, 0, 0, 0, 0, 0 }, 685, 195, 478, 210, 1 },
    { { 49, 0, 0, 0, 0, 0, 0 }, 55, 197, 66, 210, 1 },
    { { 52, 0, 0, 0, 0, 0, 0 }, 265, 197, 204, 210, 1 },
    { { 64, 166, 0, 0, 0, 0, 0 }, 151, 267, 131, 286, 2 },
    { { 53, 0, 0, 0, 0, 0, 0 }, 335, 197, 250, 210, 1 },
    { { 55, 218, 217, 219, 220 , 0, 0 }, 194, 197, 156, 210, 5 },
    { { 40, 0, 0, 0, 0, 0, 0 }, 336, 342, 247, 361, 1 },
    { { 50, 0, 0, 0, 0, 0, 0 }, 125, 197, 111, 210, 1 },
    { { 34, 0, 0, 0, 0, 0, 0 }, 195, 337, 155, 361, 1 },
    { { 54, 221, 0, 0, 0, 0, 0 }, 335, 197, 247, 210, 2 },
    { { 126, 180, 0, 0, 0, 0, 0 }, 123, 337, 110, 361, 2 },
    { { 59, 198, 0, 0, 0, 0, 0 }, 544, 337, 386, 361, 2 },
    { { 58, 0, 0, 0, 0, 0, 0 }, 685, 337, 477, 361, 1},
};



/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam p_Parent = Parent widget
 *  \iparam LookUpIndex
 */
/****************************************************************************/
CSubSpeller::CSubSpeller(CKeyBoard *p_Parent, CKeyBoardButton *p_KeyBoardButton, qint32 Offset, qint32 LookUpIndex,
                         KeyBoard::Model_t Model, KeyBoard::KeyBoardType_t KeyBoardType):
			QDialog(p_Parent,Qt::FramelessWindowHint)
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("ISO 8859-15"));
    mp_Parent = p_Parent;
    mp_ParentButton = p_KeyBoardButton;
    setMaximumWidth(10000);
    setAutoFillBackground(false);
    setAttribute(Qt::WA_TranslucentBackground);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    m_Pixmap.load(QString(":/%1/SpellerButtons/Subspeller_BG.png").arg(m_Model));

    mp_SpacerItem = 0;
    mp_HorizontalLayout = 0;
    mp_HorizontalLayout = new QHBoxLayout(this);
    mp_HorizontalLayout->setSpacing(1);

    qDebug()<<"Offset"<<Offset<<"LookUpIndex"<<LookUpIndex;
    qint32 XCoordinate;
    qint32 YCoordinate;
    if (Model == KeyBoard::SIZE_1) {
        XCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex].Size1XPos;
        YCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex].Size1YPos;
    }
    else {
        XCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex].Size2XPos;
        YCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex].Size2YPos;
    }

    QString TempString;
    TempString = QChar::fromAscii(m_SubSpellerLookUp[Offset + LookUpIndex].Button1Ascii[0]);
    if (KeyBoardType == QWERTZ_KEYBOARD && Model == SIZE_1) {
       //if char is y/Y then swap coordinates with z/Z
       if (LookUpIndex == 24 ) {
           XCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex + 1].Size1XPos;
           YCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex + 1].Size1YPos;
           TempString = QChar::fromAscii(m_SubSpellerLookUp[Offset + LookUpIndex + 1].Button1Ascii[0]);
       }
	// check if z is uppercase or lowercase
       else if( LookUpIndex == 25 && Offset == 0 ) {
           XCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex - 1].Size1XPos + 70; // for lowercase z XCoordinate increased 70 pixels.
           YCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex - 1].Size1YPos;
           TempString = QChar::fromAscii(m_SubSpellerLookUp[Offset + LookUpIndex - 1].Button1Ascii[0]);
       }
       else if (LookUpIndex == 25 && Offset == 28 ) {
           XCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex - 1].Size1XPos;
           YCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex - 1].Size1YPos;
           TempString = QChar::fromAscii(m_SubSpellerLookUp[Offset + LookUpIndex - 1].Button1Ascii[0]);
       }
    }
    else if (KeyBoardType == QWERTZ_KEYBOARD && Model == SIZE_2) {
        if (LookUpIndex == 24 ) {
            XCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex + 1].Size2XPos;
            YCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex + 1].Size2YPos;
            TempString = QChar::fromAscii(m_SubSpellerLookUp[Offset + LookUpIndex + 1].Button1Ascii[0]);
        }
        else if( LookUpIndex == 25 && Offset == 0) {
            XCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex - 1].Size2XPos + 44; // for lowercase z XCoordinate increased 47 pixels
            YCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex - 1].Size2YPos;
            TempString = QChar::fromAscii(m_SubSpellerLookUp[Offset + LookUpIndex - 1].Button1Ascii[0]);
        }
        else if (LookUpIndex == 25 && Offset == 28 ) {
           XCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex - 1].Size1XPos - 88; // for uppercase z XCoordinate decreased 99 pixels.
           YCoordinate = m_SubSpellerLookUp[Offset + LookUpIndex - 1].Size1YPos + 12; // for uppercase z YCoordinate increased 12 pixels.
           TempString = QChar::fromAscii(m_SubSpellerLookUp[Offset + LookUpIndex - 1].Button1Ascii[0]);
       }
    }
    if (m_SubSpellerLookUp[Offset + LookUpIndex].NoOfSubSpeller == 1) {
        setGeometry(XCoordinate, YCoordinate, p_KeyBoardButton->width() + m_SubspellerOffsetW,
                    p_KeyBoardButton->height() + m_SubspellerOffsetH);
        QPixmap Target(size());
        Target.fill(Qt::transparent);
        Application::CLeicaStyle::BorderPixmap(&Target, &m_Pixmap, m_SubspellerBorderMarginLeft, m_SubspellerBorderMarginTop,
                                               m_SubspellerBorderMarginRight, m_SubspellerBorderMarginBottom);

        m_Pixmap = Target;

        mp_HorizontalLayout->addWidget(CSubSpeller::CreateNewKey("", TempString, "", false));
        mp_HorizontalLayout->setContentsMargins(0,0,m_SubspellerMarginRight,0);
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
        qint16 Width = (p_KeyBoardButton->width() * NoOfSubSpellerKeys ) + SubSpellerOffsetW;
        setGeometry(XCoordinate, YCoordinate, Width, p_KeyBoardButton->height() + m_SubspellerOffsetH); 
        QPixmap Target(size());
        Target.fill(Qt::transparent);
        Application::CLeicaStyle::BorderPixmap(&Target, &m_Pixmap, 30, 20, 30, 20);
             m_Pixmap = Target;
        mp_HorizontalLayout->setSpacing(0);
        mp_HorizontalLayout->addWidget(CSubSpeller::CreateNewKey("Left",TempString,"",false));
        if (Model == SIZE_2) {
            mp_HorizontalLayout->setContentsMargins(10, 2, 12, 0);
        }
        else {
            mp_HorizontalLayout->setContentsMargins(10, 0, 10, 0);
        }
        for( qint32 I = 1; I < m_SubSpellerLookUp[Offset + LookUpIndex].NoOfSubSpeller ; I++) {
            TempString = QChar::fromAscii(m_SubSpellerLookUp[Offset + LookUpIndex].Button1Ascii[I]);
            mp_HorizontalLayout->addWidget(CSubSpeller::CreateNewKey(QString("%1").arg(( I != m_SubSpellerLookUp[Offset + LookUpIndex].NoOfSubSpeller - 1) ? "Center" : "Right"),
                                                                     TempString, "", false));
        }
    }


    m_SubSpellerList = findChildren<CKeyBoardButton *>();
    mp_SubSpellerClickedMapper = new QSignalMapper(this);
    for (int i = 0;i < m_SubSpellerList.count(); i++) {
        CONNECTSIGNALSLOT(m_SubSpellerList.at(i), clicked(),mp_SubSpellerClickedMapper, map());
        mp_SubSpellerClickedMapper->setMapping(m_SubSpellerList.at(i), i);
    }
    CONNECTSIGNALSLOT(mp_SubSpellerClickedMapper, mapped(int), this, BtnClicked(int));
    mp_LineEdit = mp_Parent->GetLineEditPtr();
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CSubSpeller::~CSubSpeller()
{
    try {
        delete mp_SubSpellerClickedMapper;
        while (!m_SubSpellerList.isEmpty()) {
            delete m_SubSpellerList.takeFirst();
        }
        if (mp_HorizontalLayout) {
            delete mp_HorizontalLayout;
        }
    }
    catch (...){

    }
}

/****************************************************************************/
/*!
 *  \brief This slot is called when a button on subspeller is clicked
 *
 *  \iparam Btn - button index
 */
/****************************************************************************/
void CSubSpeller::BtnClicked(int Btn)
{
    hide();
    QString KeyChar = m_SubSpellerList.at(Btn)->GetText1();
    mp_LineEdit->insert(KeyChar);
    mp_ParentButton->SetSubSpellerOff();
    emit SubSpellerClicked();
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
 */
/****************************************************************************/
CKeyBoardButton *CSubSpeller::CreateNewKey(QString IconType,
                                           QString BtnText1, QString BtnText2,
                                           bool IconPresent)
{
    mp_KeyBoardButton = new CKeyBoardButton(this,IconType, BtnText1, BtnText2, IconPresent);
    mp_KeyBoardButton->SetPressAndHold();
    return mp_KeyBoardButton;
}

/****************************************************************************/
/*!
 *  \brief Draws the subspeller background
 */
/****************************************************************************/
void CSubSpeller::paintEvent(QPaintEvent *p_PaintEvent)
{
      Q_UNUSED(p_PaintEvent);
      QPainter Painter(this);
      Painter.drawPixmap(0,0,m_Pixmap);
}

}//End of namespace KeyBoard
