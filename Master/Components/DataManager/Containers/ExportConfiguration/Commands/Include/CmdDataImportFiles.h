/****************************************************************************/
/*! \file Platform/Master/Components/DataManager/Containers/ExportConfiguration/Commands/Include/CmdDataImportFiles.h
 *
 *  \brief CmdDataImportFiles command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 19.06.2013
 *   $Author:  $ Raju
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

#ifndef MSGCLASSES_CMDDATAIMPORTFILES_H
#define MSGCLASSES_CMDDATAIMPORTFILES_H

#include <Global/Include/Commands/Command.h>
#include <QStringList>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdDataImportFiles command.
 *
 */
/****************************************************************************/
class CmdDataImportFiles : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdDataImportFiles &);
    friend QDataStream & operator >> (QDataStream &, CmdDataImportFiles &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdDataImportFiles(int Timeout, const QStringList FileList);
    CmdDataImportFiles();
    ~CmdDataImportFiles();
    virtual QString GetName() const;
    /****************************************************************************/
    /*!
     *  \brief   This function returns type of import, which shall be performed
     *
     *  \return  import type
     */
    /****************************************************************************/
    QStringList GetCommandData() const { return m_FileList; }
    //Q_DISABLE_COPY
private:
    //                                          ///< Not implemented.
    CmdDataImportFiles(const CmdDataImportFiles &);                       ///< Not implemented.
    /****************************************************************************/
    /**
     * \brief Not Implemented
     *
     * \return
     */
    /****************************************************************************/
    const CmdDataImportFiles & operator = (const CmdDataImportFiles &);   ///< Not implemented.
private:
    QStringList     m_FileList;       ///< List of files.
}; // end class CmdDataImportFiles

/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \param[in,out]   Stream      Stream to stream into.
     * \param[in]       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdDataImportFiles &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_FileList;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdDataImportFiles &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_FileList;
    return Stream;
}

} // end namespace MsgClasses

#endif // MSGCLASSES_CMDDATAIMPORTFILES_H
