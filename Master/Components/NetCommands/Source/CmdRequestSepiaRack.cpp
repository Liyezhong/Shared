/****************************************************************************/
/*! \file CmdRequestSepiaRack.cpp
 *
 *  \brief CmdRequestSepiaRack command implementation.
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

#include <NetCommands/Include/CmdRequestSepiaRack.h>


namespace NetCommands {

QString CmdRequestSepiaRack::NAME = "NetCommands::CmdRequestSepiaRack";

/****************************************************************************/
/*!
 *  \brief   Constructor for sending
 *
 * \param[in]   Timeout                 Timeout for command.
 * \param[in]   EventStringsDataStream   Datastream of Event Strings Data.
 */
/****************************************************************************/
CmdRequestSepiaRack::CmdRequestSepiaRack(int Timeout) :
    Command(Timeout)
{

}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdRequestSepiaRack::CmdRequestSepiaRack() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdRequestSepiaRack::~CmdRequestSepiaRack()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdRequestSepiaRack::GetName() const
{
    return NAME;
}

} // end namespace NetCommmands
