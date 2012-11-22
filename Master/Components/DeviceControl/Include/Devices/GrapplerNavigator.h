/****************************************************************************/
/*! \file GrapplerNavigator.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 24.08.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the definition of the CGrapplerNavigator class.
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

#ifndef GRAPPLERNAVIGATOR_H
#define GRAPPLERNAVIGATOR_H

#include <QObject>
#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/GrapplerDevice.h"
#include "DeviceControl/Include/Devices/BaseDeviceNavigator.h"

namespace DeviceControl
{

class CStepperMotor;
class CSRfid11785;
class CAnalogInput;
class GrapplerDevTask;


/*! task list for movement and measurement actions */
typedef QList<GrapplerDevTask*> GrapplerDevTaskList;

/*#define LAYOUT_YPOS_COL1   532
#define LAYOUT_YPOS_COL2  1596
#define LAYOUT_YPOS_COL3  2660
#define LAYOUT_YPOS_COL4  3723*/

/****************************************************************************/
/*! \class CGrapplerNavigator
 *
 *  \brief CGrapplerNavigator implements methods used by CGrapplerDevice for grappler navigation
 *         A incoming action request will be split into several tasks, e.g. motor positioning
 *         of x-,y- z-axis or RFID.
 *
 *         For correct motor positioning, the class keeps the positions of the vessels and the adjustment datas
 *
 */
/****************************************************************************/
class CGrapplerNavigator : public CBaseDeviceNavigator
{
public:
    CGrapplerNavigator();
    virtual ~CGrapplerNavigator();

    /*! requested action for grappler positioning */
    typedef enum {
        NAVIGATOR_ACTION_GRAB                 = 0x00,
        NAVIGATOR_ACTION_DISCARD              = 0x01,
        NAVIGATOR_ACTION_READ_RFID            = 0x02,
        NAVIGATOR_ACTION_READ_LIQUID_LEVEL    = 0x03,
        NAVIGATOR_ACTION_MOVEOVER             = 0x04,
        NAVIGATOR_ACTION_REFERENCE_RUN        = 0x05,   //internal use only
        NAVIGATOR_ACTION_MOTOR_POSITION_REQ   = 0x06    //internal use only
    } NavigatorAction_t;

    ReturnCode_t CreateReferenceRunFctModTasks(CGrapplerDevice* pGrapplerDev, FunctionModuleTaskMap& taskMap);
    ReturnCode_t CreatePositionReqFctModTasks(CGrapplerDevice* pGrapplerDev, FunctionModuleTaskMap& taskMap);
    ReturnCode_t CreateStationActionFctModTasks(CGrapplerDevice* pGrapplerDev, FunctionModuleTaskMap& taskMap);

    ReturnCode_t CalculatePositionRequestTasks(CGrapplerDevice* pGrappler,
                                               FunctionModuleTaskMap& taskMap);

    ReturnCode_t CalculateGrapplerActionTasks(CGrapplerDevice* pGrappler,
                                              FunctionModuleTaskMap& taskMap,
                                              qint16 stationColumn,
                                              qint16 stationRow,
                                              StationAction_t stationAktion,
                                              bool TimeRequest);

private:
    void GetStationXYPositionFromRowCol(DevInstanceID_t grapplerID,
                                        qint16 stationRow,
                                        qint16 stationCol,
                                        StationAction_t stationAction,
                                        Position_t &posX,
                                        Position_t &posY);

    void CorrectMotorPosAccordingAction(Position_t& stationPosX, Position_t& stationPosY, StationAction_t stationAction);
};

} //namespace


#endif
