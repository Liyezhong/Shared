/****************************************************************************/
/*! \file DeviceLight.h
 *
 *  \brief  Definition file for class CDeviceLight
 *
 *  \version  0.1
 *  \date     2012-10-25
 *  \author   Thirumalesha R
 *
 *  \b Description:
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

#ifndef DEVICELIGHT_H
#define DEVICELIGHT_H

#include "DeviceBase.h"

namespace DeviceControl
{

class CDigitalOutput;

/****************************************************************************/
/*! \brief Device class to handle Light device tasks
 *
 *  This class signals upper layer (DCP) when Light Status is changed. Also
 *  it emits appropriate signal when DCP requests status. \n
 *  Signals: \n
 *      Request interface to DCP: \n
 *          SetLightOn()\n
 *          SetLightOff()\n
 *
 *      Reponse Interface to DCP: \n
 *          LightOn(ReturnCode_t)\n
 *          LightOff(ReturnCode_t)\n
 *          LedError(ReturnCode_t)\n
 */
 /****************************************************************************/
class CDeviceLight : public CDeviceBase
{
    Q_OBJECT

public:
    CDeviceLight(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList,
                 DevInstanceID_t InstanceID);

signals:
    //command request interface to DCP
    /****************************************************************************/
    /*! \brief Interface to DCP to Set Light On.
    */
    /****************************************************************************/
    void SetLightOn();

    /****************************************************************************/
    /*! \brief Interface to DCP to Set Light Off.
    */
    /****************************************************************************/
    void SetLightOff();

    // command response interface to DCP
    /****************************************************************************/
    /*! \brief Emits when LightOn is Set.
    */
    /****************************************************************************/
    void LightOn(ReturnCode_t);

    /****************************************************************************/
    /*! \brief Emits when LightOff is Set.
    */
    /****************************************************************************/
    void LightOff(ReturnCode_t);

    /****************************************************************************/
    /*! \brief If Call to Function Module is unsuccessful, Emits Error Signal.
    */
    /****************************************************************************/
    void LedError(ReturnCode_t);

private:
    bool Trans_LightOff_SwitchingOn(QEvent *p_Event);
    bool Trans_SwitchingOn_LightOn(QEvent *p_Event);
    bool Trans_LightOn_SwitchingOff(QEvent *p_Event);
    bool Trans_SwitchingOff_LightOff(QEvent *p_Event);
    bool Trans_Configure(QEvent *p_Event);
    bool Start(QEvent *p_Event);

    CBaseModule *mp_BaseModule;
    CDigitalOutput *mp_FmDoLed;  ///< pointer to LED

protected slots:
    virtual void ThreadStarted();
};

} //namespace

#endif // DEVICELIGHT_H
