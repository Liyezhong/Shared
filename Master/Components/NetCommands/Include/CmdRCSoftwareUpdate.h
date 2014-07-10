/****************************************************************************/
/*! \file NetCommands/Include/CmdRCSoftwareUpdate.h
 *
 *  \brief CmdRCSoftwareUpdate command definition.
 *
 *   $Version: $ 1.0
 *   $Date:    $ 2014-03-13
 *   $Author:  $ Ramya GJ
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

#ifndef REMOTECARE_CMDRCSOFTWAREUPDATE_H
#define REMOTECARE_CMDRCSOFTWAREUPDATE_H

#include <Global/Include/Commands/Command.h>
#include <Global/Include/GlobalDefines.h>


namespace RemoteCare {

/****************************************************************************/
/**
 * \brief sw update status type.
 */
/****************************************************************************/
const QString   SWUpdate_Available  = "Available";        //!< new software available for update
const QString   SWUpdate_NotAvailable   = "NotAvailable";       //!< no new software available for update
const QString   SWUpdate_DownloadSuccess    = "DownloadSuccess";       //!< Downloading new software failed
const QString   SWUpdate_DownloadFailed = "DownloadFailed";       //!< Downloading new software failed
const QString   SWUpdate_StartDownload  = "StartDownload";       //!< User clicked on RC software update button

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdRCSoftwareUpdate command.
 */
/****************************************************************************/
class CmdRCSoftwareUpdate : public Global::Command {
    friend QDataStream & operator << (const QDataStream &, const CmdRCSoftwareUpdate &);
    friend QDataStream & operator >> (const QDataStream &, const CmdRCSoftwareUpdate &);
public:
    static QString NAME;    //!< Command name.
    CmdRCSoftwareUpdate(const int& Timeout, const QString& SWUpdateType);
    CmdRCSoftwareUpdate();
    ~CmdRCSoftwareUpdate();
    virtual QString GetName() const;
    QString GetUpdateType() const;
private:
    
    CmdRCSoftwareUpdate(const CmdRCSoftwareUpdate &);                       //!< Not implemented.
    const CmdRCSoftwareUpdate & operator = (const CmdRCSoftwareUpdate &);   //!< Not implemented.
private:
    QString m_SWUpdateType;      //!< update type
}; // end class CmdRCSoftwareUpdate

/****************************************************************************/
/**
     * \brief Output Stream Operator which streams data
     *
     * \iparam   Stream      Stream to stream into.
     * \iparam   Cmd         The command to stream.
     * \return   Output Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (const QDataStream &Stream, const CmdRCSoftwareUpdate &Cmd)
{
    QDataStream& StreamRef = const_cast<QDataStream &>(Stream);
    // copy base class data
    Cmd.CopyToStream(StreamRef);
    StreamRef << Cmd.m_SWUpdateType;
    return StreamRef;
}

/****************************************************************************/
/**
     * \brief Input Stream Operator which streams data
     *
     * \iparam   Stream      Stream to stream from.
     * \iparam   Cmd         The command to stream.
     * \return   Input Stream.
     */
/****************************************************************************/
inline QDataStream & operator >> (const QDataStream &Stream, const CmdRCSoftwareUpdate &Cmd)
{
    QDataStream& StreamRef = const_cast<QDataStream &>(Stream);
    CmdRCSoftwareUpdate& CmdRef = const_cast<CmdRCSoftwareUpdate &>(Cmd);

	// copy base class data
    CmdRef.CopyFromStream(StreamRef);
    StreamRef >> CmdRef.m_SWUpdateType;
    return StreamRef;
}

} // end namespace RemoteCare

#endif // REMOTECARE_CMDRCSOFTWAREUPDATE_H
