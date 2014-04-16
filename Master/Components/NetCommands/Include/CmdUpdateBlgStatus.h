/****************************************************************************/
/*! \file CmdUpdateBlgStatus.h
 *
 *  \brief CmdUpdateBlgStatus command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 19.12.2012
 *   $Author:  $ Ningu
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

#ifndef NETCOMMANDS_CMDUPDATEBLGSTATUS_H
#define NETCOMMANDS_CMDUPDATEBLGSTATUS_H

#include <Global/Include/Commands/Command.h>

namespace NetCommands {

//! Bathlayout generator status indicator
typedef enum
{
    BATHLAYOUT_GENERATION_FAILED,       //!< Bathlayout generator process failed
    BATHLAYOUT_GENERATION_OK,           //!< Bathlayout generator process succeeded
    VERIFY_PROG_SEQUENCE_BLG_FAILED,    //!< ProgramSequece_BLG data verification failed
    READING_PROG_SEQUENCE_BLG_FAILED,   //!< ProgramSequence_BLG data reading failed
    VERIFY_STATIONS_BLG_FAILED,         //!< Stations_BLG data verification failed
    READING_STATIONS_BLG_FAILED,        //!< Stations_BLG data reading failed
    ERROR_STATION_NULL_POINTER,         //!< Stations pointer is null
    ERROR_CONTAINER_NULL_POINTER        //!< Datacontainer pointer is null
} BlgStatus_t;

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdUpdateBlgStatus command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdUpdateBlgStatus : public Global::Command {
public:
    static QString NAME;    //!< Command name.
    friend QDataStream & operator << (QDataStream &, const CmdUpdateBlgStatus &);
    friend QDataStream & operator >> (QDataStream &, CmdUpdateBlgStatus &);
    /****************************************************************************/
    CmdUpdateBlgStatus(int Timeout,const BlgStatus_t BlgStatusId);
    ~CmdUpdateBlgStatus();
    virtual QString GetName() const;            //!< Command name.
    CmdUpdateBlgStatus();
    BlgStatus_t GetBlgStatusMsgId() const;      //!< Returns Blg status id (struct: BlgStatus_t).
private:
    CmdUpdateBlgStatus(const CmdUpdateBlgStatus &);                       //!< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdUpdateBlgStatus & operator = (const CmdUpdateBlgStatus &);   //!< Not implemented.
private:       
    BlgStatus_t m_BlgStatusId;     ///< BLG status message identifier
}; // end class CmdUpdateBlgStatus
/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdUpdateBlgStatus &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << (int)Cmd.m_BlgStatusId;
    return Stream;
}

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream from.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdUpdateBlgStatus &Cmd)
{
    qint32 MessageID;
    // copy base class data
    Cmd.CopyFromStream(Stream);
    Stream >> MessageID;
    Cmd.m_BlgStatusId = (BlgStatus_t)MessageID;    
    return Stream;
}
/****************************************************************************/
} // end namespace NetCommands

#endif // NETCOMMANDS_CMDUPDATEBLGSTATUS_H
