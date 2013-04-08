/****************************************************************************/
/*! \file Components/DataManager/Containers/UserSettings/Commands/Include/CmdRmsOnOff.h
 *
 *  \brief CmdRmsOnOff command definition.
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

#ifndef MSGCLASSES_CMDRMSONOFF_H
#define MSGCLASSES_CMDRMSONOFF_H

#include "Global/Include/Commands/Command.h"

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdRmsOnOff command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdRmsOnOff : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdRmsOnOff &);
    friend QDataStream & operator >> (QDataStream &, CmdRmsOnOff &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdRmsOnOff(int Timeout, bool RmsState);
    ~CmdRmsOnOff();
    virtual QString GetName() const;
    bool GetRmsState() const;
private:
    CmdRmsOnOff();                                          ///< Not implemented.
    CmdRmsOnOff(const CmdRmsOnOff &);                       ///< Not implemented.
    const CmdRmsOnOff & operator = (const CmdRmsOnOff &);   ///< Not implemented.
private:
    bool    m_RmsState;     ///< RMS state: true if on.
}; // end class CmdRmsOnOff

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdRmsOnOff &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_RmsState;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdRmsOnOff &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_RmsState;
    return Stream;
}

} // end namespace MsgClasses

#endif // MSGCLASSES_CMDRMSONOFF_H
