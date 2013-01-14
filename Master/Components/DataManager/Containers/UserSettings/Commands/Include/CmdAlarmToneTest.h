/****************************************************************************/
/*! \file CmdAlarmToneTest.h
 *
 *  \brief CmdAlarmToneTest command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.01.2013
 *   $Author:  $ S.Sahoo
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

#ifndef MSGCLASSES_CMDALARMTONETEST_H
#define MSGCLASSES_CMDALARMTONETEST_H

#include "Global/Include/Commands/Command.h"

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdAlarmToneTest command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdAlarmToneTest : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdAlarmToneTest &);
    friend QDataStream & operator >> (QDataStream &, CmdAlarmToneTest &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdAlarmToneTest(int Timeout, int Sound, int Volume, bool AlarmType);
    CmdAlarmToneTest();
    ~CmdAlarmToneTest();
    virtual QString GetName() const;
    int GetSound() const;
    int GetVolume() const;
    bool GetAlarmType() const;
private:

    CmdAlarmToneTest(const CmdAlarmToneTest &);                       ///< Not implemented.
    const CmdAlarmToneTest & operator = (const CmdAlarmToneTest &);   ///< Not implemented.
private:
    int    m_Sound;     ///< Sound Parameter.
    int    m_Volume;    ///< Volume Parameter.
    bool   m_AlarmType; ///< AlaramType Parameter.
   }; // end class CmdAlarmToneTest
/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdAlarmToneTest &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_Sound << Cmd.m_Volume << Cmd.m_AlarmType;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdAlarmToneTest &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_Sound >> Cmd.m_Volume >> Cmd.m_AlarmType;
    return Stream;
}

} // end namespace MsgClasses

#endif // MSGCLASSES_CMDALARMTONETEST_H
