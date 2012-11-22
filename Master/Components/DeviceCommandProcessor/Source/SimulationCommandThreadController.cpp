/****************************************************************************/
/*! \file SimulationCommandThreadController.cpp
 *
 *  \brief Implementation file for class SimulationCommandThreadController.
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

#include "DeviceCommandProcessor/Include/SimulationCommandThreadController.h"
#include "DeviceCommandProcessor/Include/Commands/CmdRackInserted.h"
#include "DeviceCommandProcessor/Include/Commands/CmdEstimatedTime.h"
#include "DeviceCommandProcessor/Include/Commands/CmdLoaderStateChanged.h"
#include "DeviceCommandProcessor/Include/Commands/CmdLoaderUpdate.h"
#include "DeviceCommandProcessor/Include/Commands/CmdCountSlides.h"
#include "DeviceCommandProcessor/Include/Commands/CmdGrapplerLetDownRack.h"
#include "DeviceCommandProcessor/Include/Commands/CmdGrapplerPullUpRack.h"
#include "DeviceCommandProcessor/Include/Commands/CmdGrapplerTransportRackTo.h"
#include "DeviceCommandProcessor/Include/Commands/CmdGrapplerMoveEmptyTo.h"
#include "DeviceCommandProcessor/Include/Commands/CmdLoaderSetBlockingState.h"
#include "DeviceCommandProcessor/Include/Commands/CmdGrapplerAttachToRack.h"
#include "DeviceCommandProcessor/Include/Commands/CmdGrapplerDetachFromRack.h"
#include "DeviceCommandProcessor/Include/Commands/CmdAgitationSetSpeed.h"
#include "DeviceCommandProcessor/Include/Commands/CmdSetWaterLevel.h"
#include "DeviceCommandProcessor/Include/Commands/CmdHeatStation.h"
#include "DeviceCommandProcessor/Include/Commands/CmdStationState.h"
#include "DeviceCommandProcessor/Include/Commands/CmdCoverStateChanged.h"
#include "Scheduler/Include/StationHandler.h"


#include "DeviceCommandProcessor/Include/Commands/AcknEstimatedTime.h"
#include "DeviceCommandProcessor/Include/Commands/AcknSlideCount.h"
//#include "NetworkComponents/Include/NetworkServerDevice.h"
#include "Global/Include/SystemPaths.h"
#include "Global/Include/Commands/Command.h"
#include "DeviceCommandProcessor/Include/Tray.h"
#include "DataManager/Containers/UserSettings/Commands/Include/CmdWaterStationUpdate.h"

#include <QtGui/QApplication>
#include <QPair>
#include <QDir>
#include <QDebug>
#include <QDateTime>



namespace DeviceCommandProcessor {

//const NetworkBase::NetworkServerType_t MY_CLIENT_TYPE3 = NetworkBase::NSE_TYPE_SEPIA;

SimulationCommandThreadController::SimulationCommandThreadController(Global::gSourceType loggingSource)
    : DeviceCommandProcessorThreadController(loggingSource)
    , m_timer(this)
    , m_currentTime(0)
    , m_clientConnection(0)
    , m_sampleTime(100)
    , m_commfile(new QFile("simulationInput.txt"))
    , m_outStream(m_commfile)
    , m_verbose(true)
    , m_continuousRacks(0)
    , m_continuousUnloading(false)
    , m_lastTimeRackAttached(0)
    , m_racksInserted(0)
    , m_rackTimeOffset(20000)
    , m_acceleration(1)
//    , simWindow(0)
{
    m_freeStationId.push_back(DeviceControl::LOADER_ID_RFID_1);
    m_freeStationId.push_back(DeviceControl::LOADER_ID_RFID_2);
    m_freeStationId.push_back(DeviceControl::LOADER_ID_RFID_3);
    m_freeStationId.push_back(DeviceControl::LOADER_ID_RFID_4);
    m_freeStationId.push_back(DeviceControl::LOADER_ID_RFID_5);

    ConvertXmlToEvents(ReadXmlFromFile("../SimulationEvents.xml"));

//    simWindow = &(SchedulerSimulation::MainWindow::getInstance());
    m_loadTray = new DeviceCommandProcessor::Tray();
    m_unloadTray = new DeviceCommandProcessor::Tray();
    m_cover = new DeviceCommandProcessor::Tray();
    m_ovenCover = new DeviceCommandProcessor::Tray();

    m_tcpServer = new QTcpServer(this);
    if (!m_tcpServer->listen(QHostAddress::Any, 8889))
        qDebug() << m_tcpServer->errorString();

    qDebug() << m_tcpServer->serverAddress();
    qDebug() << m_tcpServer->serverPort();
    qDebug() << m_tcpServer->errorString();

    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(registerConnection()));

    if (!m_commfile->open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    if (m_verbose)
    {
        m_outStream << QDateTime::currentDateTime().time().toString() << " Starting\n";
        m_outStream.flush();
        m_commfile->flush();
    }
    startSimulationGui();
}

void SimulationCommandThreadController::startSimulationGui()
{
    QProcess *process = new QProcess(this);
    QString program = "./SchedulerSimulation";
    process->start(program);
}

void SimulationCommandThreadController::registerConnection()
{
    qDebug() << "SimulationCommandThreadController::registerConnection";
    m_clientConnection = m_tcpServer->nextPendingConnection();
    connect(m_clientConnection, SIGNAL(readyRead()), this, SLOT(readMessageFromClient()));
    sendMessageToClient("Connected to Colorado simulation");
}

void SimulationCommandThreadController::readMessageFromClient()
{
    QDataStream in(m_clientConnection);
    in.setVersion(QDataStream::Qt_4_0);
    QString input;
    in >> input;

    if (m_verbose)
    {
        if (!input.contains("CurrentTime"))
        {
            m_outStream << QDateTime::currentDateTime().toTime_t() << " SimulationCommandThreadController Read: " << input << "\n";
            m_outStream.flush();
            m_commfile->flush();
        }
    }

    if (input == "RequestLoaderStateChange")
    {
        DeviceCommandProcessor::CmdLoaderStateChanged* commandPtr(new DeviceCommandProcessor::CmdLoaderStateChanged(DeviceControl::DEVICE_INSTANCE_ID_LOADER));
        commandPtr->m_DevInstanceID = DeviceControl::DEVICE_INSTANCE_ID_LOADER;
        commandPtr->m_Open = m_loadTray->isClosed();
        commandPtr->m_Locked = m_loadTray->isLocked();
        QPair<quint64, Global::Command*> event;
        event.first = m_currentTime;
        event.second = commandPtr;
        m_commandList.push_back(event);
    }
    if (input == "RequestUnloaderStateChange")
    {
        DeviceCommandProcessor::CmdLoaderStateChanged* commandPtr(new DeviceCommandProcessor::CmdLoaderStateChanged(DeviceControl::DEVICE_INSTANCE_ID_LOADER));
        commandPtr->m_DevInstanceID = DeviceControl::DEVICE_INSTANCE_ID_UNLOADER;
        commandPtr->m_Open = m_unloadTray->isClosed();
        commandPtr->m_Locked = m_unloadTray->isLocked();
        QPair<quint64, Global::Command*> event;
        event.first = m_currentTime;
        event.second = commandPtr;
        m_commandList.push_back(event);
    }
    if (input == "RequestCoverStateChange")
    {
        DeviceCommandProcessor::CmdCoverStateChanged* commandPtr(new DeviceCommandProcessor::CmdCoverStateChanged(DeviceControl::DEVICE_INSTANCE_ID_HOOD));
        commandPtr->m_DevInstanceID = DeviceControl::DEVICE_INSTANCE_ID_HOOD;
        commandPtr->m_CoverState = m_cover->isClosed() ? Global::OPEN : Global::CLOSED;
        QPair<quint64, Global::Command*> event;
        event.first = m_currentTime;
        event.second = commandPtr;
        m_commandList.push_back(event);
    }
    if (input.contains("RackInserted", Qt::CaseInsensitive))
    {
        QStringList s = input.split(";");
        if (s.count() == 4)
        {
            DeviceCommandProcessor::CmdLoaderUpdate* commandPtr(new DeviceCommandProcessor::CmdLoaderUpdate(DeviceControl::DEVICE_INSTANCE_ID_LOADER));
            commandPtr->m_RackId = s.at(1).toLong();
            commandPtr->m_Empty = (commandPtr->m_RackId == 0);
            QString stationId = s.at(3);
            commandPtr->m_DevInstanceID = (stationId.left(1) == "L") ? DeviceControl::DEVICE_INSTANCE_ID_LOADER : DeviceControl::DEVICE_INSTANCE_ID_UNLOADER;
            if (stationId.right(2) == "01") commandPtr->m_StationID = LOADER_ID_RFID_1;
            if (stationId.right(2) == "02") commandPtr->m_StationID = LOADER_ID_RFID_2;
            if (stationId.right(2) == "03") commandPtr->m_StationID = LOADER_ID_RFID_3;
            if (stationId.right(2) == "04") commandPtr->m_StationID = LOADER_ID_RFID_4;
            if (stationId.right(2) == "05") commandPtr->m_StationID = LOADER_ID_RFID_5;
            commandPtr->m_RackUserData = s.at(2).toLongLong();
            QPair<quint64, Global::Command*> event;
            event.first = m_currentTime;
            event.second = commandPtr;
            m_commandList.push_back(event);

        }
    }
}

void SimulationCommandThreadController::sendMessageToClient(QString message)
{
    if (!m_clientConnection)
        return;

//    qDebug() << "SimulationCommandThreadController::sendMessage" << message;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << message;
    m_clientConnection->write(block);
    m_clientConnection->flush();

    if (m_verbose)
    {
        if (!message.contains("CurrentTime"))
        {
            m_outStream << QDateTime::currentDateTime().toTime_t() << " SimulationCommandThreadController Send: " << message << "\n";
            m_outStream.flush();
            m_commfile->flush();
        }
    }
}


void SimulationCommandThreadController::ExecuteCommand(Global::tRefType Ref, Global::Command *Cmd)
{
    bool success = true;
    if (Cmd->GetName() == "DeviceCommandProcessor::CmdLoaderStateChanged")
    {
        DeviceCommandProcessor::CmdLoaderStateChanged* command = (DeviceCommandProcessor::CmdLoaderStateChanged*)Cmd;
        if (command->m_DevInstanceID == DeviceControl::DEVICE_INSTANCE_ID_LOADER)
            success = m_loadTray->executeTransition(command->m_Open, command->m_Locked);
        else
            success = m_unloadTray->executeTransition(command->m_Open, command->m_Locked);

        // output to simulation gui
        if (success)
        {
            QString message = command->GetName();
            message += ";" + QString::number((int)command->m_DevInstanceID);
            message += ";" + QString::number(command->m_Open);
            message += ";" + QString::number(command->m_Locked);
            sendMessageToClient(message);
        }
    }
    if (Cmd->GetName() == "DeviceCommandProcessor::CmdCoverStateChanged")
    {
        DeviceCommandProcessor::CmdCoverStateChanged* command = (DeviceCommandProcessor::CmdCoverStateChanged*)Cmd;

        m_cover->executeTransition(command->m_CoverState == Global::OPEN, false);
        QString message = command->GetName();
        message += ";";
        message += QString::number(m_cover->isOpen());
        sendMessageToClient(message);
    }

    if (success)
    {
        SendCommand(Ref, Global::CommandShPtr_t(Cmd));
    }
}

void SimulationCommandThreadController::SendCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd)
{
//    qDebug() << "SimulationCommandThreadController::SendCommand" << Cmd.GetPointerToUserData()->GetName();
    //sendMessage(Cmd.GetPointerToUserData()->GetName());
    DeviceCommandProcessorThreadController::SendCommand(Ref, Cmd);
}

void SimulationCommandThreadController::OnEstimatedTime(Global::tRefType Ref, const CmdEstimatedTime &Cmd)
{
//    qDebug() << "SimulationCommandThreadController::OnEstimatedTime" << Cmd.m_commandType << Cmd.m_configData;
    QByteArray duration = getDuration(Cmd.m_commandType, Cmd.m_configData);
    SendAcknowledge(Ref, Global::AcknowledgeShPtr_t(new DeviceCommandProcessor::AcknEstimatedTime(Cmd.m_configData, duration)));
}

void SimulationCommandThreadController::OnDeviceProcessingInit(Global::tRefType Ref, const CmdDeviceProcessingInit &Cmd){}

void SimulationCommandThreadController::OnDeviceProcessingCleanup(Global::tRefType Ref, const CmdDeviceProcessingCleanup &Cmd){}

void SimulationCommandThreadController::OnGrapplerAttachToRack(Global::tRefType Ref, const CmdGrapplerAttachToRack &Cmd)
{
    //qDebug() << "SimulationCommandThreadController::OnGrapplerAttachToRack";

    QString message = Cmd.GetName();
    message += ";" + QString::number((int)Cmd.m_DevInstanceID);
    message += ";" + QString(Cmd.m_Station);
    message += ";" + QString::number(Cmd.m_Rack);
    sendMessageToClient(message);

    SendAcknowledgeOK(Ref);

    if (Scheduler::StationHandler::getInstance().getStationType(Cmd.m_Station) == LOAD)
    {
        m_freeStationId.push_back(Scheduler::StationHandler::getLoaderStation(Cmd.m_Station));
        m_lastTimeRackAttached = m_currentTime;


//        qDebug() << "xxxxxxxx SimulationCommandThreadController::OnGrapplerAttachToRack" << m_freeStationId.size();
    }
}

void SimulationCommandThreadController::OnGrapplerDetachFromRack(Global::tRefType Ref, const CmdGrapplerDetachFromRack &Cmd)
{
    //qDebug() << "SimulationCommandThreadController::OnGrapplerDetachFromRack";

    QString message = Cmd.GetName();
    message += ";" + QString::number((int)Cmd.m_DevInstanceID);
    message += ";" + QString(Cmd.m_Station);
    message += ";" + QString::number(Cmd.m_Rack);
    sendMessageToClient(message);

    SendAcknowledgeOK(Ref);
}

void SimulationCommandThreadController::OnGrapplerLetDownRack(Global::tRefType Ref, const CmdGrapplerLetDownRack &Cmd)
{
    //qDebug() << "SimulationCommandThreadController::OnGrapplerLetDownRack";

    QString message = Cmd.GetName();
    message += ";" + QString::number((int)Cmd.m_DevInstanceID);
    message += ";" + QString(Cmd.m_Station);
    message += ";" + QString::number(Cmd.m_Rack);
    sendMessageToClient(message);

    SendAcknowledgeOK(Ref);
}

void SimulationCommandThreadController::OnGrapplerMoveEmptyTo(Global::tRefType Ref, const CmdGrapplerMoveEmptyTo &Cmd)
{
    //qDebug() << "SimulationCommandThreadController::OnGrapplerMoveEmptyTo";

    QString message = Cmd.GetName();
    message += ";" + QString::number((int)Cmd.m_DevInstanceID);
    message += ";" + QString::number(Cmd.m_Row);
    message += ";" + QString::number(Cmd.m_Column);
    sendMessageToClient(message);

    SendAcknowledgeOK(Ref);
}

void SimulationCommandThreadController::OnGrapplerPullUpRack(Global::tRefType Ref, const CmdGrapplerPullUpRack &Cmd)
{
    //qDebug() << "SimulationCommandThreadController::OnGrapplerPullUpRack";

    QString message = Cmd.GetName();
    message += ";" + QString::number((int)Cmd.m_DevInstanceID);
    message += ";" + QString(Cmd.m_Station);
    message += ";" + QString::number(Cmd.m_Rack);
    sendMessageToClient(message);

    SendAcknowledgeOK(Ref);
}

void SimulationCommandThreadController::OnGrapplerReferenceRun(Global::tRefType Ref, const CmdGrapplerReferenceRun &Cmd)
{
    SendAcknowledgeOK(Ref);
}

void SimulationCommandThreadController::OnGrapplerTransportRackTo(Global::tRefType Ref, const CmdGrapplerTransportRackTo &Cmd)
{
//    qDebug() << "SimulationCommandThreadController::OnGrapplerTransportRackTo";

    QString message = Cmd.GetName();
    message += ";" + QString::number((int)Cmd.m_DevInstanceID);
    message += ";" + QString::number(Cmd.m_Row);
    message += ";" + QString::number(Cmd.m_Column);
    message += ";" + QString::number(Cmd.m_Rack);
    sendMessageToClient(message);

    SendAcknowledgeOK(Ref);
}

void SimulationCommandThreadController::OnGrapplerReadRFID(Global::tRefType Ref, const CmdGrapplerReadRFID &Cmd)
{
    SendAcknowledgeOK(Ref);
}

void SimulationCommandThreadController::OnGrapplerReadLiquidLevel(Global::tRefType Ref, const CmdGrapplerReadLiquidLevel &Cmd)
{
    SendAcknowledgeOK(Ref);
}

void SimulationCommandThreadController::OnGrapplerCountSlides(Global::tRefType Ref, const CmdCountSlides &Cmd)
{
//    qDebug() << "SimulationCommandThreadController::OnGrapplerCountSlides" << Ref;
    qint8 slideNumber = 27;

    QString message = Cmd.GetName();
    message += ";" + QString::number((int)Cmd.m_DevInstanceID);
    message += ";" + QString::number(slideNumber);
    message += ";" + QString::number(Cmd.m_rackId);
    sendMessageToClient(message);

    SendAcknowledge(Ref, Global::AcknowledgeShPtr_t(new DeviceCommandProcessor::AcknSlideCount(Cmd.m_rackId, slideNumber)));
}

void SimulationCommandThreadController::OnLoaderReferenceRun(Global::tRefType Ref, const CmdLoaderReferenceRun &Cmd)
{
    SendAcknowledgeOK(Ref);
}

void SimulationCommandThreadController::OnLoaderSetBlockingState(Global::tRefType Ref, const CmdLoaderSetBlockingState &Cmd)
{
    bool success = false;
    if (Cmd.m_DevInstanceID == DeviceControl::DEVICE_INSTANCE_ID_LOADER)
    {
        if (Cmd.m_BlockingState == DeviceControl::BLOCKING_STATE_BLOCKED)
            success = m_loadTray->lockTray();
        else
            success = m_loadTray->unlockTray();
    }
    if (Cmd.m_DevInstanceID == DeviceControl::DEVICE_INSTANCE_ID_UNLOADER)
    {
        if (Cmd.m_BlockingState == DeviceControl::BLOCKING_STATE_BLOCKED)
            success = m_unloadTray->lockTray();
        else
            success = m_unloadTray->unlockTray();
    }

    if (success)
    {
        QString message = Cmd.GetName();
        message += ";" + QString::number((int)Cmd.m_DevInstanceID);
        message += ";" + QString::number(Cmd.m_BlockingState);
        sendMessageToClient(message);

        if ((Cmd.m_DevInstanceID == DeviceControl::DEVICE_INSTANCE_ID_UNLOADER) && (Cmd.m_BlockingState == DeviceControl::BLOCKING_STATE_FREE))
        {
            if (m_continuousUnloading)  // remove racks as soon as they have been placed into unloader
            {
                insertLoaderBlocking(DeviceControl::DEVICE_INSTANCE_ID_UNLOADER,
                                     true,
                                     false,
                                     m_currentTime + 1000);

                insertLoaderBlocking(DeviceControl::DEVICE_INSTANCE_ID_UNLOADER,
                                     false,
                                     false,
                                     m_currentTime + 2000);
            }
        }
    }

    if (success)
        SendAcknowledgeOK(Ref);
    else
        SendAcknowledgeNOK(Ref);
}

void SimulationCommandThreadController::OnAgitationReferenceRun(Global::tRefType Ref, const CmdAgitationReferenceRun &Cmd)
{
    SendAcknowledgeOK(Ref);
}

void SimulationCommandThreadController::OnAgitationSetSpeed(Global::tRefType Ref, const CmdAgitationSetSpeed &Cmd)
{
    QString message = Cmd.GetName();
    message += ";" + QString::number((int)Cmd.m_AgitationSpeed);
    sendMessageToClient(message);

    SendAcknowledgeOK(Ref);
}

void SimulationCommandThreadController::OnSetWaterLevel(Global::tRefType Ref, const CmdSetWaterLevel &Cmd)
{
    QString message = Cmd.GetName();
    message += ";" + Cmd.m_Station;
    message += ";" + QString::number(Cmd.m_fill);
    sendMessageToClient(message);

    SendAcknowledgeOK(Ref);

    MsgClasses::CmdWaterStationUpdate* waterStationUpdate(new MsgClasses::CmdWaterStationUpdate(
                                                              100,
                                                              Cmd.m_Station,
                                                              Cmd.m_fill));

    qint64 outRef = GetNewCommandRef();
    ExecuteCommand(outRef, waterStationUpdate);
    qDebug() << "SimulationCommandThreadController::OnSetWaterLevel";
}

void SimulationCommandThreadController::OnOvenCoverReferenceRun(Global::tRefType Ref, const CmdOvenCoverReferenceRun &Cmd)
{
    SendAcknowledgeOK(Ref);
}

void SimulationCommandThreadController::OnSetOvenCoverPosition(Global::tRefType Ref, const CmdSetOvenCoverPostion &Cmd)
{
    SendAcknowledgeOK(Ref);
}

void SimulationCommandThreadController::OnHeatStation(Global::tRefType Ref, const CmdHeatStation &Cmd)
{
    QString message = Cmd.GetName();
    message += ";" + Cmd.m_Station;
    message += ";" + QString::number((int)Cmd.m_targetTemperature);
    sendMessageToClient(message);

    SendAcknowledgeOK(Ref);

    DeviceCommandProcessor::CmdStationState* outgoingCommand1(new DeviceCommandProcessor::CmdStationState(DeviceControl::DEVICE_INSTANCE_ID_HVESSELS));
    outgoingCommand1->m_Station = Cmd.m_Station;
    outgoingCommand1->m_Temperature = 20;   // todo: current temperature
    if (Cmd.m_targetTemperature > 20)
        outgoingCommand1->m_HeatingState = Global::TEMPERATURE_OUT_OF_RANGE;
    else
        outgoingCommand1->m_HeatingState = Global::OFF;

    qint64 outRef = GetNewCommandRef();
    ExecuteCommand(outRef, outgoingCommand1);

    // Tell the outside world when we are ready now
    if (Cmd.m_targetTemperature > 20)
    {
        DeviceCommandProcessor::CmdStationState* outgoingCommand2(new DeviceCommandProcessor::CmdStationState(DeviceControl::DEVICE_INSTANCE_ID_HVESSELS));
        outgoingCommand2->m_Station = Cmd.m_Station;
        outgoingCommand2->m_Temperature = Cmd.m_targetTemperature;
        outgoingCommand2->m_HeatingState = Global::TEMPERATURE_IN_RANGE;

        QPair<quint64, Global::Command*> event;
        int row = Scheduler::StationHandler::getInstance().getRow(Cmd.m_Station);
        int col = Scheduler::StationHandler::getInstance().getCol(Cmd.m_Station);
        event.first = m_currentTime + (getHeatupDuration(row, col, Cmd.m_targetTemperature) - 1000) / m_acceleration;
        event.second = outgoingCommand2;
        m_commandList.push_back(event);
    }
}

void SimulationCommandThreadController::OnAckOKNOK(Global::tRefType Ref, const Global::AckOKNOK &Ack){}

void SimulationCommandThreadController::OnTimer()
{
    m_timer.stop();

    m_currentTime += m_sampleTime;

    sendMessageToClient("CurrentTime;" + QString::number(m_currentTime));

//    qDebug() << "SimulationCommandThreadController::OnTimer2" << m_currentTime;

    for (int i = 0; i < m_commandList.size(); i++)
    {
        quint64 time = m_commandList.at(i).first;
        Global::Command* command = m_commandList.at(i).second;

        if (m_currentTime >= time)
        {
            qint64 Ref = GetNewCommandRef();
            ExecuteCommand(Ref, command);

            Q_UNUSED(Ref);
        }
    }

    for (int i = m_commandList.size() - 1; i >= 0; i--)
    {
        quint64 time = m_commandList.at(i).first;

        if (time <= m_currentTime)
        {
            m_commandList.remove(i);
        }
    }

    if ((m_continuousRacks > 0) && (m_continuousRacks > m_racksInserted))
    {
        if (m_currentTime + m_rackTimeOffset > m_lastTimeRackAttached)
        {
            if (m_freeStationId.size() > 0)
            {
                insertRack(DeviceControl::DEVICE_INSTANCE_ID_LOADER,
                           ++m_RackId,
                           m_RackUserData,
                           m_freeStationId[0],
                           m_currentTime + m_rackTimeOffset);
            }
        }
    }

    m_timer.start();
}

QDomDocument SimulationCommandThreadController::ReadXmlFromFile(QString filename)
{
    QDomDocument doc("CommandEvents");
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly))
    {
        doc.setContent(&file);
    }

    file.close();
    return doc;
}

void SimulationCommandThreadController::ConvertXmlToEvents(QDomDocument doc)
{
    m_commandList.clear();

    QDomElement docElem = doc.documentElement();

    QDomNodeList nodeList2 = docElem.elementsByTagName("Mode");

    for (int i = 0;i < nodeList2.count(); i++)
    {
        QDomElement element = nodeList2.at(i).firstChildElement("Command");
        QString type = element.attribute("Type");

        if (type == "ContinuousLoading")
        {
            m_continuousRacks = element.attribute("Racks").toInt();
        }
        if (type == "ContinuousUnloading")
            m_continuousUnloading = true;
    }

    QDomNodeList nodeList = docElem.elementsByTagName("Event");

    for (int i = 0;i < nodeList.count(); i++)
    {
        quint64 time = nodeList.at(i).firstChildElement("Time").text().toULongLong();
        QDomElement element = nodeList.at(i).firstChildElement("Command");
        QString type = element.attribute("Type");

        if (type == "CmdLoaderStateChanged")
        {
//            DeviceCommandProcessor::CmdLoaderStateChanged* commandPtr(new DeviceCommandProcessor::CmdLoaderStateChanged(DeviceControl::DEVICE_INSTANCE_ID_LOADER));
//            commandPtr->m_DevInstanceID = (element.attribute("Device")=="Loader") ? DeviceControl::DEVICE_INSTANCE_ID_LOADER : DeviceControl::DEVICE_INSTANCE_ID_UNLOADER;
//            commandPtr->m_Open = element.attribute("Open").toInt();
//            commandPtr->m_Locked = element.attribute("Locked").toInt();
//            QPair<quint64, Global::Command*> event;
//            event.first = time;
//            event.second = commandPtr;
//            m_commandList.push_back(event);

            insertLoaderBlocking((element.attribute("Device")=="Loader") ? DeviceControl::DEVICE_INSTANCE_ID_LOADER : DeviceControl::DEVICE_INSTANCE_ID_UNLOADER,
                                 element.attribute("Open").toInt(),
                                 element.attribute("Locked").toInt(),
                                 time);
        }

        if (type == "CmdLoaderUpdate")
        {
            DeviceControl::LoaderRFIDChannel_t rfidChannel = Scheduler::StationHandler::getLoaderStation(element.attribute("StationId"));

            insertRack((element.attribute("Device")=="Loader") ? DeviceControl::DEVICE_INSTANCE_ID_LOADER : DeviceControl::DEVICE_INSTANCE_ID_UNLOADER,
                       element.attribute("RackId").toLong(),
                       element.attribute("RackUserData").toLongLong(),
                       rfidChannel,
                       time);

            if (m_continuousRacks > 0)
            {
                m_RackId = element.attribute("RackId").toLong();
                m_RackUserData = element.attribute("RackUserData").toLongLong();
            }
        }
        if (type == "CmdCoverStateChanged")
        {
            DeviceCommandProcessor::CmdCoverStateChanged* commandPtr(new DeviceCommandProcessor::CmdCoverStateChanged(DeviceControl::DEVICE_INSTANCE_ID_LOADER));
            commandPtr->m_DevInstanceID = (element.attribute("Device")=="Cover") ? DeviceControl::DEVICE_INSTANCE_ID_HOOD : DeviceControl::DEVICE_INSTANCE_ID_HOOD;
            commandPtr->m_CoverState = (element.attribute("Open").toInt() == 1) ? Global::OPEN : Global::CLOSED;
            QPair<quint64, Global::Command*> event;
            event.first = time;
            event.second = commandPtr;
            m_commandList.push_back(event);
        }
    }
}

void SimulationCommandThreadController::insertLoaderBlocking(DeviceControl::DevInstanceID_t deviceId, bool open, bool locked, quint64 time)
{
    DeviceCommandProcessor::CmdLoaderStateChanged* commandPtr(new DeviceCommandProcessor::CmdLoaderStateChanged(DeviceControl::DEVICE_INSTANCE_ID_LOADER));
    commandPtr->m_DevInstanceID = deviceId;
    commandPtr->m_Open = open;
    commandPtr->m_Locked = locked;
    QPair<quint64, Global::Command*> event;
    event.first = time;
    event.second = commandPtr;
    m_commandList.push_back(event);
}

void SimulationCommandThreadController::insertRack(DeviceControl::DevInstanceID_t deviceId, qint32 rackId, qint32 rackUserData, DeviceControl::LoaderRFIDChannel_t stationId, quint64 time)
{
    DeviceCommandProcessor::CmdLoaderUpdate* commandPtr(new DeviceCommandProcessor::CmdLoaderUpdate(DeviceControl::DEVICE_INSTANCE_ID_LOADER));
    commandPtr->m_DevInstanceID = deviceId;
    commandPtr->m_RackId = rackId;
    commandPtr->m_Empty = (commandPtr->m_RackId == 0);
    commandPtr->m_StationID = stationId;
    commandPtr->m_RackUserData = rackUserData;
    QPair<quint64, Global::Command*> event;
    event.first = time;
    event.second = commandPtr;
    m_commandList.push_back(event);

    for (int i=0; i < m_freeStationId.size(); i++)
    {
        if (stationId == m_freeStationId[i])
            m_freeStationId.remove(i);
    }
    m_racksInserted++;
//    qDebug() << "xxxxxxxx SimulationCommandThreadController::insertRack" << m_freeStationId.size();
}

/****************************************************************************/
void SimulationCommandThreadController::OnGoReceived()
{
    qDebug() << "SimulationCommandThreadController::OnGoReceived";

    /// \todo implement
    QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(OnTimer()));

    // start timer to send something to GUI
    m_timer.setInterval(m_sampleTime);
