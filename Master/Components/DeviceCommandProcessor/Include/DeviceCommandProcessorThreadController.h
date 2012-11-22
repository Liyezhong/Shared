/****************************************************************************/
/*! \file DeviceCommandProcessorThreadController.h
 *
 *  \brief Definition file for class DeviceCommandProcessorThreadController.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-08-22
 *  $Author:    $ Norbert Wiedmann
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

#ifndef DEVICECOMMANDPROCESSOR_DEVICECOMMANDPROCESSORTHREADCONTROLLER_H
#define DEVICECOMMANDPROCESSOR_DEVICECOMMANDPROCESSORTHREADCONTROLLER_H

#include <Threads/Include/ThreadController.h>
#include <Global/Include/Commands/AckOKNOK.h>

//Adaption to device processing
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include "DeviceControl/Include/Interface/IDeviceProcessing.h"

#include <QMutex>

using namespace DeviceControl;

// DestroyObjects is the cleanup function for class DeviceCommandProcessorThreadController,
// so we inform lint about this.
//lint -sem(DeviceCommandProcessor::DeviceCommandProcessorThreadController::DestroyObjects, cleanup)

namespace DeviceCommandProcessor {

class CmdDeviceProcessingInit;
class CmdDeviceProcessingCleanup;
class CmdGrapplerAttachToRack;
class CmdGrapplerDetachFromRack;
class CmdGrapplerLetDownRack;
class CmdGrapplerMoveEmptyTo;
class CmdGrapplerPullUpRack;
class CmdGrapplerReferenceRun;
class CmdGrapplerTransportRackTo;
class CmdGrapplerReadRFID;
class CmdGrapplerReadLiquidLevel;
class CmdLoaderReferenceRun;
class CmdLoaderSetBlockingState;
class CmdLoaderRacksChanged;
class CmdAgitationReferenceRun;
class CmdAgitationSetSpeed;
class CmdOvenCoverReferenceRun;
class CmdSetOvenCoverPostion;
class CmdHeatStation;
class CmdSetWaterLevel;
class CmdCountSlides;
class CmdEstimatedTime;
class IDeviceProcessing;

#define CMD_REF_CNT  25   ///< array size of the command date buffer

/****************************************************************************/
/**
 * \brief This class encapsulates the thread controller for the device command processing.
 *
 * This class encapsulates the thread controller for the device command
 * processing. It communicates with the real hardware.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class DeviceCommandProcessorThreadController : public Threads::ThreadController {
    Q_OBJECT

private:
    /****************************************************************************/
    DeviceCommandProcessorThreadController();                                                                   ///< Not implemented.
    DeviceCommandProcessorThreadController(const DeviceCommandProcessorThreadController &);                     ///< Not implemented.
    const DeviceCommandProcessorThreadController & operator = (const DeviceCommandProcessorThreadController &); ///< Not implemented.
    /****************************************************************************/
    /**
     * \brief Process error event from local object.
     *
     * See detailed description in the base class' header.
     *
     * \param[in]   TheEventEntry = event from one of local objects
     * \return      true if event was processed.
     */
    /****************************************************************************/
    virtual bool LocalProcessErrorEvent(const DataLogging::EventEntry &TheEventEntry);
    /****************************************************************************/
    /**
     * \brief Destroy all created objects.
     */
    /****************************************************************************/
    void DestroyObjects();
    /****************************************************************************/
    /**
     * \brief Get string ID for grappler name from instance id.
     *
     * Get string ID for grappler name from instance id. If incorrect instance id
     * provided returns EVENT_DEVICECOMMANDPROCESSOR_STRING_GRAPPLER_UNKNOWN.
     *
     * \param[in]   GrapplerID      Grappler ID.
     * \return                      String ID for grappler.
     */
    /****************************************************************************/
    quint32 GrapplerNameByInstanceID(DeviceControl::DevInstanceID_t GrapplerID) const;

