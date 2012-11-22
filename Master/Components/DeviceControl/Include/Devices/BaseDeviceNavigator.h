/****************************************************************************/
/*! \file BaseDeviceNavigator.h
 *
 *  \brief
 *
 *   Version: 0.1
 *   Date:    16.02.2011
 *   Author:  Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaation of the class CBaseDeviceNavigator
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2011 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/
#ifndef BASEDEVICENAVIGATOR_H
#define BASEDEVICENAVIGATOR_H

#include "DeviceControl/Include/Devices/FunctionModuleTaskManager.h"
#include <QObject>

namespace DeviceControl
{

class DeviceTask;
class CStepperMotor;
class CRfid1178;
class CAnalogInput;
class FunctionModuleTask;
class CANStepperMotorTask;

/****************************************************************************/
/*! \class CBaseDeviceNavigator
 *
 *  \brief This is the base class of the device navigator classes
 *
 *         The class provides navigaton relevant base functions
 */
/****************************************************************************/
class CBaseDeviceNavigator
{

public:
    CBaseDeviceNavigator();
    virtual ~CBaseDeviceNavigator();

    void ResetDeviceTask( DeviceTask* pDeviceTask);
};

} //namespace

#endif // BASEDEVICENAVIGATOR_H
