/****************************************************************************/
/*! \file DataLogging/Include/EventFilterNetworkServer.h
 *
 *  \brief Definition file for class EventFilterNetworkServer.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
 *  $Author:    $ J.Bugariu
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

#ifndef DATALOGGING_EVENTFILTERNETWORKSERVER_H
#define DATALOGGING_EVENTFILTERNETWORKSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QAbstractSocket>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Network server for \ref EventFilter.
 *
 * This class listens to a socket port, accepts incoming connections, reads
 * data and feeds processed information to the \ref EventFilter.
 */
/****************************************************************************/
class EventFilterNetworkServer : public QObject {
    Q_OBJECT
private:
    QTcpServer      *m_pServer;         ///< Socket server.
    QTcpSocket      *m_pSocket;         ///< Currently used socket.
    /****************************************************************************/
    EventFilterNetworkServer();                                                     ///< Not implemented.
    EventFilterNetworkServer(const EventFilterNetworkServer &);                     ///< Not implemented.
    const EventFilterNetworkServer & operator = (const EventFilterNetworkServer &); ///< Not implemented.
    /****************************************************************************/
    /**
     * \brief Read and process all data from socket.
     */
    /****************************************************************************/
    void ReadData();
    /****************************************************************************/
    /**
     * \brief Process data in protocol version 1.
     *
     * \param[in, out]  rLines  Received data.
     */
    /****************************************************************************/
    void ProcessProtocolVersion_1(QStringList &rLines) const;
private slots:
    /****************************************************************************/
    /**
     * \brief Accept a new connection.
     *
     * A new socket is created and stored in m_pSocket.
     */
    /****************************************************************************/
    void AcceptNewConnection();
    /****************************************************************************/
    /**
     * \brief Socket disconnected.
     *
     * Read all pending data and process it. Delete m_pSocket.
     */
    /****************************************************************************/
    void ConnectionDisconnected();
protected:
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   pParent     pointer to parent object.
     * \param[in]   Port        Port to listen on.
     */
    /****************************************************************************/
    EventFilterNetworkServer(QObject *pParent, quint16 Port);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~EventFilterNetworkServer();
}; // end class EventFilterNetworkServer

} // end namespace DataLogging

#endif // DATALOGGING_EVENTFILTERNETWORKSERVER_H
