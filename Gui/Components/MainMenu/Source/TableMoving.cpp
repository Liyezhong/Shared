/****************************************************************************/
/*! \file TableMoving.cpp
 *
 *  \brief Implementation of file for class CTableMoving.
 *
 *  \b Description:
 *          This class implements a base widget,which will give the moving
 *          functionality with in a Table widget.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-08-23
 *   $Author:  $ M.Scherer
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

#include "MainMenu/Include/TableMoving.h"
#include "ui_TableMoving.h"

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CTableMoving::CTableMoving(QWidget *p_Parent) : QGroupBox(p_Parent), mp_Ui(new Ui::CTableMoving)
{
    mp_Ui->setupUi(this);

    (void) connect(mp_Ui->beginButton, SIGNAL(clicked()), this, SIGNAL(OnBeginButtonClicked()));
    (void) connect(mp_Ui->upButton, SIGNAL(clicked()), this, SIGNAL(OnUpButtonClicked()));
    (void) connect(mp_Ui->downButton, SIGNAL(clicked()), this, SIGNAL(OnDownButtonClicked()));
    (void) connect(mp_Ui->endButton, SIGNAL(clicked()), this, SIGNAL(OnEndButtonClicked()));
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CTableMoving::~CTableMoving()
{
    delete mp_Ui;
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/

void CTableMoving::changeEvent(QEvent *p_Event)
{
    QGroupBox::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            break;
        default:
            break;
    }
}

/****************************************************************************/
/*!
 *  \brief Sets title as per the maximum title string length.
 *
 *  \iparam StringLength = Maximum string lenght of dialog title.
 *  \iparam TitleText = String to be set
 *
 */
/****************************************************************************/
void CTableMoving::SetTitle(qint32 StringLength, QString TitleText)
{
    if (TitleText.length() > StringLength) {
        (void) TitleText.remove(StringLength, TitleText.length() - StringLength);
    }
    setTitle(TitleText);
}

} // end namespace MainMenu
