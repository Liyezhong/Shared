/****************************************************************************/
/*! \file CmdDataUpdate.cpp
 *
 *  \brief CmdDataUpdate command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-11-29
 *   $Author:  $ Michael Thiel
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

#include <QBuffer>
#include "NetCommands/Include/CmdDataUpdate.h"

namespace NetCommands {

QString CmdDataUpdate::NAME = "NetCommands::CmdDataUpdate";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \iparam   Timeout                  Timeout for command.
 * \iparam   DataUpdateDataStream
 * \iparam   CmdType                  Type of Command.
 * \iparam   DlgStatus   Dialog display status
 */
/****************************************************************************/
CmdDataUpdate::CmdDataUpdate(int Timeout, const QDataStream &DataUpdateDataStream, DataUpdateType CmdType, bool DlgStatus)
    : Command(Timeout)
//    , m_RelatedClass(relatedClass)
    , m_CmdType(CmdType)
    ,m_DlgStatus(DlgStatus)
{
    m_DataUpdateByteArray = static_cast< QBuffer *>(DataUpdateDataStream.device())->data();
}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdDataUpdate::CmdDataUpdate() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdDataUpdate::~CmdDataUpdate()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdDataUpdate::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns the dialog displaying status
 *
 *  \return  true if the dialog needs to be displayed
 */
/****************************************************************************/
bool CmdDataUpdate::GetDialogStatus() const
{
    return m_DlgStatus;
}

} // end namespace MsgClasses
