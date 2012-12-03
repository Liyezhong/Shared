/****************************************************************************/
/*! \file CmdEventStrings.h
 *
 *  \brief CmdEventStrings command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 10.08.2012
 *   $Author:  $ N.Kamath
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

#ifndef CMDEVENTSTRINGS_H
#define CMDEVENTSTRINGS_H

#include <Global/Include/Commands/Command.h>
#include <QByteArray>
#include <QBuffer>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdEventStrings command.
 *
 * \todo implement
 */
/****************************************************************************/

class CmdEventStrings : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdEventStrings &);
    friend QDataStream & operator >> (QDataStream &, CmdEventStrings &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdEventStrings(int Timeout, const QDataStream &EventStringsDataStream);
    CmdEventStrings();
    ~CmdEventStrings();
    virtual QString GetName() const;
    /****************************************************************************/
    /**
     * \brief Get the Event String Data
     * \return Byte array.
     */
    /****************************************************************************/
    QByteArray const & GetEventsStringsData() const { return m_EventStringsByteArray;}

private:
    CmdEventStrings(const CmdEventStrings &);                     ///< Not implemented.
    const CmdEventStrings & operator = (const CmdEventStrings &); ///< Not implemented.

    QByteArray m_EventStringsByteArray; //!< Byte array for Event Strings
}; // end class CmdEventStrings

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdEventStrings &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_EventStringsByteArray;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdEventStrings &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_EventStringsByteArray;
    return Stream;
}

} // end namespace NetCommands


#endif // CMDEVENTSTRINGS_H
