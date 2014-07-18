/****************************************************************************/
/*! \file   ResetModule.h
 *
 *  \brief  Definition file for class CResetModule
 *
 *  \version  0.1
 *  \date     2013-01-15
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

#ifndef DEVICECONTROL_RESETMODULE_H
#define DEVICECONTROL_RESETMODULE_H

#include "DeviceState.h"
#include "DeviceControl/Include/Global/DeviceControlReturnCode.h"

#ifdef Q_UNIT_TEST
#include "Simulator.h"
#endif

namespace DeviceControl
{

class CModule;

/****************************************************************************/
/*!
 *  \brief  Used for modules implementing the basic data reset functionality
 */
/****************************************************************************/
class CResetModule : public CState
{
    Q_OBJECT

public:
    explicit CResetModule(CModule *p_Module, const QString &Name, QState *p_Parent = 0);

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
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CResetModule)

    bool ReqDataReset(QEvent *p_Event);
    bool ReportDataResetAckn(QEvent *p_Event);

    CModule *mp_Module; //!< Module whose data is reset
};

} //namespace

#endif // DEVICECONTROL_RESETMODULE_H
