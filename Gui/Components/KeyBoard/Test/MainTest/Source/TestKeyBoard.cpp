/****************************************************************************/
/*! \file TestKeyBoard.cpp
 *
 *  \brief Unit test for KeyBoard widget
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-12-18
 *  $Author:    $ N.Kamath
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

#include <QTest>
#include <QDebug>
#include "../../../../KeyBoard/Include/KeyBoard.h"
#include <QObject>

namespace KeyBoard {

/****************************************************************************/
/**
 * \brief Test class for KeyBoard class.
 */
/****************************************************************************/
class CTestKeyBoard : public QObject {
    Q_OBJECT
private slots:
    /****************************************************************************/
    /**
     * \brief Called before the first testfunction is executed.
     */
    /****************************************************************************/
    void initTestCase();
    /****************************************************************************/
    /**
     * \brief Called before each testfunction is executed.
     */
    /****************************************************************************/
    void init();
    /****************************************************************************/
    /**
     * \brief Called after each testfunction was executed.
     */
    /****************************************************************************/
    void cleanup();
    /****************************************************************************/
    /**
     * \brief Called after last testfunction was executed.
     */
    /****************************************************************************/
    void cleanupTestCase();

    /****************************************************************************/
    /**
     * \brief Test  Program sequence object
     */
    /****************************************************************************/
    void utTestKeyBoard();

}; // end class TestDataReagentList

/****************************************************************************/
void CTestKeyBoard::initTestCase() {
}

/****************************************************************************/
void CTestKeyBoard::init() {
}

/****************************************************************************/
void CTestKeyBoard::cleanup() {
}

/****************************************************************************/
void CTestKeyBoard::cleanupTestCase() {
}

/****************************************************************************/
void CTestKeyBoard::utTestKeyBoard() {
     KeyBoard::CKeyBoard *p_KeyBoardWidget = new KeyBoard::CKeyBoard;
     p_KeyBoardWidget->SetPasswordMode(true);
     QCOMPARE(p_KeyBoardWidget->mp_LineEdit->echoMode(), QLineEdit::Password);
     p_KeyBoardWidget->SetMaxCharLength(16);
     QCOMPARE(p_KeyBoardWidget->m_MaxEnteredCharLen, (unsigned int)16);
     p_KeyBoardWidget->SetMinCharLength(4);
     QCOMPARE(p_KeyBoardWidget->m_MinEnteredCharLen,(unsigned int) 4);
     p_KeyBoardWidget->mp_LineEdit->setText("Test");
     QCOMPARE(p_KeyBoardWidget->GetLineEditString(), QString("Test"));
     p_KeyBoardWidget->MoveCharacters(true);
     p_KeyBoardWidget->SetNumericMode(true);
     p_KeyBoardWidget->SetLineEditContent("Test String");
     QCOMPARE(p_KeyBoardWidget->GetLineEditString(), QString("TestTest String"));
     p_KeyBoardWidget->DisplayNumericKeyBoard();
     p_KeyBoardWidget->SetCaps(true);
     p_KeyBoardWidget->InitSizeAttributes();
     p_KeyBoardWidget->CreateLineEditWidget();
     p_KeyBoardWidget->MoveKeyboard(35,100);
     qint32 XPos = 0;
     qint32 YPos = 0;
     p_KeyBoardWidget->GetKeyboardCoordinates(XPos, YPos);
     QCOMPARE(XPos, 35);
     QCOMPARE(YPos, 100);
     p_KeyBoardWidget->GetDisplayMode();
     p_KeyBoardWidget->GetCapsStatus();
}

} // end namespace KeyBoard

QTEST_MAIN(KeyBoard::CTestKeyBoard)
#include "TestKeyBoard.moc"

