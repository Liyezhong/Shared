/****************************************************************************/
/*! \file  DataManager/Containers/UserSettings/Commands/Source/CmdAlarmToneTest.cpp
 *
 *  \brief CmdAlarmToneTest command implementation.
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

#include "DataManager/Containers/UserSettings/Commands/Include/CmdAlarmToneTest.h"

namespace MsgClasses {

QString CmdAlarmToneTest::NAME = "MsgClasses::CmdAlarmToneTest";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   Sound       Sound for Alarm.
 * \param[in]   Volume      Volume for Alarm.
 * \param[in]   AlarmType   Type for Alarm.True for NOTE.False for ERROR.
 */
/****************************************************************************/
CmdAlarmToneTest::CmdAlarmToneTest(int Timeout, quint8 Sound, quint8 Volume, bool AlarmType) :
                                   Command(Timeout),
                                   m_Sound(Sound),
                                   m_Volume(Volume),
                                   m_AlarmType(AlarmType)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdAlarmToneTest::~CmdAlarmToneTest()
{
}

CmdAlarmToneTest::CmdAlarmToneTest():Command(0)
{
    m_Sound = 0;
    m_Volume = 0;
    m_AlarmType = false;
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdAlarmToneTest::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns requested Sound
 *
 *  \return  Sound
 */
/****************************************************************************/
quint8 CmdAlarmToneTest::GetSound() const
{
    return m_Sound;
}
/****************************************************************************/
/*!
 *  \brief   This function returns requested Volume
 *
 *  \return  Volume
 */
/****************************************************************************/
quint8 CmdAlarmToneTest::GetVolume() const
{
    return m_Volume;
}
/****************************************************************************/
/*!
 *  \brief   This function returns Type of Alarm
 *
 *  \return  AlarmType
 */
/****************************************************************************/
bool CmdAlarmToneTest::GetAlarmType() const
{
    return m_AlarmType;
}

} // end namespace MsgClasses