//    m_Timer.setSingleShot(true);

    m_timer.start();
}

QByteArray SimulationCommandThreadController::getDuration(QString commandType, QByteArray configData) //(QString commandType)
{
    //QString commandType = command.GetPointerToUserData()->GetName();
    QByteArray ba;
    QDataStream ds(&ba, QIODevice::WriteOnly);

    if (commandType == "DeviceCommandProcessor::CmdGrapplerLetDownRack")
    {
        if (configData.count() > 0)
        {
            if (configData.at(0) > 0)
                ds << getMoveDownSlowDuration();
            else
                ds << getMoveDownDuration();
        }
        else
        {
            return 0;  // todo: error handling (info slowMode is missing)
        }
    }

    if (commandType == "DeviceCommandProcessor::CmdGrapplerPullUpRack")
    {
        if (configData.count() > 0)
        {
            if (configData.at(0) > 0)
                ds << getMoveUpSlowDuration();
            else
                ds << getMoveUpDuration();
        }
        else
        {
            return 0;  // todo: error handling (info slowMode is missing)
        }
    }

    if (commandType == "DeviceCommandProcessor::CmdGrapplerAttachToRack")
        ds << getAttachDuration();

    if (commandType == "DeviceCommandProcessor::CmdAgitationSetSpeed")
        ds << getActivateAgitationDuration();

    if (commandType == "DeviceCommandProcessor::CmdGrapplerDetachFromRack")
        ds << getDetachDuration();

    if (commandType == "DeviceCommandProcessor::CmdCountSlides")
        ds << getCountSlidesDuration();

    if (commandType == "DeviceCommandProcessor::CmdSetOvenCoverPostion")
        ds << getOpenOvenCoverDuration();

    if (commandType == "DeviceCommandProcessor::CmdSetWaterLevel")
        ds << getSetWaterLevelDuration();

    if (commandType == "DeviceCommandProcessor::CmdLoaderSetBlockingState")
        ds << getLoaderSetBlockingStateDuration();

    if (commandType == "DeviceCommandProcessor::CmdHeatStation")
    {
        for (int i = 0; i < configData.count(); i = i + 4)
        {
            quint8 row = configData.at(i);
            quint8 column = configData.at(i+1);
            quint8 temperature = configData.at(i+2);

            if ((row == 0) && (column == 0) && (temperature == 0))
                ds << (quint32)getHeatingSwitchDuration();
            else
                ds << (quint32)getHeatupDuration(row, column, temperature);
        }
    }

    if (commandType == "DeviceCommandProcessor::CmdGrapplerTransportRackTo")
    {
        for (int i = 0; i < configData.count(); i = i + 4)
        {
            quint8 sourceRow = configData.at(i);
            quint8 sourceColumn = configData.at(i+1);
            quint8 row = configData.at(i+2);
            quint8 column = configData.at(i+3);
            quint8 xDist = abs(row - sourceRow);
            quint8 yDist = abs(column - sourceColumn);
            double dist = sqrt(xDist * xDist + yDist * yDist);

            ds << (quint32)(dist * 1500.0);
        }
    }

    if (commandType == "DeviceCommandProcessor::CmdGrapplerMoveEmptyTo")
    {
        for (int i = 0; i < configData.count(); i = i + 4)
        {
            quint8 sourceRow = configData.at(i);
            quint8 sourceColumn = configData.at(i+1);
            quint8 row = configData.at(i+2);
            quint8 column = configData.at(i+3);
            quint8 xDist = abs(row - sourceRow);
            quint8 yDist = abs(column - sourceColumn);
            double dist = sqrt(xDist * xDist + yDist * yDist);

            ds << (quint32)(dist * 900.0);
        }
    }

    return ba;
}


