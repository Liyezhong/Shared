/****************************************************************************/
/*! \file CmdStationsDefect.h
 *
 *  \brief Definition file for class CmdStationsDefect.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-10-31
 *  $Author:    $ N.Kamath
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2011 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/
#ifndef CMDSTATIONSDEFECT_H
#define CMDSTATIONSDEFECT_H

#include <QStringList>
#include <Global/Include/Commands/Command.h>

namespace NetCommands {
/****************************************************************************/
/*!
 *  \brief  This class implements a StationsDefect command.This command is
 *          Sent from Main to the GUI when Stations are defective.
 */
/****************************************************************************/
class CmdStationsDefect : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdStationsDefect &);
    friend QDataStream & operator >> (QDataStream &, CmdStationsDefect &);
public:
    static QString NAME;    ///< Command name.
    CmdStationsDefect(int TimeOut,
                      const QStringList StationsList, bool IsDefective);
    CmdStationsDefect();
    ~CmdStationsDefect();
    virtual QString GetName() const;
    QStringList GetStationList() const;
    bool GetListDefectiveFlag() const;

private:
    CmdStationsDefect(const CmdStationsDefect &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdStationsDefect & operator = (const CmdStationsDefect &);   ///< Not implemented.
    QStringList m_StationList;   ///< List of defective/corrected Stations
    bool m_IsDefective;         ///<  List differentaitor true = Defective staions list
                                ///< false = Corrected stations list
};


/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdStationsDefect &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_StationList << Cmd.m_IsDefective;
    return Stream;
}

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream from.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdStationsDefect &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_StationList >> Cmd.m_IsDefective;;
    return Stream;
}

} // End of namespace NetCommands
#endif // CMDSTATIONSDEFECT_H
