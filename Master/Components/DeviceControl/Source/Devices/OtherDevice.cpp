#include "DeviceControl/Include/Devices/OtherDevice.h"
#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/DeviceProcessing/DeviceLifeCycleRecord.h"
#include "DeviceControl/Include/SlaveModules/OtherModule.h"

namespace DeviceControl
{
/****************************************************************************/
/*!
 *  \brief    Constructor of the COtherDevice class
 *
 *
 *  \param    pDeviceProcessing = pointer to DeviceProcessing
 *  \param    Type = Device type string
 *  \param    ModuleList module list
 *  \param    InstanceID instance id
 */
/****************************************************************************/
COtherDevice::COtherDevice(DeviceProcessing* pDeviceProcessing, QString& Type,
                         const DeviceModuleList_t &ModuleList,
                         quint32 InstanceID) :
    CBaseDevice(pDeviceProcessing, Type, ModuleList, InstanceID)
{
    m_MainStateOld = m_MainState = DEVICE_MAIN_STATE_START;
    m_mapModules["EBox"] = OD_EBox;
    m_mapModules["VentilationFan"] = OD_VentilationFan;
    m_mapModules["TouchScreen"] = OD_TouchScreen;
    m_mapModules["PressureSensor"] = OD_Pressure_Sensor;
}

/****************************************************************************/
/*!
 *  \brief  Destructor of COtherDevice
 */
/****************************************************************************/
COtherDevice::~COtherDevice()
{
}



/****************************************************************************/
/*!
 *  \brief  Handles the internal state machine
 *
 *      This function should be called periodically, it handles the state
 *      machine of the class. Furthermore, the HandleTask-function of the
 *      interface class will be called. Each main state has a individual
 *      handling function, which will be called according to the currently
 *      active main state.
 */
/****************************************************************************/
void COtherDevice::HandleTasks()
{
    ReturnCode_t RetVal;

    if(m_MainState == DEVICE_MAIN_STATE_IDLE)
    {
    }
    else if(m_MainState == DEVICE_MAIN_STATE_START)
    {
        m_MainState = DEVICE_MAIN_STATE_INIT;
    }
    else if(m_MainState == DEVICE_MAIN_STATE_INIT)
    {
        RetVal = HandleInitializationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_CONFIG;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_CONFIG)
    {
        m_MainState = DEVICE_MAIN_STATE_FCT_MOD_CFG;
    }
    else if(m_MainState == DEVICE_MAIN_STATE_FCT_MOD_CFG)
    {
        m_MainState = DEVICE_MAIN_STATE_IDLE;
    }
    else if(m_MainState == DEVICE_MAIN_STATE_ERROR)
    {
        
    }

    if(m_MainStateOld != m_MainState)
    {
        m_MainStateOld = m_MainState;
    }
}

ReturnCode_t COtherDevice::InitFunctionModuleLifeTime(const QString& FctModKey, const QString& modName)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint32 InstanceID = GetFctModInstanceFromKey(FctModKey);

    m_pOtherModules[m_mapModules[modName]] = (COtherModule*) m_pDevProc->GetFunctionModule(InstanceID);
    if(m_pOtherModules[m_mapModules[modName]] == 0)
    {
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }
    else
    {
        if (m_ModuleLifeCycleRecord)
        {
            PartLifeCycleRecord* pPartLifeCycleRecord = m_ModuleLifeCycleRecord->m_PartLifeCycleMap.value(modName);
            if (pPartLifeCycleRecord)
            {
                m_pOtherModules[m_mapModules[modName]]->SetPartLifeCycleRecord(pPartLifeCycleRecord);
            }
        }
    }
    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief   Handles the classes initialization state.
 *
 *           This function attaches the function modules pointer variables
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS or error return code
 */
/****************************************************************************/
ReturnCode_t COtherDevice::HandleInitializationState()
{
    ReturnCode_t RetVal = InitFunctionModuleLifeTime(CANObjectKeyLUT::m_OtherDeviceEBoxKey, "EBox");
    if (DCL_ERR_FCT_CALL_FAILED == RetVal)
        return RetVal;

    RetVal = InitFunctionModuleLifeTime(CANObjectKeyLUT::m_OtherDeviceTouchScreenKey, "TouchScreen");
    if (DCL_ERR_FCT_CALL_FAILED == RetVal)
        return RetVal;

    RetVal = InitFunctionModuleLifeTime(CANObjectKeyLUT::m_OtherDeviceVentilationFanKey, "VentilationFan");
    if (DCL_ERR_FCT_CALL_FAILED == RetVal)
        return RetVal;

    RetVal = InitFunctionModuleLifeTime(CANObjectKeyLUT::m_OtherDevicePressureSensor, "PressureSensor");
    if (DCL_ERR_FCT_CALL_FAILED == RetVal)
        return RetVal;
    return DCL_ERR_FCT_CALL_SUCCESS;
}



} //namespace
