/****************************************************************************/
/*! \file CmdRCNotifyAssetComplete.h
 *
 *  \brief CmdRCNotifyAssetComplete command definition.
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
    friend QDataStream & operator << (QDataStream &, const CmdRCNotifyAssetComplete &);
    friend QDataStream & operator >> (QDataStream &, CmdRCNotifyAssetComplete &);
public:
    static QString NAME;    ///< Command name.
    CmdRCNotifyAssetComplete(int TimeOut,const QString &FilePath);
    CmdRCNotifyAssetComplete();
    ~CmdRCNotifyAssetComplete();
    virtual QString GetName() const;
    QString GetFilePath() const;

private:
    CmdRCNotifyAssetComplete(const CmdRCNotifyAssetComplete &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdRCNotifyAssetComplete & operator = (const CmdRCNotifyAssetComplete &);   ///< Not implemented.
    QString m_FilePath;   ///< asset archive file path
};


/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \iparam   Stream      Stream to stream into.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdRCNotifyAssetComplete &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_FilePath;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdRCNotifyAssetComplete &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_FilePath;
    return Stream;
}

} // End of namespace NetCommands
#endif // NETCOMMANDS_CMDRCNOTIFYASSETCOMPLETE_H
