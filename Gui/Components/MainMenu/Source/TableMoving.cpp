/****************************************************************************/
/*! \file TableMoving.cpp
 *
 *  \brief TableMoving implementation.
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

    connect(mp_Ui->beginButton, SIGNAL(clicked()), this, SIGNAL(OnBeginButtonClicked()));
    connect(mp_Ui->upButton, SIGNAL(clicked()), this, SIGNAL(OnUpButtonClicked()));
    connect(mp_Ui->downButton, SIGNAL(clicked()), this, SIGNAL(OnDownButtonClicked()));
    connect(mp_Ui->endButton, SIGNAL(clicked()), this, SIGNAL(OnEndButtonClicked()));
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
            mp_Ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

} // end namespace MainMenu
