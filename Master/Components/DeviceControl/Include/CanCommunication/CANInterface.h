/****************************************************************************/
/*! \file CANInterface.h
 *
 *  \brief
 *
 *   Version: $ 0.1
 *   Date:    $ 08.07.2010
 *   Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the CANInterface class
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

#ifndef CANINTERFACE_H
#define CANINTERFACE_H

#include <sys/socket.h>
#include <linux/can.h>
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief CAN interface class
 *
 *      This class opens, closes, reads and writes to operating system's
 *      socket can interface.
 */
/****************************************************************************/
class CANInterface
{
public:
    CANInterface();
    //! Open the can interface
    qint16 Open(const char* szOpenInterface);
    //! Close the can interface
    void Close();
    //! Write can message to device
    int Write(can_frame &CanMsg);
    //! check if msg is ready for reading
    int Ready();
    //! Read can message from device
    int Read(can_frame* canmsg);

private:
    int m_sockCan;  //!< Communication socket

    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CANInterface)
};

} //namespace

#endif /* CANINTERFACE_H */
