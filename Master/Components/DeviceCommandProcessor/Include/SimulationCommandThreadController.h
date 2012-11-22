/****************************************************************************/
/*! \file SimulationCommandThreadController.h
 *
 *  \brief Definition file for class SimulationCommandThreadController.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-10
 *  $Author:    $ Michael Thiel
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

#ifndef SIMULATIONCOMMANDTHREADCONTROLLER_H
#define SIMULATIONCOMMANDTHREADCONTROLLER_H

#include <Threads/Include/ThreadController.h>
#include "DeviceCommandProcessor/Include/DeviceCommandProcessorThreadController.h"
#include "NetworkComponents/Include/NetworkServerDevice.h"
#include "DeviceCommandProcessor/Include/Tray.h"

//#include "Global/Include/Commands/Command.h"
#include <QVector>

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
class Global::Command;
class Tray;

/**
  @brief Simulates a DeviceCommandProcessor, running without hardware
  */
class SimulationCommandThreadController : public DeviceCommandProcessor::DeviceCommandProcessorThreadController
{
    Q_OBJECT

public:
    SimulationCommandThreadController(Global::gSourceType loggingSource);

    void SendCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd);

protected:

    virtual void OnGoReceived();

    virtual void OnEstimatedTime(Global::tRefType Ref, const CmdEstimatedTime &Cmd);

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

    /****************************************************************************/
    /**
     * \brief Acknowledge of type AckOKNOK received.
     *
     * \param[in]   Ref     Command reference.
     * \param[in]   Ack     Acknowledge.
     */
    /****************************************************************************/
    virtual void OnAckOKNOK(Global::tRefType Ref, const Global::AckOKNOK &Ack);

private:
    static quint32 getMoveDownDuration();

    static quint32 getMoveUpDuration();

    static quint32 getMoveDownSlowDuration();

    static quint32 getMoveUpSlowDuration();

    /**
      @brief Returns the estimated duration of an attach operation
      */
    static quint32 getAttachDuration();

    static quint32 getActivateAgitationDuration();

    /**
      @brief Returns the estimated duration of a detach operation
      */
    static quint32 getDetachDuration();

    static quint32 getHeatupDuration(int row, int column, double targetTemperature);

    static quint32 getHeatingSwitchDuration();

    static quint32 getCountSlidesDuration();

    static QByteArray getDuration(QString commandType, QByteArray configData);

    static quint32 getOpenOvenCoverDuration();

    static quint32 getSetWaterLevelDuration();

    static quint32 getLoaderSetBlockingStateDuration();

//    static inline void toNetworkData(quint32 num, char *data)
//    {
//        unsigned char *udata = (unsigned char *)data;
//        udata[3] = (num & 0xff);
//        udata[2] = (num & 0xff00) >> 8;
//        udata[1] = (num & 0xff0000) >> 16;
//        udata[0] = (num & 0xff000000) >> 24;
//    }

    QTimer m_timer;         ///< Timer used to trigger time events
    qint64 m_currentTime;   ///< The simulated time in msecs

    /**
      @brief A list of commands to be executed when due
      Only commands resulting from device events (i.e. loader/unloader opening and closing, rack insertion and removal)
      are supported
      */
    QVector< QPair<quint64, Global::Command*> > m_commandList;

    /**
      @brief Reads events from a XML structure and places them into m_commandList
      */
    void ConvertXmlToEvents(QDomDocument doc);

    /**
      @brief Reads a XML structure out of a file
      */
    QDomDocument ReadXmlFromFile(QString filename);

    /**
      @brief Sends due commands to main controller
      */
    void ExecuteCommand(Global::tRefType Ref, Global::Command* Cmd);

    void insertRack(DeviceControl::DevInstanceID_t deviceId, qint32 rackId, qint32 rackUserData, DeviceControl::LoaderRFIDChannel_t stationId, quint64 time);

    void insertLoaderBlocking(DeviceControl::DevInstanceID_t deviceId, bool open, bool locked, quint64 time);

    void startSimulationGui();

//    NetworkBase::NetworkServerDevice *networkServer;

    QTcpServer *m_tcpServer;
    QTcpSocket *m_clientConnection;

//    SchedulerSimulation::MainWindow *simWindow;
    DeviceCommandProcessor::Tray *m_loadTray;
    DeviceCommandProcessor::Tray *m_unloadTray;
    DeviceCommandProcessor::Tray *m_cover;
    DeviceCommandProcessor::Tray *m_ovenCover;

    quint16 m_sampleTime;

    QFile *m_commfile;
    QTextStream m_outStream;
    bool m_verbose;

    /**
      @brief Determines whether device is being loaded with rack continuously, i.e. as soon as one rack has been removed from loader another one is placed in
      */
    qint16 m_continuousRacks;
    bool m_continuousUnloading;

    qint32 m_RackId;        ///< Rack RFID
    qint32 m_RackUserData;  ///< Rack user data providing information such as color
    QVector<DeviceControl::LoaderRFIDChannel_t> m_freeStationId;    ///< Loading/unloading station number where the rack is placed

    qint64 m_lastTimeRackAttached;
    qint16 m_racksInserted; ///< The number of racks already processed or being processed

    qint64 m_rackTimeOffset;

    qint16 m_acceleration;


private slots:
    void OnTimer();
    void registerConnection();
    void readMessageFromClient();
    void sendMessageToClient(QString message);
};

}
#endif // SIMULATIONCOMMANDTHREADCONTROLLER_H
