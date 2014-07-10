/****************************************************************************/
/*! \file CmdRCNotifyAssetComplete.cpp
 *
 *  \brief CmdRCNotifyAssetComplete command implementation.
 *
 *   $Version: $ 1.0
 *   $Date:    $ 2014-03-13
 *   $Author:  $ Ramya GJ
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

#include <NetCommands/Include/CmdRCNotifyAssetComplete.h>

namespace NetCommands {

QString CmdRCNotifyAssetComplete::NAME = "NetCommands::CmdRCNotifyAssetComplete"; //!< Name of the command

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \iparam   Timeout         Timeout for command.
 *  \iparam   FilePath    asset archive file path
 */
/****************************************************************************/
CmdRCNotifyAssetComplete::CmdRCNotifyAssetComplete(const int& Timeout, const QString& FilePath) :
    Command(Timeout),
    m_FilePath(FilePath)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdRCNotifyAssetComplete::~CmdRCNotifyAssetComplete()
{
}

/****************************************************************************/
/*!
 *  \brief  Constructor to receive commands
 */
/****************************************************************************/
CmdRCNotifyAssetComplete::CmdRCNotifyAssetComplete() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdRCNotifyAssetComplete::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns asset archive file path
 *
 *  \return  QString - asset archive file path
 */
/****************************************************************************/
QString CmdRCNotifyAssetComplete::GetFilePath() const
{
    return m_FilePath;
}

}//End of namespace NetCommands


