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
  @brief Simulates a DeviceCommandProcessor, running without hardware
  */
class SimulationConnector : public QObject
{
    Q_OBJECT

public:
    SimulationConnector();

    /**
      @brief Reads a XML structure out of a file
      */
    QDomDocument ReadXmlFromFile(QString filename);
    void startSimulationGui();
    void sendMessageToClient(QString message);

signals:
    void messageFromClient(QString message);

private:

    QTcpServer *m_tcpServer;
    QTcpSocket *m_clientConnection;

    QFile *m_commfile;
    QTextStream m_outStream;
    bool m_verbose;

private slots:
    void registerConnection();
    void readMessageFromClient();
};

}
#endif // SIMULATIONCONNECTOR_H
