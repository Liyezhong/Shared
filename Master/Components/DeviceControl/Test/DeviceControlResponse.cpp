#include <unistd.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include <DeviceControl/Test/DeviceControlResponse.h>
#include <Global/Include/SystemPaths.h>
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include "DeviceControl/Include/Interface/IDeviceProcessing.h"
#include "DeviceControl/Include/Interface/Devices/ILoaderDevice.h"

using std::cout;

namespace DeviceControl {



DeviceProcessingResponse::DeviceProcessingResponse()

{
    // create a instance of the device control layer (the one and only instance)
    m_bInitialisationFinished = false;
    m_bConfigurationFinished   = false;

    Global::SystemPaths::Instance().SetSettingsPath("../../../../../../Himalaya/HimalayaMain/Master/Components/Main/Build/Settings");


    m_pIDeviceProcessing = new IDeviceProcessing();
    m_pIDeviceProcessing->SetIResponse(this);


    m_pIDeviceProcessing->Start();

}

DeviceProcessingResponse::~DeviceProcessingResponse()
{
    m_pIDeviceProcessing->Destroy();

    delete m_pIDeviceProcessing;
    m_pIDeviceProcessing = 0;
}

QString DeviceProcessingResponse::GetSerialNumber()
{
    QString SerialNo;

    if(m_pIDeviceProcessing)
    {
        m_pIDeviceProcessing->GetSerialNumber(SerialNo);
    }

    return SerialNo;
}

void DeviceProcessingResponse::Start()
{
    if(m_pIDeviceProcessing)
    {
        m_pIDeviceProcessing->Start();
    }
}

void DeviceProcessingResponse::StartConfiguration()
{
    ReturnCode_t RetCode;

    if(m_pIDeviceProcessing)
    {
        RetCode = m_pIDeviceProcessing->StartConfigurationService();
    }
}

bool DeviceProcessingResponse::RequestCANNodes()
{
    bool Result = false;
    CBaseModule* pICANNode = 0;

    if(m_pIDeviceProcessing)
    {
        quint16 HWVersion, SWVersion;
        pICANNode = m_pIDeviceProcessing->GetINode(0);
        if(pICANNode)
        {
            Result = true;
        }

        while(pICANNode)
        {
            HWVersion = pICANNode->GetHWVersion();
            SWVersion = pICANNode->GetSWVersion();

            if((HWVersion != 1) || (SWVersion != 1))
            {
                Result = false;
            }

            pICANNode = m_pIDeviceProcessing->GetINode(pICANNode);
        }
    }

    return Result;

}

void DeviceProcessingResponse::ThrowError(DevInstanceID_t InstanceID, quint16 usErrorGroup, quint16 usErrorID, quint16 usErrorData,const QDateTime & /*TimeStamp*/)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(usErrorGroup);
    Q_UNUSED(usErrorID);
    Q_UNUSED(usErrorData);
    Q_UNUSED(usErrorGroup);

}

void DeviceProcessingResponse::DevProcInitialisationAckn(DevInstanceID_t InstanceID, ReturnCode_t configResult)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(configResult);

    m_bInitialisationFinished = true;
}

void DeviceProcessingResponse::DevProcConfigurationAckn(DevInstanceID_t InstanceID, ReturnCode_t hdlInfo)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(hdlInfo);

    m_bConfigurationFinished = true;
}

void DeviceProcessingResponse::DevProcStartNormalOpModeAckn(DevInstanceID_t InstanceID, ReturnCode_t hdlInfo)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(hdlInfo);
}

void DeviceProcessingResponse::DevProcModeChangeInfo(DevInstanceID_t /*InstanceID*/, ReturnCode_t /*hdlInfo*/, quint8 /*newState*/)
{
}

void DeviceProcessingResponse::StartAdjustmentServiceAckn(DevInstanceID_t InstanceID, ReturnCode_t hdlInfo)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(hdlInfo);
}

void DeviceProcessingResponse::GrapplerReferenceRunAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo)
{
    emit ReportGrapplerReferenceRunAckn(InstanceID, HdlInfo);
}

void DeviceProcessingResponse::GrapplerPositioningAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo)
{
    emit ReportGrapplerPositioningAckn(InstanceID, HdlInfo);
}

// rfid data have been read ( result of ReqStationAction(.., STATION_ACTION_READ_RFID)
void DeviceProcessingResponse::GrapplerReportRFIDData(DevInstanceID_t InstanceID, ReturnCode_t hdlInfo, quint32 rfidMSB, quint32 rfidLSB)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(hdlInfo);
    Q_UNUSED(rfidMSB);
    Q_UNUSED(rfidLSB);
}

void DeviceProcessingResponse::GrapplerReportLiquidLevel(DevInstanceID_t InstanceID, ReturnCode_t hdlInfo, quint16 liquidLevelValue)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(hdlInfo);
    Q_UNUSED(liquidLevelValue);
}

void DeviceProcessingResponse::LoaderReferenceRunAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, LoaderPosition_t Position)
{
    emit ReportLoaderReferenceRunAckn(InstanceID, HdlInfo, Position);
}

void DeviceProcessingResponse::LoaderReportActDrawerPosition(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, LoaderPosition_t position)
{
    Q_UNUSED(instanceID);
    Q_UNUSED(hdlInfo);
    Q_UNUSED(position);
}

void DeviceProcessingResponse::LoaderReportDrawerUserAction(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, LoaderPosition_t position, BlockState_t BlockState)
{
    Q_UNUSED(instanceID);
    Q_UNUSED(hdlInfo);
    Q_UNUSED(position);
    Q_UNUSED(BlockState);
}

