/****************************************************************************/
/*! \file CANInterface.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CANInterface
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

#include <cerrno>

#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <linux/can/error.h>

#include "DeviceControl/Include/CanCommunication/CANCommunicator.h"
#include "DeviceControl/Include/CanCommunication/CANInterface.h"
#include "DeviceControl/Include/Global/dcl_log.h"

namespace DeviceControl
{

/* At time of writing, these constants are not defined in the headers */
#ifndef PF_CAN
#define PF_CAN 29  //!< protocoll family
#endif

#ifndef AF_CAN
#define AF_CAN PF_CAN    //!< protocoll family
#endif

#define ERROR_CANINTERFACE_OK            0  //!< success
#define ERROR_CANINTERFACE_UNDEF        -1  //!< undefined error
#define ERROR_CANINTERFACE_NAME         -2  //!< interface name invalid
#define ERROR_CANINTERFACE_SOCKET       -3  //!< general socket error
#define ERROR_CANINTERFACE_SOCKET_IOCTL -4  //!< socket error ioctl
#define ERROR_CANINTERFACE_SOCKET_BIND  -5  //!< socket error bind
#define ERROR_CANINTERFACE_SOCKET_OPT   -6  //!< socket error options
#define ERROR_CANINTERFACE_WRITE        -7  //!< write error at 'can' socket

/****************************************************************************/
/*!
 *  \brief  Constructor for the CANInterface class
 */
/****************************************************************************/
CANInterface::CANInterface() : m_sockCan(0)
{
}

/****************************************************************************/
/*!
 *  \brief  Open the CAN interface
 *
 *  \iparam szOpenInterface = can interface string
 *
 *  \return 0 if successfully opened, otherwise a negative value
 */
/****************************************************************************/
// see https://www.kernel.org/doc/Documentation/networking/can.txt
// or
// see example at http://www.can-cia.org/fileadmin/cia/files/icc/13/kleine-budde.pdf
// chapter "Applications and the CAN_RAW protocol"
qint16 CANInterface::Open(const char* szOpenInterface)
{
    qint16 sRetval = ERROR_CANINTERFACE_UNDEF;
    struct sockaddr_can addr;
    can_err_mask_t err_mask;
    int nbytes;
    struct ifreq ifr;

    FILE_LOG_L(laINIT, llDEBUG) << " enter function";

    nbytes = strlen(szOpenInterface);
    if (nbytes >= 9)
    {
        FILE_LOG_L(laINIT, llERROR) << " name of CAN device '%s' is too long!\n" << szOpenInterface;
        return ERROR_CANINTERFACE_NAME;
    }
    FILE_LOG_L(laINIT, llDEBUG) << " try to open '" << szOpenInterface << "'";

    // open CAN_RAW socket to allow send and receive of raw CAN frames
    m_sockCan = socket(PF_CAN, (qint32)SOCK_RAW | (qint32)SOCK_NONBLOCK, CAN_RAW);
    if (m_sockCan < 0)
    {
        FILE_LOG_L(laINIT, llDEBUG) << " error while opening " << szOpenInterface;
        return ERROR_CANINTERFACE_SOCKET;
    }
    FILE_LOG_L(laINIT, llDEBUG) << " socket " << m_sockCan << " created";

    addr.can_family = AF_CAN;

    char* pifr_name = &ifr.ifr_name[0];
    memset(pifr_name, 0, sizeof(ifr.ifr_name));
    strncpy(ifr.ifr_name, szOpenInterface, nbytes);

    // convert interface sting "can0" into interface index
    strcpy(ifr.ifr_name, szOpenInterface);
    if (ioctl(m_sockCan, SIOCGIFINDEX, &ifr) < 0)
    {
        std::string strError(strerror(errno));
        FILE_LOG_L(laINIT, llDEBUG) << " Error while open can interface 0: " << strError;
        return ERROR_CANINTERFACE_SOCKET_IOCTL;
    }

    //setup address for bind
    addr.can_ifindex = ifr.ifr_ifindex;
    // bind socket to the can0 interface
    if (bind(m_sockCan, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        FILE_LOG_L(laINIT, llDEBUG) << " Error while bind socket can interface 0: ";
        return ERROR_CANINTERFACE_SOCKET_BIND;
    }

    //Error handling options
    err_mask = (CAN_ERR_TX_TIMEOUT | CAN_ERR_LOSTARB | CAN_ERR_CRTL | CAN_ERR_PROT | CAN_ERR_TRX |
                CAN_ERR_ACK | CAN_ERR_BUSOFF | CAN_ERR_BUSERROR | CAN_ERR_RESTARTED );
    sRetval = setsockopt(m_sockCan, SOL_CAN_RAW, CAN_RAW_ERR_FILTER, &err_mask, sizeof(err_mask));
    if (sRetval < 0)
    {
        FILE_LOG_L(laINIT, llERROR) << "rt_dev_setsockopt: %s\n" << strerror(-sRetval);
        return ERROR_CANINTERFACE_SOCKET_OPT;
    }
    FILE_LOG_L(laINIT, llDEBUG) << "Using err_mask " << std::hex << err_mask;

    FILE_LOG_L(laINIT, llDEBUG) << " interface " << ifr.ifr_name << " successfully opened";

    sRetval = ERROR_CANINTERFACE_OK;

    return sRetval;
}

/****************************************************************************/
/*!
 *  \brief  Close the CAN interface
 */
/****************************************************************************/
void CANInterface::Close()
{
    close(m_sockCan);
    FILE_LOG_L(laINIT, llDEBUG) << " interface socket " << m_sockCan << " closed";
}

/****************************************************************************/
/*!
 *  \brief  Writes a can message's data to to can socket
 *
 *  \iparam CanMsg = The CAN message to send
 *
 *  \return negative value in case of an error
 */
/****************************************************************************/
int CANInterface::Write(can_frame & CanMsg)
{
    CanMsg.can_id |= CAN_EFF_FLAG;
    CanMsg.can_id &= ~CAN_RTR_FLAG;

    return write(m_sockCan, &CanMsg, sizeof(can_frame));
}

/****************************************************************************/
/*!
 *  \brief  Check if can message is ready for reading from the CAN socket interface
 *
 *  \return negative value in case of an error,
 *          zero if not ready.
 *          positive value if ready
 */
/****************************************************************************/
int CANInterface::Ready()
{
/* Will also return 1 if interface is down.
   In this case the next read will return an error.

   see linux man page for select.
   quote: "Those listed in readfds will be watched to see if characters become
   available for reading (more precisely, to see if a read  will  not  block;
   in  particular,  a  file descriptor  is  also ready on end-of-file)"
*/

    //lint -esym(530, ReadFileDescriptors)
    fd_set ReadFileDescriptors;
    struct timeval TimeValue;

    FD_ZERO(&ReadFileDescriptors);              //lint !e529
    FD_SET(m_sockCan, &ReadFileDescriptors);    //lint !e573

    TimeValue.tv_sec = 0;
    TimeValue.tv_usec = 500000;

    return select(m_sockCan + 1, &ReadFileDescriptors, NULL, NULL, &TimeValue);
}


/****************************************************************************/
/*!
 *  \brief  Read a can message's data from the CAN socket interface
 *
 *  \iparam pCanMsg = The CAN message to read, will be filled
 *
 *  \return negative value in case of an error
 */
/****************************************************************************/
int CANInterface::Read(can_frame* pCanMsg)
{
    return read(m_sockCan, pCanMsg, sizeof(can_frame));
}

} //namespace
