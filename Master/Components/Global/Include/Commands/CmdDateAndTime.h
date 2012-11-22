/****************************************************************************/
/*! \file Global/Include/Commands/CmdDateAndTime.h
 *
 *  \brief CmdDateAndTime command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 18.03.2011
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

#ifndef GLOBAL_CMDDATEANDTIME_H
#define GLOBAL_CMDDATEANDTIME_H

#include <Global/Include/Commands/Command.h>
#include <Global/Include/Commands/Acknowledge.h>

#include <QDateTime>
#include <QString>

namespace Global {


/****************************************************************************/
/*!
 *  \brief  This class implements a CmdDateAndTime command.
 *
 *      \todo
 */
/****************************************************************************/

class CmdDateAndTime : public Command {
friend QDataStream & operator << (QDataStream &, const CmdDateAndTime &);
friend QDataStream & operator >> (QDataStream &, CmdDateAndTime &);
public:
    static QString NAME;

    CmdDateAndTime();
    CmdDateAndTime(int Timeout, const QDateTime &dt);
    CmdDateAndTime(const CmdDateAndTime &rOther);
    ~CmdDateAndTime();
    const CmdDateAndTime & operator = (const CmdDateAndTime &rOther);

    virtual QString GetName() const;
    QDateTime GetDateTime() const;

private:

    void CopyFrom(const CmdDateAndTime &rOther);

private:

    QDateTime      m_DateAndTime;

}; // end class CmdDateAndTime

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdDateAndTime &Cmd) {
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_DateAndTime;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdDateAndTime &Cmd) {
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_DateAndTime;
    return Stream;
}

} // end namespace Threads

#endif