void DeviceProcessingResponse::LoaderReportRFIDData(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, LoaderRFIDChannel_t rfidChannel, quint32 rfidMSB, quint32 rfidLSB)
{
    Q_UNUSED(instanceID);
    Q_UNUSED(hdlInfo);
    Q_UNUSED(rfidChannel);
    Q_UNUSED(rfidMSB);
    Q_UNUSED(rfidLSB);
}

void DeviceProcessingResponse::LoaderReportBlockingState(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, BlockState_t BlockingState)
{
    emit ReportLoaderBlockingState(InstanceID,  HdlInfo, BlockingState);
}

void DeviceProcessingResponse::OvenReportActStatusTempCtrl(DevInstanceID_t /*instanceID*/, ReturnCode_t /*hdlInfo*/, TempCtrlStatus_t /*statusTempCtrl*/)
{
}

void DeviceProcessingResponse::OvenReportActOperatingMode(DevInstanceID_t /*instanceID*/, ReturnCode_t /*hdlInfo*/, TempCtrlOperatingMode_t /*operatingMode*/)
{
}

void DeviceProcessingResponse::OvenReportActTemperature(DevInstanceID_t /*instanceID*/, ReturnCode_t /*hdlInfo*/, qint16 /*temperature*/)
{
}

void DeviceProcessingResponse::OvenCoverReferenceRunAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, OvenCoverPosition_t OvenCoverPos)
{
    emit ReportOvenReferenceRunAckn(InstanceID, HdlInfo, OvenCoverPos);
}


void DeviceProcessingResponse::OvenReportActCoverPosition(DevInstanceID_t /*instanceID*/, ReturnCode_t /*hdlInfo*/, OvenCoverPosition_t /*ovenCoverPos*/)
{
}

void DeviceProcessingResponse::HeatedVesselsReportActTemperature(DevInstanceID_t /*instanceID*/, ReturnCode_t /*hdlInfo*/, HeatedVesselID_t /*vesselID*/, qint16 /*temperature*/)
{
}

void DeviceProcessingResponse::HeatedVesselsReportActStatusTempCtrl(DevInstanceID_t /*instanceID*/, ReturnCode_t /*hdlInfo*/, TempCtrlStatus_t /*statusTempCtrl*/)
{
}

void DeviceProcessingResponse::HeatedVesselsReportActOperatingMode(DevInstanceID_t /*instanceID*/, ReturnCode_t /*hdlInfo*/, TempCtrlOperatingMode_t /*operatingMode*/)
{
}

void DeviceProcessingResponse::RackTransferReferenceRunAckn(DevInstanceID_t /*instanceID*/, ReturnCode_t /*hdlInfo*/, RackAdapterPosition_t /*position*/)
{
}

void DeviceProcessingResponse::RackTransferReportActAdapterPosition(DevInstanceID_t /*instanceID*/, ReturnCode_t /*hdlInfo*/, RackAdapterPosition_t /*position*/)
{
}

void DeviceProcessingResponse::AgitationReferenceRunAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, AgitationPosition_t Position)
{
    emit ReportAgitationReferenceRunAckn(InstanceID, HdlInfo, Position);
}

void DeviceProcessingResponse::AgitationReportActPosition(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, AgitationPosition_t Position)
{
    emit ReportAgitationActPosition(InstanceID, HdlInfo, Position);
}

void DeviceProcessingResponse::AgitationReportActSpeed(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, AgitationSpeed_t Speed)
{
    emit ReportAgitationActSpeed(InstanceID, HdlInfo, Speed);
}

void DeviceProcessingResponse::ExhaustReportFanCurrent(DevInstanceID_t /*instanceID*/, ReturnCode_t /*hdlInfo*/, quint16 /*fanCurrent*/)
{
}

void DeviceProcessingResponse::ExhaustReportFlowSensorStatus(DevInstanceID_t /*instanceID*/, ReturnCode_t /*hdlInfo*/, quint16 /*flowSensor*/)
{
}

void DeviceProcessingResponse::WaterReportLiquidLevel(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, quint16 LiquidLevel)
{
    emit ReportWaterLiquidLevel(InstanceID, HdlInfo, LiquidLevel);
}

void DeviceProcessingResponse::WaterReportValveStatus(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, WaterValveID_t ValveID, quint8 ValveStatus)
{
    emit ReportWaterValveStatus(InstanceID, HdlInfo, ValveID, ValveStatus);
}

void DeviceProcessingResponse::InclinometerReportInclination(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, qint16 inclValue)
{
    Q_UNUSED(instanceID);
    Q_UNUSED(hdlInfo);
    Q_UNUSED(inclValue);
}

void DeviceProcessingResponse::InclinometerReportInclinationLimitAck(DevInstanceID_t instanceID, ReturnCode_t hdlInfo)
{
    Q_UNUSED(instanceID);
    Q_UNUSED(hdlInfo);
}

void DeviceProcessingResponse::InclinometerReportReferencePosAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, qint16 inclValue)
{
    Q_UNUSED(instanceID);
    Q_UNUSED(hdlInfo);
    Q_UNUSED(inclValue);
}

void DeviceProcessingResponse::CoverLineReferenceRunAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo)
{
    Q_UNUSED(instanceID);
    Q_UNUSED(hdlInfo);
}

void DeviceProcessingResponse::CoverLineCoverAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo)
{
    Q_UNUSED(instanceID);
    Q_UNUSED(hdlInfo);
}

void DeviceProcessingResponse::RackHandlingReferenceRunAckn(DevInstanceID_t /*instanceID*/, ReturnCode_t /*hdlInfo*/)
{
}

void DeviceProcessingResponse::RackHandlingPositioningAckn(DevInstanceID_t /*instanceID*/, ReturnCode_t /*hdlInfo*/)
{

}
} //namespace
