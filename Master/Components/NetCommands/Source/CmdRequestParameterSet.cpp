/****************************************************************************/
/*! \file CmdRequestParameterSet.cpp
 *
 *  \brief CmdRequestParameterSet command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-01-02
 *   $Author:  $ C.Adaragunchi
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

#include <NetCommands/Include/CmdRequestParameterSet.h>


namespace NetCommands {

QString CmdRequestParameterSet::NAME = "NetCommands::CmdRequestParameterSet";

/****************************************************************************/
/*!
 *  \brief   Constructor for sending
 *
 * \iparam   Timeout                 Timeout for command.
 * \iparam   UniqueRFId              Unique RFId of the Rack.
 */
/****************************************************************************/
CmdRequestParameterSet::CmdRequestParameterSet(int Timeout, quint32 UniqueRFId) :
    Command(Timeout), m_UniqueRFId(UniqueRFId)
{

}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdRequestParameterSet::CmdRequestParameterSet() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdRequestParameterSet::~CmdRequestParameterSet()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdRequestParameterSet::GetName() const
{
    return NAME;
}

} // end namespace NetCommmands