quint32 SimulationCommandThreadController::getMoveDownDuration()
{
    return 950;
}

quint32 SimulationCommandThreadController::getMoveUpDuration()
{
    return 950;
}

quint32 SimulationCommandThreadController::getMoveDownSlowDuration()
{
    return 2000;
}

quint32 SimulationCommandThreadController::getMoveUpSlowDuration()
{
    return 3850;
}

quint32 SimulationCommandThreadController::getActivateAgitationDuration()
{
    return 100;
}

quint32 SimulationCommandThreadController::getAttachDuration()
{
    return 100;
}

quint32 SimulationCommandThreadController::getDetachDuration()
{
    return 100;
}

quint32 SimulationCommandThreadController::getCountSlidesDuration()
{
    return 5000;
}

quint32 SimulationCommandThreadController::getOpenOvenCoverDuration()
{
    return 5000;
}

quint32 SimulationCommandThreadController::getSetWaterLevelDuration()
{
    return 20000;
}

quint32 SimulationCommandThreadController::getLoaderSetBlockingStateDuration()
{
    return 1000;
}

quint32 SimulationCommandThreadController::getHeatingSwitchDuration()
{
    return 100;
}

quint32 SimulationCommandThreadController::getHeatupDuration(int row, int column, double targetTemperature)
{
    // todo: calculate heatup prediction according to station (given by row and column)
    // both for oven and heated cuvettes
    // time prediction is based on current state
    return 23000;
}


}