protected:
    /****************************************************************************/
    /**
     * \brief This method is called when the base class received the \ref Go signal.
     *
     * This means that everything is fine and normal operation started.
     * We are running in our own thread now.\n
     */
    /****************************************************************************/
    virtual void OnGoReceived();
    /****************************************************************************/
    /**
     * \brief This method is called when the base class received the \ref Stop signal.
     *
     * This means that normal operation will stop after processing this signal.
     * We are still running in our own thread.\n
     */
    /****************************************************************************/
    virtual void OnStopReceived();
    /****************************************************************************/
    /**
     * \brief Power will fail shortly.
     */
    /****************************************************************************/
    virtual void OnPowerFail();
    /****************************************************************************/
    /**
     * \brief Default handler for command timeouts.
     *
     * Will be called when no other handler is installed.
     *
     * \param[in]       Ref         The command reference.
     * \param[in]       CmdName     Name of command.
     */
    /****************************************************************************/
    virtual void OnCmdTimeout(Global::tRefType Ref, const QString &CmdName);

public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   TheLoggingSource    Logging source to be used.
     */
    /****************************************************************************/
    DeviceCommandProcessorThreadController(Global::gSourceType TheLoggingSource);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~DeviceCommandProcessorThreadController();
    /****************************************************************************/
    /**
     * \brief Create and configure needed objects.
     *
     * If something goes wrong, the already created objects must be deleted.
     */
    /****************************************************************************/
    virtual void CreateAndInitializeObjects();
    /****************************************************************************/
    /**
     * \brief Cleanup and destroy the created objects.
     */
    /****************************************************************************/
    virtual void CleanupAndDestroyObjects();
    /****************************************************************************/
    /**
     * \brief Command of type CmdDeviceProcessingInit received.
     *
     * \param[in]       Ref                 Reference of command.
     * \param[in]       Cmd                 Command.
     */
    /****************************************************************************/
    virtual void OnDeviceProcessingInit(Global::tRefType Ref, const CmdDeviceProcessingInit &Cmd);
    /****************************************************************************/
    /**
     * \brief Command of type CmdDeviceProcessingCleanup received.
     *
     * \param[in]       Ref                 Reference of command.
     * \param[in]       Cmd                 Command.
     */
    /****************************************************************************/
    virtual void OnDeviceProcessingCleanup(Global::tRefType Ref, const CmdDeviceProcessingCleanup &Cmd);
    /****************************************************************************/
    /**
     * \brief Command of type CmdGrapplerAttachToRack received.
     *
     * \param[in]       Ref                 Reference of command.
     * \param[in]       Cmd                 Command.
     */
    /****************************************************************************/
    virtual void OnGrapplerAttachToRack(Global::tRefType Ref, const CmdGrapplerAttachToRack &Cmd);
    /****************************************************************************/
    /**
     * \brief Command of type CmdGrapplerDetachFromRack received.
     *
     * \param[in]       Ref                 Reference of command.
     * \param[in]       Cmd                 Command.
     */
    /****************************************************************************/
    virtual void OnGrapplerDetachFromRack(Global::tRefType Ref, const CmdGrapplerDetachFromRack &Cmd);
    /****************************************************************************/
    /**
     * \brief Command of type CmdGrapplerLetDownRack received.
     *
     * \param[in]       Ref                 Reference of command.
     * \param[in]       Cmd                 Command.
     */
    /****************************************************************************/
    virtual void OnGrapplerLetDownRack(Global::tRefType Ref, const CmdGrapplerLetDownRack &Cmd);
    /****************************************************************************/
    /**
     * \brief Command of type CmdGrapplerMoveEmptyTo received.
     *
     * \param[in]       Ref                 Reference of command.
     * \param[in]       Cmd                 Command.
     */
    /****************************************************************************/
    virtual void OnGrapplerMoveEmptyTo(Global::tRefType Ref, const CmdGrapplerMoveEmptyTo &Cmd);
    /****************************************************************************/
    /**
     * \brief Command of type CmdGrapplerPullUpRack received.
     *
     * \param[in]       Ref                 Reference of command.
     * \param[in]       Cmd                 Command.
     */
    /****************************************************************************/
    virtual void OnGrapplerPullUpRack(Global::tRefType Ref, const CmdGrapplerPullUpRack &Cmd);
    /****************************************************************************/
    /**
     * \brief Command of type CmdGrapplerReferenceRun received.
     *
     * \param[in]       Ref                 Reference of command.
     * \param[in]       Cmd                 Command.
     */
    /****************************************************************************/
    virtual void OnGrapplerReferenceRun(Global::tRefType Ref, const CmdGrapplerReferenceRun &Cmd);
    /****************************************************************************/
    /**
     * \brief Command of type CmdGrapplerTransportRackTo received.
     *
     * \param[in]       Ref                 Reference of command.
     * \param[in]       Cmd                 Command.
     */
    /****************************************************************************/
    virtual void OnGrapplerTransportRackTo(Global::tRefType Ref, const CmdGrapplerTransportRackTo &Cmd);
    /****************************************************************************/
    /**
     * \brief Command of type CmdGrapplerReadLiquidLevel received.
     *
     * \param[in]       Ref                 Reference of command.
     * \param[in]       Cmd                 Command.
     */
    /****************************************************************************/
    virtual void OnGrapplerReadRFID(Global::tRefType Ref, const CmdGrapplerReadRFID &Cmd);
    /****************************************************************************/
    /**
     * \brief Command of type CmdGrapplerReadLiquidLevel received.
     *
     * \param[in]       Ref                 Reference of command.
     * \param[in]       Cmd                 Command.
     */
    /****************************************************************************/
    virtual void OnGrapplerReadLiquidLevel(Global::tRefType Ref, const CmdGrapplerReadLiquidLevel &Cmd);

    virtual void OnGrapplerCountSlides(Global::tRefType Ref, const CmdCountSlides &Cmd);

    //*************************************************
    //**************   Loader/Unloader   **************
    /**
     * \brief Command of type CmdLoaderReferenceRun received.
     *
     * \param[in]       Ref                 Reference of command.
     * \param[in]       Cmd                 Command.
     */
    /****************************************************************************/
    virtual void OnLoaderReferenceRun(Global::tRefType Ref, const CmdLoaderReferenceRun &Cmd);
    /**
     * \brief Command of type CmdLoaderSetBlockingState received.
     *
     * \param[in]       Ref                 Reference of command.
     * \param[in]       Cmd                 Command.
     */
    /****************************************************************************/
    virtual void OnLoaderSetBlockingState(Global::tRefType Ref, const CmdLoaderSetBlockingState &Cmd);

    //*************************************************
    //**************   Agitation   **************
    /**
     * \brief Command of type CmdAgitationReferenceRun received.
     *
     * \param[in]       Ref                 Reference of command.
     * \param[in]       Cmd                 Command.
     */
    /****************************************************************************/
    virtual void OnAgitationReferenceRun(Global::tRefType Ref, const CmdAgitationReferenceRun &Cmd);
    /**
     * \brief Command of type CmdAgitationSetSpeed received.
     *
     * \param[in]       Ref                 Reference of command.
     * \param[in]       Cmd                 Command.
     */
    /****************************************************************************/
    virtual void OnAgitationSetSpeed(Global::tRefType Ref, const CmdAgitationSetSpeed &Cmd);

    virtual void OnSetWaterLevel(Global::tRefType Ref, const CmdSetWaterLevel &Cmd);

    //*************************************************
    //**************   Oven   **************
    /**
     * \brief Command of type CmdOvenCoverReferenceRun received.
     *
     * \param[in]       Ref                 Reference of command.
     * \param[in]       Cmd                 Command.
     */
    /****************************************************************************/
    virtual void OnOvenCoverReferenceRun(Global::tRefType Ref, const CmdOvenCoverReferenceRun &Cmd);

    virtual void OnSetOvenCoverPosition(Global::tRefType Ref, const CmdSetOvenCoverPostion &Cmd);

    virtual void OnHeatStation(Global::tRefType Ref, const CmdHeatStation &Cmd);

    virtual void OnEstimatedTime(Global::tRefType Ref, const CmdEstimatedTime &Cmd);

    //*************************************************
    //**************   Hood   **************
    /**
     * \brief           Command of type CmdHoodStatusReq received.
     *
     * \param[in]       Ref                 Reference of command.
     * \param[in]       Cmd                 Command.
     */
    /****************************************************************************/
    virtual void OnHoodStatusReq(Global::tRefType Ref, const CmdLoaderSetBlockingState &Cmd);

    /****************************************************************************/
    /**
     * \brief Acknowledge of type AckOKNOK received.
     *
     * \param[in]   Ref     Command reference.
     * \param[in]   Ack     Acknowledge.
     */
    /****************************************************************************/
    virtual void OnAckOKNOK(Global::tRefType Ref, const Global::AckOKNOK &Ack);


    //####################################################################
    // Slots connected to signals from IDeviceProcessing
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
    void LoaderReportDrawerUserAction(DevInstanceID_t instanceID, ReturnCode_t hdlInfo,
                                      LoaderPosition_t position, BlockState_t BlockState);
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

    void WaterReportLiquidLevel(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, quint16 liquidLevel);
    void WaterReportValveStatus(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, WaterValveID_t valveID, quint8 valveStatus);

    void InclinometerReportInclination(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, qint16 inclValue);
    void InclinometerReportInclinationLimitAck(DevInstanceID_t instanceID, ReturnCode_t hdlInfo);
    void InclinometerReportReferencePosAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo, qint16 inclValue);

    void CoverLineReferenceRunAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo);
    void CoverLineCoverAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo);

    void RackHandlingReferenceRunAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo);
    void RackHandlingPositioningAckn(DevInstanceID_t instanceID, ReturnCode_t hdlInfo);

    void HoodStatusReqAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, HoodState_t HoodState);
    //###################################################


