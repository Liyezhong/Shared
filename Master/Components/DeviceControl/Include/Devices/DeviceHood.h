/****************************************************************************/
/*! \file DeviceHood.h
 *
 *  \brief  Definition file for class CDeviceHood
 *
 *  \version  0.1
 *  \date     2012-09-26
 *  \author   Torsten Scheck
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

#ifndef DEVICECONTROL_HOOD_H
#define DEVICECONTROL_HOOD_H

#include "DeviceBase.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"

namespace DeviceControl
{

/****************************************************************************/
/*! \brief Device class to handle Hood device tasks
 *
 *  This class signals upper layer (DCP) when Hood Status is changed. Also
 *  it emits appropriate signal when DCP requests status. \n
 *  Signals: \n
 *      Request interface to DCP: \n
 *          IsClosed()\n
 *
 *      Reponse Interface to DCP: \n
 *          ReportIsClosed()\n
 *          Opened()\n
 *          Closed()\n
 *
 */
 /****************************************************************************/
class CDeviceHood : public CDeviceBase
{
    Q_OBJECT

public:
    CDeviceHood(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList, DevInstanceID_t InstanceID);

signals:
    //command request interface to DCP
    /****************************************************************************/
    /*! \brief Interface to DCP to identify if hood is closed.
     */
    /****************************************************************************/
    void IsClosed();

    // command response interface to DCP
    /****************************************************************************/
    /*! \brief Response to IsClosed() request.
     *
     *  \iparam Result result code sent by FM layer
     *  \iparam Closed true, if hood is closed.
     */
    /****************************************************************************/
    void ReportIsClosed(ReturnCode_t Result, bool Closed);

    // Independent signals to DCP
    /****************************************************************************/
    /*! \brief Emits when hood is opened
     */
    /****************************************************************************/
    void Opened();

    /****************************************************************************/
    /*! \brief Emits when hood is closed
     */
    /****************************************************************************/
    void Closed();

protected:
    bool Start(QEvent *p_Event);
    bool GuardCheckOpen(QEvent *p_Event);
    bool GuardCheckClosed(QEvent *p_Event);
    bool CheckSignalForValue(QEvent *p_Event, quint16 CheckValue);
    bool AckIsClosedTrue(QEvent *p_Event);
    bool AckIsClosedFalse(QEvent *p_Event);
    bool DefaultNackIsClosed(QEvent *p_Event);

    bool Trans_Configure(QEvent *p_Event);

private:
    CDigitalInput *mp_HoodSensor;

};

} //namespace

#endif // DEVICECONTROL_HOOD_H
 
// vi: set ts=4 sw=4 et:

