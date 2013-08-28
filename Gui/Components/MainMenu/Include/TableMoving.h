/****************************************************************************/
/*! \file TableMoving.h
 *
 *  \brief TableMoving definition.
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

#ifndef MAINMENU_TABLEMOVING_H
#define MAINMENU_TABLEMOVING_H


#include <QGroupBox>

namespace MainMenu {

namespace Ui {
    class CTableMoving;
}

/****************************************************************************/
/**
 * \brief A set of buttons for the movement of rows in table views
 */
/****************************************************************************/
class CTableMoving : public QGroupBox
{
    Q_OBJECT

public:
    explicit CTableMoving(QWidget *p_Parent = 0);
    virtual ~CTableMoving();

private:
    Ui::CTableMoving *mp_Ui;    //!< User interface

protected:
    void changeEvent(QEvent *p_Event);

signals:
    /****************************************************************************/
    /*!
     *  \brief Move the row to the beginning of the table
     */
    /****************************************************************************/
    void OnBeginButtonClicked();
    /****************************************************************************/
    /*!
     *  \brief Move the item one row up
     */
    /****************************************************************************/
    void OnUpButtonClicked();
    /****************************************************************************/
    /*!
     *  \brief Move the item one row down
     */
    /****************************************************************************/
    void OnDownButtonClicked();
    /****************************************************************************/
    /*!
     *  \brief Move the row to the end of the table
     */
    /****************************************************************************/
    void OnEndButtonClicked();
};

} // end namespace MainMenu

#endif // MAINMENU_TABLEMOVING_H
