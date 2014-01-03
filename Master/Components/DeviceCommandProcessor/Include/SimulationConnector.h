/****************************************************************************/
/*! \file SimulationConnector.h
 *
 *  \brief Definition file for class SimulationConnector.
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2012-11-16
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
#ifndef SIMULATIONCONNECTOR_H
#define SIMULATIONCONNECTOR_H

#include "NetworkComponents/Include/NetworkServerDevice.h"
#include <QVector>

namespace DeviceCommandProcessor {

/**
 * \brief Simulates a DeviceCommandProcessor, running without hardware
 */
class SimulationConnector : public QObject
{
    Q_OBJECT

public:
    /**
     * \brief   Constructor
     * \iparam  port = TCP/IP port
     */
    SimulationConnector(quint16 port);

    /**
     * \brief   Destructor
     */
    ~SimulationConnector();

    /**
     * \brief Reads a XML structure out of a file
     *
     * \iparam  filename = command sequence file name
     *
     * \return  QDomDocument reference
     */
    QDomDocument ReadXmlFromFile(QString filename);

    /**
     * \brief Starts simulation GUI
     *
     * \iparam  fileName = gui executable
     */
    void startSimulationGui(QString fileName);

    /**
     * \brief Stops simulation GUI
     *
     * \iparam  fileName = gui executable
     */
    void stopSimulationGui(QString fileName);

    /**
     * \brief Sends message to client
     *
     * \iparam  message = message
     */
    void sendMessageToClient(QString message);

    /**
     * \brief Handle to pipe broken signal
     *
     * \iparam  sig = Signal
     */
    static void handleBrokenPipe(int sig);

signals:
    /**
     * \brief   Message from client
     *
     * \iparam  message = Message from client
     */
    void messageFromClient(QString message);

private:
    QTcpServer *m_tcpServer;            //!< tcp server
    QTcpSocket *m_clientConnection;     //!< tcp client

    QFile *m_pCommfile;                 //!< communication file
    QTextStream m_outStream;            //!< out stream
    bool m_verbose;                     //!< verbose state
    QStringList m_pendingMessages;      //!< list of messages
    QTimer *m_pReadTimer;               //!< read timer
    static bool m_pipeBroken;           //!< pipe status

    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(SimulationConnector)

private slots:
    /**
     * \brief   Registers Connection
     */
    void registerConnection();

    /**
     * \brief   Reads message from client
     */
    void readMessageFromClient();
};

}
#endif // SIMULATIONCONNECTOR_H
