/****************************************************************************/
/*! \file DateTime.h
 *
 *  \brief DateTime definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-06-21
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

#ifndef MAINMENU_DATETIME_H
#define MAINMENU_DATETIME_H

#include "Global/Include/Utils.h"
#include "Global/Include/Exception.h"
#include "MainMenu/Include/ScrollWheel.h"
#include "MainMenu/Include/MainWindow.h"
#include <QDateTime>
#include <QWidget>

namespace MainMenu {

namespace Ui {
    class CDateTime;
}

/****************************************************************************/
/**
 * \brief Date and time can be set with the scroll wheels displayed in this
 *        widget.
 */
/****************************************************************************/
class CDateTime : public QWidget
{
    Q_OBJECT

public:
    explicit CDateTime(QWidget *p_Parent = 0);
    virtual ~CDateTime();

    void RefreshDateTime(Global::TimeFormat TimeFormat);
    QDateTime &GetDateTime();
    void SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow);
    void ResetButtons();

signals:
    /****************************************************************************/
    /*!
     *  \brief Forwards the date and time to the main clock
     *
     *  \iparam DateTime = Date and time
     */
    /****************************************************************************/
    void ApplyData(const QDateTime DateTime);

private:
    Ui::CDateTime *mp_Ui;   //!< User interface
    MainMenu::CScrollWheel *mp_DayWheel;    //!< Day scroll wheel
    MainMenu::CScrollWheel *mp_MonthWheel;  //!< Month scroll wheel
    MainMenu::CScrollWheel *mp_YearWheel;   //!< Year scroll wheel
    MainMenu::CScrollWheel *mp_HourWheel;   //!< Hour scroll wheel
    MainMenu::CScrollWheel *mp_MinWheel;    //!< Minute scroll wheel
    QDateTime m_DateTime;   //!< Date and time displayed
    MainMenu::CMainWindow *mp_MainWindow;           //!< Pointer to MainWindow
    //Flags
    bool m_ProcessRunning;                  //!< Process running state
    void RetranslateUI();

protected:
    void changeEvent(QEvent *p_Event);

private slots:
    void CollectData(bool Send = true);
    void OnProcessStateChanged();
};

} // end namespace MainMenu

#endif // MAINMENU_DATETIME_H
