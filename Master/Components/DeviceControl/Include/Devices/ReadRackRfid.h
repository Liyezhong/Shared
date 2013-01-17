/****************************************************************************/
/*! \file   ReadRackRfid.h
 *
 *  \brief  Definition file for class CReadRackRfid
 *
 *  \version  0.1
 *  \date     2012-11-22
 *  \author   M.Scherer
 *
 *  \b Description:
 *
 *   N/A
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#ifndef DEVICECONTROL_READRACKRFID_H
#define DEVICECONTROL_READRACKRFID_H

#include "DeviceState.h"
#include "DeviceControl/Include/Global/DeviceControl.h"
#include "DeviceControl/Include/SlaveModules/Rfid11785.h"

#ifdef Q_UNIT_TEST
#include "Simulator.h"
#endif

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  State machine for reading the RFID tag of a rack
 */
/****************************************************************************/
class CReadRackRfid : public CState
{
    Q_OBJECT

public:
    explicit CReadRackRfid(CRfid11785 *p_Rfid11785, const QString &Name, QState *p_Parent = 0);

signals:
    // command request interface to DCP
    /****************************************************************************/
    /*!
     *  \brief  Reads the data from an RFID tag
     *
     *  \iparam Channel = RFID channel used for the request
     */
    /****************************************************************************/
    void ReadRackRfid(LoaderRFIDChannel_t Channel);

    // command response interface to DCP
    /****************************************************************************/
    /*!
     *  \brief  Signals the result of the get temperature function
     *
     *  \iparam ReturnCode = ReturnCode of Device Control Layer
     *  \iparam Channel = RFID channel used for the request
     *  \iparam Uid = Unique ID of the RFID tag
     *  \iparam Data = Data word read from the tag
     */
    /****************************************************************************/
    void ReportReadRackRfid(ReturnCode_t ReturnCode, LoaderRFIDChannel_t Channel, quint32 Uid, quint32 Data);

    // to this
    /****************************************************************************/
    /*!
     *  \brief  Internal signal forwarding error information
     *
     *  \iparam ReturnCode = ReturnCode of Device Control Layer
     */
    /****************************************************************************/
    void RfidError(ReturnCode_t ReturnCode);
    
private:
    bool SetState(QEvent *p_Event);
    bool SendLogin(QEvent *p_Event);
    bool ReadUid(QEvent *p_Event);
    bool ReadData(QEvent *p_Event);
    bool ReportRead(QEvent *p_Event);
    bool Nack(QEvent *p_Event);

    CRfid11785 *mp_Rfid11785;       //!< RFID ISO 11785 function module
    LoaderRFIDChannel_t m_Channel;  //!< RFID channel used for the request
    quint32 m_Uid;                  //!< Unique ID of the RFID tag
    quint32 m_Data;                 //!< Data word read from the tag
};

} //namespace

#endif // DEVICECONTROL_READRACKRFID_H
