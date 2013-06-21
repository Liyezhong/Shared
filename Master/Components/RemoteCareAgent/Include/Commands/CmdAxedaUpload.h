/****************************************************************************/
/*! \file CmdAxedaUpload.h
 *
 *  \brief CmdAxedaUpload command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.09.2011
 *   $Author:  $ Y.Novak
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

#ifndef RCAGENTNAMESPACE_CMDAXEDAUPLOAD_H
#define RCAGENTNAMESPACE_CMDAXEDAUPLOAD_H

#include <Global/Include/GlobalDefines.h>
#include <Global/Include/Commands/Command.h>

namespace RCAgentNamespace {

/****************************************************************************/
/*!
 *  \brief  This class implements an Upload command for Axeda Agent.
 *
 ****************************************************************************/
class CmdAxedaUpload : public Global::Command
{
    /// streaming operators are friends
    friend QDataStream & operator << (QDataStream &, const CmdAxedaUpload &);
    friend QDataStream & operator >> (QDataStream &, CmdAxedaUpload &);

public:

    static QString NAME; ///< name of this command

    CmdAxedaUpload();
    CmdAxedaUpload(int timeout);
    CmdAxedaUpload(int timeout, const QString &filename);
    CmdAxedaUpload(const CmdAxedaUpload &rOther);
    const CmdAxedaUpload & operator = (const CmdAxedaUpload &rOther);
    ~CmdAxedaUpload();

    // inherited function
    virtual QString GetName() const;
    // own functions
    QString GetUploadFileName() const;

private:

    void CopyFrom(const CmdAxedaUpload &rOther);

private:

    QString    m_FileName;      ///< the name of the file to upload

}; // end class


/****************************************************************************/
/*!
 * \brief   Streaming operator to work with the QDataStream class.
 *     See QDataStream documentation: "Reading and writing other Qt classes"
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         Command to stream from.
 *
 * \return  Stream with the serialized command
 *
 ****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdAxedaUpload &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // stream the CmdAxedaUpload private members
    Stream << Cmd.m_FileName;
    return Stream;
}

/****************************************************************************/
/*!
 * \brief   Streaming operator to work with the QDataStream class.
 *     See QDataStream documentation: "Reading and writing other Qt classes"
 *
 * \param[in,out]   Stream      Stream to stream from.
 * \param[in,out]   Cmd         Command to stream into.
 *
 * \return  Stream
 *
 ****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdAxedaUpload &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // stream the CmdAxedaUpload private members
    Stream >> Cmd.m_FileName;
    return Stream;
}

} // end namespace

#endif
