/****************************************************************************/
/*! \file NetCommands/Include/CmdRCSoftwareUpdate.h
 *
 *  \brief CmdRCSoftwareUpdate command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 30.04.2013
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
 * \brief remote care data item quality.
 */
/****************************************************************************/
const QString   SWUpdate_Available  = "Available";        ///< new software available for update
const QString   SWUpdate_NotAvailable   = "NotAvailable";       ///< no new software available for update
const QString   SWUpdate_DownloadSuccess    = "DownloadSuccess";       ///< Downloading new software failed
const QString   SWUpdate_DownloadFailed = "DownloadFailed";       ///< Downloading new software failed
const QString   SWUpdate_StartDownload  = "StartDownload";       ///< User clicked on RC software update button

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdRCSoftwareUpdate command.
 */
/****************************************************************************/
class CmdRCSoftwareUpdate : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdRCSoftwareUpdate &);
    friend QDataStream & operator >> (QDataStream &, CmdRCSoftwareUpdate &);
public:
    static QString NAME;    ///< Command name.
    CmdRCSoftwareUpdate(int Timeout, QString SWUpdateType);
    CmdRCSoftwareUpdate();
    ~CmdRCSoftwareUpdate();
    virtual QString GetName() const;
    QString GetUpdateType() const;
private:
    
    CmdRCSoftwareUpdate(const CmdRCSoftwareUpdate &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdRCSoftwareUpdate & operator = (const CmdRCSoftwareUpdate &);   ///< Not implemented.
private:
    QString m_SWUpdateType;      ///< update type
}; // end class CmdRCSoftwareUpdate

/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \iparam   Stream      Stream to stream into.
     * \iparam       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdRCSoftwareUpdate &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    Stream << Cmd.m_SWUpdateType;
    return Stream;
}

/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \iparam   Stream      Stream to stream from.
     * \iparam       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdRCSoftwareUpdate &Cmd)
{
	// copy base class data
    Cmd.CopyFromStream(Stream);
    Stream >> Cmd.m_SWUpdateType;
    return Stream;
}

} // end namespace RemoteCare

#endif // REMOTECARE_CMDRCSOFTWAREUPDATE_H
