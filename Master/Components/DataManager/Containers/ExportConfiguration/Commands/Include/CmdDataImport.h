/****************************************************************************/
/*! \file Platform/Master/Components/DataManager/Containers/ExportConfiguration/Commands/Include/CmdDataImport.h
 *
 *  \brief CmdDataImport command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 17.03.2011
 *   $Author:  $ Y.Novak, Ramya GJ
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

#ifndef IMPORTEXPORT_CMDDATAIMPORT_H
#define IMPORTEXPORT_CMDDATAIMPORT_H

#include <Global/Include/Commands/Command.h>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdDataImport command.
 *
 */
/****************************************************************************/
class CmdDataImport : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdDataImport &);
    friend QDataStream & operator >> (QDataStream &, CmdDataImport &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdDataImport(int Timeout, const QDataStream &DataStream);
    CmdDataImport();
    ~CmdDataImport();
    virtual QString GetName() const;
    /****************************************************************************/
    /*!
     *  \brief   This function returns type of import, which shall be performed
     *
     *  \return  import type
     */
    /****************************************************************************/
    QByteArray GetCommandData() const { return m_ImportByteArray; }
private:
    //                                          ///< Not implemented.
    CmdDataImport(const CmdDataImport &);                       ///< Not implemented.
    /****************************************************************************/
    /**
         * \brief Not Implemented
         *
         * \return
         */
    /****************************************************************************/
    const CmdDataImport & operator = (const CmdDataImport &);   ///< Not implemented.
private:
    QByteArray     m_ImportByteArray;       ///< Type of export which shall be performed.
}; // end class CmdDataImport

/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \param[in,out]   Stream      Stream to stream into.
     * \param[in]       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdDataImport &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_ImportByteArray;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdDataImport &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_ImportByteArray;
    return Stream;
}

} // end namespace ImportExport

#endif // IMPORTEXPORT_CMDDATAIMPORT_H
