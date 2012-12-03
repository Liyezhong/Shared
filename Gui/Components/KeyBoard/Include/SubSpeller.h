/****************************************************************************/
/*! \file SubSpeller.h
 *
 *  \brief SubSpeller Class declaration for keyboard
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
 */
/****************************************************************************/

#ifndef SUBSPELLER_H
#define SUBSPELLER_H

//#include "KeyBoard/Include/KeyBoard.h"
#include "KeyBoard/Include/KeyBoardButton.h"
#include "KeyBoard/Include/KeyBoard.h"
#include <QDialog>
#include <QPainter>
#include <QHBoxLayout>
#include <QSignalMapper>
#include <QApplication>

namespace KeyBoard {

/****************************************************************************/
/**
 * \brief Subspeller dialog-popup a subspeller for certain keys
 */
/****************************************************************************/
class CSubSpeller:public QDialog
{
Q_OBJECT
private:
    struct SubSpellerLookUp
    {
        qint32 Button1Ascii[7];//! < Store Ascii value of the subspeller text
        qint16 Size1XPos;//! < X co-ordinate for SIZE_1
        qint16 Size1YPos;//! < y co-ordinate for SIZE_1
        qint16 Size2XPos;//! < x co-ordinate for SIZE_2
        qint16 Size2YPos;//! < y co-ordinate for SIZE_2
        qint16 NoOfSubSpeller;//! < No of buttons in subspeller
    };//! < Store Subspeller info
    QPixmap m_Pixmap; //! < store Subspeller Background image
    QHBoxLayout *mp_HorizontalLayout;//! < lay the buttons horizontally
    static const SubSpellerLookUp m_SubSpellerLookUp[56];//! < Holds Subspeller info
    CKeyBoard *mp_Parent;//! < pointer to parent widget
    QSignalMapper *mp_SubSpellerClickedMapper;//! <Signal mapper element used when a subspeller is clicked.
    QList <CKeyBoardButton *> m_SubSpellerList; //! < used to store pointers to all subspeller buttons created
    CKeyBoardButton *mp_KeyBoardButton;//! < Subspeller buttons
    CKeyBoardButton *mp_ParentButton;//! < Parent button that poped the subspeller
    QSpacerItem *mp_SpacerItem;//! < This spacer is used to fix the spacing issue between keys when more than 2 keys are present in subspeller
    QLineEdit *mp_LineEdit;
    CKeyBoardButton *CreateNewKey(QString IconType, QString BtnText1, QString BtnText2, bool IconPresent);


public:
    static QString m_Model;//! < Used to check if model is SIZE_1 or SIZE_2
    static qint32 m_SubspellerOffsetW;//! < width offset for subspeller popup
    static qint32 m_SubspellerOffsetH;//! < height offset for subspeller popup
    static qint32 m_SubspellerMarginRight;//! < subspeller popup right margin
    static qint32 m_SubspellerBorderMarginLeft;//! < Margins passed to leica style Borderpixmap api
    static qint32 m_SubspellerBorderMarginTop;//! < Margins passed to leica style Borderpixmap api
    static qint32 m_SubspellerBorderMarginBottom;//! < Margins passed to leica style Borderpixmap api
    static qint32 m_SubspellerBorderMarginRight;//!< Margins passed to leica style Borderpixmap api
    explicit CSubSpeller(CKeyBoard *p_Parent = 0,CKeyBoardButton *p_KeyBoardButton = 0,
                         qint32 Offset = 0, qint32 LookUpIndex = 0,KeyBoard::Model_t = SIZE_1,
                         KeyBoard::KeyBoardType_t = QWERTZ_KEYBOARD);
    ~CSubSpeller();
    void paintEvent(QPaintEvent *);

signals:
    void SubSpellerClicked();

private slots:
    void BtnClicked(int Btn);
};

} // End of namespace KeyBoard
#endif // SUBSPELLER_H
