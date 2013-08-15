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
qint16  CANInterface::Open(const char* szOpenInterface)
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

    m_sockCan = socket(PF_CAN, SOCK_RAW | SOCK_NONBLOCK, CAN_RAW); //lint !e641 !e655
    if (m_sockCan < 0)
    {
        perror("socket");
        FILE_LOG_L(laINIT, llDEBUG) << " error while opening " << szOpenInterface;
        return ERROR_CANINTERFACE_SOCKET;
    }
    FILE_LOG_L(laINIT, llDEBUG) << " socket " << m_sockCan << " created";

    addr.can_family = AF_CAN;

    char* pifr_name = &ifr.ifr_name[0];
    memset(pifr_name, 0, sizeof(ifr.ifr_name));
    //memset(&ifr.ifr_name, 0, sizeof(ifr.ifr_name));
    strncpy(ifr.ifr_name, szOpenInterface, nbytes);

    strcpy(ifr.ifr_name, szOpenInterface); /// \todo constanten raus
    if (ioctl(m_sockCan, SIOCGIFINDEX, &ifr) < 0)
    {
        perror("SIOCGIFINDEX");
        std::string strError(strerror(errno));
        FILE_LOG_L(laINIT, llDEBUG) << " Error while open can interface 0: " << strError;
        return ERROR_CANINTERFACE_SOCKET_IOCTL;
    }
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(m_sockCan, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        FILE_LOG_L(laINIT, llDEBUG) << " Error while bind socket can interface 0: ";
        return ERROR_CANINTERFACE_SOCKET_BIND;
    }
    //Error handling options
    err_mask = ( CAN_ERR_TX_TIMEOUT | CAN_ERR_LOSTARB | CAN_ERR_CRTL | CAN_ERR_PROT | CAN_ERR_TRX | CAN_ERR_ACK | CAN_ERR_BUSOFF | CAN_ERR_BUSOFF | CAN_ERR_RESTARTED );
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
 *  \iparam pCanMsg = The CAN message to send
 *
 *  \return negative vlaue in case of an error
 */
/****************************************************************************/
int CANInterface::Write(const can_frame* pCanMsg)
{
    int    nWriteResult = 0;
    short  sIndex;
    struct can_frame frame;

    frame.can_id  = pCanMsg->can_id;
    frame.can_id |= CAN_EFF_FLAG;
    frame.can_id &= ~CAN_RTR_FLAG;

    for(sIndex=0; sIndex<8; sIndex++)
        frame.data[sIndex] = pCanMsg->data[sIndex];
    frame.can_dlc = pCanMsg->can_dlc;

    /* send frame */
    /// \todo Error handling, acknowledge sending
    // refer to socketcan mailinglist
    if ((nWriteResult = write(m_sockCan, &frame, sizeof(frame))) != sizeof(frame))
    {
        perror("Colorado, write socketcan:");
        return ERROR_CANINTERFACE_WRITE;
    }

    if(nWriteResult < 0)
    {
        FILE_LOG_L(laCAN, llERROR) << "can write error " << nWriteResult;
    }

    return nWriteResult;
}

/****************************************************************************/
/*!
 *  \brief  Read a can message's data from the CAN socket interface
 *
 *  \iparam pCanMsg = The CAN message to read, will be filled
 *
 *  \return negative vlaue in case of an error
 */
/****************************************************************************/
int CANInterface::Read(can_frame* pCanMsg)
{
    struct can_frame frame;
    int bytes_read, sIndex;
    fd_set ReadFileDescriptors;
    struct timeval TimeValue;
    qint32 ReturnValue;

    FD_ZERO(&ReadFileDescriptors); //lint !e529
    FD_SET(m_sockCan, &ReadFileDescriptors); //lint !e573 !e530

    TimeValue.tv_sec = 0;
    TimeValue.tv_usec = 500000;

    ReturnValue = select(m_sockCan + 1, &ReadFileDescriptors, NULL, NULL, &TimeValue);
    if (ReturnValue < 0)
    {
        return ERROR_CANINTERFACE_SOCKET;
    }
    else if (ReturnValue == 0)
    {
        return 0;
    }

    /* Read a message back from the CAN bus */
    bytes_read = read(m_sockCan, &frame, sizeof(frame));

    if(bytes_read == sizeof(frame))
    {
        //************ Norbert's information, SEP11 *******************
        // any errors detected by the can driver should be signaled here.
        // This doesn't work so far with the phytec driver, but phytec was kicked out, and I was ordered to spend no more time on it.
        // when starting with LNT, refer to their CAN-drivers error masking information and adapt changes, if any.
        // use 'ifconfig can0' at command line, or procfs to ensure error detection by the driver itself
        // Bus-Warning, BusOff shall be detected.
        // connect CAN H/L to CAN-GND, this will force an error.
        // Use PeakCAN, which will report CAN-Bus errors as well
        if (frame.can_id & CAN_ERR_FLAG)
        {
            FILE_LOG_L(laCAN, llWARNING) << " Error-CanID: " << std::hex << (frame.can_id & CAN_ERR_MASK);
            if (frame.can_id & CAN_ERR_BUSOFF)
            {
                FILE_LOG_L(laCAN, llWARNING) << " Error: Bus-Off";
            }
            if (frame.can_id & CAN_ERR_CRTL)
            {
                FILE_LOG_L(laCAN, llWARNING) << " Error: controller problem";
            }
            return ERROR_CANINTERFACE_SOCKET;
        }
        else if (frame.can_id & CAN_EFF_FLAG)
        {
            //if you need a loging of each CAN-message, use this:
            FILE_LOG_L(laCAN, llDEBUG2) << " CanID: " << std::hex << (frame.can_id & CAN_EFF_MASK) <<
                                                 " "  << std::hex << (int)frame.data[0] <<
                                                 " "  << std::hex << (int)frame.data[1] <<
                                                 " "  << std::hex << (int)frame.data[2] <<
                                                 " "  << std::hex << (int)frame.data[3] <<
                                                 " "  << std::hex << (int)frame.data[4] <<
                                                 " "  << std::hex << (int)frame.data[5] <<
                                                 " "  << std::hex << (int)frame.data[6] <<
                                                 " "  << std::hex << (int)frame.data[7];
            pCanMsg->can_id  = frame.can_id & CAN_EFF_MASK;
        }
        else
        {
            pCanMsg->can_id  = frame.can_id & CAN_SFF_MASK;
            FILE_LOG_L(laCAN, llERROR) << " standard CANID received: 0x" << std::hex << (frame.can_id & CAN_SFF_MASK);
        }

        /// \todo error message handling, at this time just standard messages will be forwarded
        pCanMsg->can_id  = frame.can_id & 0x1FFFFFFF;
        pCanMsg->can_dlc  = frame.can_dlc;
        for(sIndex=0; sIndex<8; sIndex++)
        {
            if(sIndex < frame.can_dlc)
                pCanMsg->data[sIndex]  = frame.data[sIndex];
            else
                pCanMsg->data[sIndex] = 0x00;
        }
        return bytes_read;
    }
    else
    {
        FILE_LOG_L(laCAN, llERROR) << " bytes read not match can_frame size: " << bytes_read;
    }
    return ERROR_CANINTERFACE_SOCKET;
}

/****************************************************************************/
/*!
 *  \brief  Return the status of the communication socket
 *
 *  \return Status information (not implemented so far)
 */
/****************************************************************************/
int CANInterface::GetStatus()
{
    int val = 0;
    //ioctl(m_sockCan, IOC_GET_STATUS, &val);
    return val;
}

} //namespace