private:
    bool GetFreeCommandCtrlContainer(quint8& CmdIdx);
    bool GetCommandCtrlContainer(quint8& CmdIdx,
                                 DeviceControl::DevInstanceID_t InstanceID,
                                 QStringList& CommandNameList);

    ReturnCode_t ReqRFIDData(LoaderRFIDChannel_t RfidChannel);

    DeviceControl::IDeviceProcessing m_DeviceProcessing;    //!< Device processing instance

    /*! command state definition*/
    typedef enum {
        DCP_TC_CMD_STATE_FREE  = 0x00,  ///< command state free
        DCP_TC_CMD_STATE_ACTIV = 0x01   ///< command state activ
    } CommandRefState_t;

    /*! command container element*/
    typedef struct {
        CommandRefState_t               m_State;         ///< command state
        Global::tRefType                m_RefType;       ///< RefType from command
        DeviceControl::DevInstanceID_t  m_DevInstanceID; ///< Device instance identification
        QString                         m_CommandName;   ///< command name
    } CommandRef_t;

    CommandRef_t  m_CommandRef[CMD_REF_CNT];   ///< command container array

    //this is a quick solution to store the reftypes of incomming commands
    // until they will be acknowledged
    Global::tRefType   m_RefInitDCL;            ///< command reference of the initialisation command


    Global::tRefType   m_InLoaderChangedRef;     ///< Expected LoaderChanged acknowledge reference.
    Global::tRefType   m_InUnloaderChangedRef;   ///< Expected LoaderChanged acknowledge reference.

    QMutex m_Mutex;   //!< Access serialization
}; // end class DeviceCommandProcessorThreadController

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_DEVICECOMMANDPROCESSORTHREADCONTROLLER_H
