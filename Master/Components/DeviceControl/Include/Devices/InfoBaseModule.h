/****************************************************************************/
/*! \file   InfoBaseModule.h
 *
 *  \brief  Definition file for class CInfoBaseModule
 *
 *  \version  0.1
 *  \date     2013-01-07
 *  \author   M.Scherer
 *
 *  \b Description:
 *
 *   N/A
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#ifndef DEVICECONTROL_INFOBASEMODULE_H
#define DEVICECONTROL_INFOBASEMODULE_H

#include "DeviceState.h"
#include "DeviceControl/Include/Global/DeviceControl.h"

#ifdef Q_UNIT_TEST
#include "Simulator.h"
#endif

namespace DataManager {
    class CSubModule;
}

namespace DeviceControl
{

class CBaseModule;

/****************************************************************************/
/*!
 *  \brief  Fetches the service information from the base module
 */
/****************************************************************************/
class CInfoBaseModule : public CState
{
    Q_OBJECT

public:
    explicit CInfoBaseModule(CBaseModule *p_BaseModule, DataManager::CSubModule *p_SubModule, QState *p_Parent = 0);

signals:
    /****************************************************************************/
    /*!
     *  \brief  Emitted when an error occurred during communication
     *
     *  \iparam ReturnCode = Error code
     */
    /****************************************************************************/
    void ReportError(ReturnCode_t ReturnCode);

private:
    bool ReqSerialNumber(QEvent *p_Event);
    bool ReqEndTestResult(QEvent *p_Event);
    bool ReqHWInfo(QEvent *p_Event);
    bool ReqSWInfo(QEvent *p_Event);
    bool ReqLoaderInfo(QEvent *p_Event);
    bool ReqLifeCycleData(QEvent *p_Event);
    bool ReqLaunchDate(QEvent *p_Event);
    bool ReqBoardName(QEvent *p_Event);
    bool ReqUniqueNumber(QEvent *p_Event);
    bool Finished(QEvent *p_Event);

    CBaseModule *mp_BaseModule;             //!< Base module used to get the information
    DataManager::CSubModule *mp_SubModule;  //!< Data container in which the data is stored
};

} //namespace

#endif // DEVICECONTROL_INFOBASEMODULE_H
