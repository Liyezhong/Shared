/****************************************************************************/
/*! \file CmdRequestSepiaParameterSet.cpp
 *
 *  \brief CmdRequestSepiaParameterSet command implementation.
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

#include <NetCommands/Include/CmdRequestSepiaParameterSet.h>


namespace NetCommands {

QString CmdRequestSepiaParameterSet::NAME = "NetCommands::CmdRequestSepiaParameterSet";

/****************************************************************************/
/*!
 *  \brief   Constructor for sending
 *
 * \param[in]   Timeout                 Timeout for command.
 * \param[in]   EventStringsDataStream   Datastream of Event Strings Data.
 */
/****************************************************************************/
CmdRequestSepiaParameterSet::CmdRequestSepiaParameterSet(int Timeout, quint32 UniqueRFId) :
    Command(Timeout), m_UniqueRFId(UniqueRFId)
{

}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdRequestSepiaParameterSet::CmdRequestSepiaParameterSet() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdRequestSepiaParameterSet::~CmdRequestSepiaParameterSet()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdRequestSepiaParameterSet::GetName() const
{
    return NAME;
}

} // end namespace NetCommmands

