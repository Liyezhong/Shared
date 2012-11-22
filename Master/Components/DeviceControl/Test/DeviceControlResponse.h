#ifndef DEVICECONTROLRESPONSE_H
#define DEVICECONTROLRESPONSE_H

#include "DeviceControl/Include/Interface/IResponse.h"

namespace DeviceControl {

class IDeviceProcessing;

class DeviceProcessingResponse: public QObject, public IResponse
{
    Q_OBJECT
public:
    DeviceProcessingResponse();
    ~DeviceProcessingResponse();

    IDeviceProcessing* GetIDeviceProcessing() { return m_pIDeviceProcessing;};

    //####################################################################
    //virtual function implementation from IResponse
    void ThrowError(DevInstanceID_t instanceID, quint16 usErrorGroup, quint16 usErrorID, quint16 usErrorData,const QDateTime & TimeStamp);
    void DevProcInitialisationAckn(DevInstanceID_t instanceID, ReturnCode_t configResult);
    void DevProcConfigurationAckn(DevInstanceID_t instanceID, ReturnCode_t configResult);
    void DevProcStartNormalOpModeAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo);
    void StartAdjustmentServiceAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo);
    void DevProcModeChangeInfo(DevInstanceID_t instanceID, ReturnCode_t mode, quint8 newState);

    // reference run has finished
    void GrapplerReferenceRunAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo);
    // positioning has been done ( result of ReqStationAction(.., STATION_ACTION_GRAB - STATION_ACTION_DISCARD)
    void GrapplerPositioningAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo);
    // rfid data have been read ( result of ReqStationAction(.., STATION_ACTION_READ_RFID)
    void GrapplerReportRFIDData(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, quint32 rfidMSB, quint32 rfidLSB);
    // liquid level has been read ( result of ReqStationAction(.., STATION_ACTION_READ_LIQUID_LEVEL)
    void GrapplerReportLiquidLevel(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, quint16 liquidLevelValue);

    // reference run has finished
    void LoaderReferenceRunAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, LoaderPosition_t position);
    // the drawer has finished a positioning ( triggered by application), or a position request was called
    void LoaderReportActDrawerPosition(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, LoaderPosition_t position);
    // the drawer has finished a positioning which was triggered by the user
    void LoaderReportDrawerUserAction(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, LoaderPosition_t position, BlockState_t BlockState);
    // rfid data have been read (use instanceID to determine which cannel (FCTMOD_RFID<1-5>_LOADER, e.g. FCTMOD_RFID2_LOADER)
    void LoaderReportRFIDData(DevInstanceID_t instanceID, ReturnCode_t handlingCode, LoaderRFIDChannel_t rfidChannel, quint32 rfidMSB, quint32 rfidLSB);
    // the drawers blocking state has changed
    void LoaderReportBlockingState(DevInstanceID_t instanceID, ReturnCode_t handlingCode, BlockState_t blockingState);

    void OvenReportActStatusTempCtrl(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, TempCtrlStatus_t statusTempCtrl);
    void OvenReportActOperatingMode(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, TempCtrlOperatingMode_t operatingMode);
    void OvenReportActTemperature(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, qint16 temperature);
    void OvenCoverReferenceRunAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, OvenCoverPosition_t ovenCoverPos);
    //void OvenCoverPositionAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, OvenCoverPosition_t ovenCoverPos);
    void OvenReportActCoverPosition(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, OvenCoverPosition_t ovenCoverPos);

    void HeatedVesselsReportActTemperature(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, HeatedVesselID_t vesselID, qint16 temperature);
    void HeatedVesselsReportActStatusTempCtrl(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, TempCtrlStatus_t statusTempCtrl);
    void HeatedVesselsReportActOperatingMode(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, TempCtrlOperatingMode_t operatingMode);

    void RackTransferReferenceRunAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, RackAdapterPosition_t position);
    void RackTransferReportActAdapterPosition(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, RackAdapterPosition_t position);

    void AgitationReferenceRunAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, AgitationPosition_t position);
    void AgitationReportActPosition(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, AgitationPosition_t position);
    void AgitationReportActSpeed(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, AgitationSpeed_t speed);

    void ExhaustReportFanCurrent(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, quint16 fanCurrent);
    void ExhaustReportFlowSensorStatus(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, quint16 flowSensor);

    void WaterReportLiquidLevel(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, quint16 LiquidLevel);
    void WaterReportValveStatus(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, WaterValveID_t ValveID, quint8 ValveStatus);

    void InclinometerReportInclination(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, qint16 inclValue);
    void InclinometerReportInclinationLimitAck(DevInstanceID_t instanceID, ReturnCode_t hdlInfo);
    void InclinometerReportReferencePosAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, qint16 inclValue);

    void CoverLineReferenceRunAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo);
    void CoverLineCoverAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo);

    void RackHandlingReferenceRunAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo);
    void RackHandlingPositioningAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo);

    QString GetSerialNumber();
    void Start();
    void StartConfiguration();
    bool RequestCANNodes();

    bool m_bInitialisationFinished;
    bool m_bConfigurationFinished;

signals:
    void ReportGrapplerReferenceRunAckn(DevInstanceID_t, ReturnCode_t);
    void ReportGrapplerPositioningAckn(DevInstanceID_t, ReturnCode_t);

    void ReportLoaderReferenceRunAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, LoaderPosition_t Position);
    void ReportLoaderBlockingState(DevInstanceID_t instanceID, ReturnCode_t handlingCode, BlockState_t blockingState);

    void ReportOvenReferenceRunAckn(DevInstanceID_t, ReturnCode_t, OvenCoverPosition_t);


    void ReportAgitationReferenceRunAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, AgitationPosition_t position);
    void ReportAgitationActPosition(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, AgitationPosition_t position);
    void ReportAgitationActSpeed(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, AgitationSpeed_t speed);

    void ReportWaterLiquidLevel(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, quint16 LiquidLevel);
    void ReportWaterValveStatus(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, WaterValveID_t ValveID, quint8 ValveStatus);

private:
    IDeviceProcessing* m_pIDeviceProcessing;


};

} //namespace
#endif // DEVICECONTROLRESPONSE_H
