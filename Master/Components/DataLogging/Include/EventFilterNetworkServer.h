/****************************************************************************/
/*! \file DataLogging/Include/EventFilterNetworkServer.h
 *
 *  \brief Definition file for class EventFilterNetworkServer.
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2013-10-16
 *  $Author:    $ Raju
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
    friend class TestEventFilterNetworkServer;
    Q_OBJECT
private:
    QTcpServer      *m_pServer;         ///< Socket server.
    QTcpSocket      *m_pSocket;         ///< Currently used socket.
    /****************************************************************************/
    /****************************************************************************/
    /**
     * \brief Constructor.
     */
    /****************************************************************************/
    EventFilterNetworkServer();              ///< Not implemented.

    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(EventFilterNetworkServer)

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
     * \iparam  rLines  Received data.
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
     * \iparam   pParent     pointer to parent object.
     * \iparam   Port        Port to listen on.
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
