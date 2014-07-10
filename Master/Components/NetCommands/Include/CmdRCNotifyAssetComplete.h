/****************************************************************************/
/*! \file CmdRCNotifyAssetComplete.h
 *
 *  \brief CmdRCNotifyAssetComplete command definition.
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
#ifndef NETCOMMANDS_CMDRCNOTIFYASSETCOMPLETE_H
#define NETCOMMANDS_CMDRCNOTIFYASSETCOMPLETE_H
#include <Global/Include/Commands/Command.h>

namespace NetCommands {
/****************************************************************************/
/*!
 *  \brief  This class implements a CmdRCNotifyAssetComplete command.
 */
/****************************************************************************/
class CmdRCNotifyAssetComplete : public Global::Command {
    friend QDataStream & operator << (const QDataStream &, const CmdRCNotifyAssetComplete &);
    friend QDataStream & operator >> (const QDataStream &, const CmdRCNotifyAssetComplete &);
public:
    static QString NAME;    //!< Command name.
    CmdRCNotifyAssetComplete(const int& TimeOut,const QString& FilePath);
    CmdRCNotifyAssetComplete();
    ~CmdRCNotifyAssetComplete();
    virtual QString GetName() const;
    QString GetFilePath() const;

private:
    CmdRCNotifyAssetComplete(const CmdRCNotifyAssetComplete &);                       //!< Not implemented.
    const CmdRCNotifyAssetComplete & operator = (const CmdRCNotifyAssetComplete &);   //!< Not implemented.
    QString m_FilePath;   //!< asset archive file path
};


/****************************************************************************/
/**
 * \brief Output Stream Operator which streams data
 *
 * \iparam   Stream      Stream to stream into.
 * \iparam   Cmd         The command to stream.
 * \return   Output Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (const QDataStream &Stream, const CmdRCNotifyAssetComplete &Cmd)
{
    QDataStream& StreamRef = const_cast<QDataStream &>(Stream);
    // copy base class data
    Cmd.CopyToStream(StreamRef);
    // copy internal data
    StreamRef << Cmd.m_FilePath;
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
inline QDataStream & operator >> (const QDataStream &Stream, const CmdRCNotifyAssetComplete &Cmd)
{
    QDataStream& StreamRef = const_cast<QDataStream &>(Stream);
    CmdRCNotifyAssetComplete& CmdRef = const_cast<CmdRCNotifyAssetComplete &>(Cmd);

    // copy base class data
    CmdRef.CopyFromStream(StreamRef);
    // copy internal data
    StreamRef >> CmdRef.m_FilePath;
    return StreamRef;
}

} // End of namespace NetCommands
#endif // NETCOMMANDS_CMDRCNOTIFYASSETCOMPLETE_H
