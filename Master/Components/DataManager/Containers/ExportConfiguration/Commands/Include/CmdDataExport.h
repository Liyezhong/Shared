/****************************************************************************/
/*! \file CmdDataExport.h
 *
 *  \brief CmdDataExport command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 17.03.2011
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

#ifndef IMPORTEXPORT_CMDDATAEXPORT_H
#define IMPORTEXPORT_CMDDATAEXPORT_H

#include <Global/Include/Commands/Command.h>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdDataExport command.
 *
 */
/****************************************************************************/
class CmdDataExport : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdDataExport &);
    friend QDataStream & operator >> (QDataStream &, CmdDataExport &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdDataExport(int Timeout, const QDataStream &DataStream);
    CmdDataExport();
    ~CmdDataExport();
    virtual QString GetName() const;
    /****************************************************************************/
    /*!
     *  \brief   This function returns type of export, which shall be performed
     *
     *  \return  export type
     */
    /****************************************************************************/
    QByteArray GetCommandData() { return m_ExportByteArray; }
private:
    CmdDataExport(const CmdDataExport &);                       ///< Not implemented.
    const CmdDataExport & operator = (const CmdDataExport &);   ///< Not implemented.
private:
    QByteArray      m_ExportByteArray;      ///< Type of export which shall be performed.
}; // end class CmdDataExport


/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \param[in,out]   Stream      Stream to stream into.
     * \param[in]       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdDataExport &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_ExportByteArray;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdDataExport &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_ExportByteArray;
    return Stream;
}

} // end namespace ImportExport

#endif // IMPORTEXPORT_CMDDATAEXPORT_H
