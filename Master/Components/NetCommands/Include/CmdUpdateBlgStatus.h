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

typedef enum
{
    BATHLAYOUT_GENERATION_FAILED,
    BATHLAYOUT_GENERATION_OK,
    VERIFY_PROG_SEQUENCE_BLG_FAILED,
    READING_PROG_SEQUENCE_BLG_FAILED,
    VERIFY_STATIONS_BLG_FAILED,
    READING_STATIONS_BLG_FAILED,
    ERROR_STATION_NULL_POINTER,
    ERROR_CONTAINER_NULL_POINTER
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
    static QString NAME;    ///< Command name.
    friend QDataStream & operator << (QDataStream &, const CmdUpdateBlgStatus &);
    friend QDataStream & operator >> (QDataStream &, CmdUpdateBlgStatus &);
    /****************************************************************************/
    CmdUpdateBlgStatus(int Timeout,const BlgStatus_t BlgStatusId);
    ~CmdUpdateBlgStatus();
    virtual QString GetName() const;
    QString GetBathLayoutStatus() const;
    CmdUpdateBlgStatus();
    BlgStatus_t GetBlgStatusMsgId() const;    
private:
    CmdUpdateBlgStatus(const CmdUpdateBlgStatus &);                       ///< Not implemented.
    const CmdUpdateBlgStatus & operator = (const CmdUpdateBlgStatus &);   ///< Not implemented.
private:       
    BlgStatus_t m_BlgStatusId;     ///< BLG status message identifier
}; // end class CmdUpdateBlgStatus
/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
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
 * \param[in]       Cmd         The command to stream.
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

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDUPDATEBLGSTATUS_H
