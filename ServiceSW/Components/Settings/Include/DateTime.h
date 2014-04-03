/****************************************************************************/
/*! \file DateTime.h
 *
 *  \brief Header file for class CDateTime.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-06-21
 *   $Author:  $ M.Scherer, Soumya. D
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

#ifndef SETTINGS_DATETIME_H
#define SETTINGS_DATETIME_H

#include "../Include/PlatformServiceDefines.h"
#include "Global/Include/Utils.h"
#include "Global/Include/Exception.h"
#include "Settings/Include/ScrollWheelWidget.h"
#include "MainMenu/Include/MainWindow.h"
#include <QDateTime>
#include <QWidget>

namespace Settings {

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
    int GetDateTimeOffSet();
    void SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow);
    void ResetButtons();
    void SetApplyButtonStatus(PlatformService::SoftwareModeType_t SoftwareMode);

protected:
    void changeEvent(QEvent *p_Event);

private:
    Ui::CDateTime *mp_Ui;   //!< User interface
    Settings::CScrollWheelWidget *mp_DayWheel;    //!< Day scroll wheel
    Settings::CScrollWheelWidget *mp_MonthWheel;  //!< Month scroll wheel
    Settings::CScrollWheelWidget *mp_YearWheel;   //!< Year scroll wheel
    Settings::CScrollWheelWidget *mp_HourWheel;   //!< Hour scroll wheel
    Settings::CScrollWheelWidget *mp_MinWheel;    //!< Minute scroll wheel
    QDateTime m_DateTime;                         //!< Date time object
    MainMenu::CMainWindow *mp_MainWindow;         //!< Pointer to MainWindow

    //Flags
    bool m_ProcessRunning;                        //!< Process running state
    int m_OffSetSeconds;                          //!< Offset
    void RetranslateUI();

signals:
    /****************************************************************************/
    /*!
     *  \brief Forwards the date and time to the main clock
     *
     *  \iparam OffSet = Send offset value of the time
     */
    /****************************************************************************/
    void ApplyData(const QDateTime& DateTime);

private slots:
    void CollectData(bool Send = true);
    void OnProcessStateChanged();   

};

} // end namespace Settings

#endif // SETTINGS_DATETIME_H
