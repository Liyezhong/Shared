/****************************************************************************/
/*! \file CmdDataImport.cpp
 *
 *  \brief CmdDataImport command implementation.
 *         This command class is used for the data import
 *
 *   $Version: $ 0.1
 *   $Date:    $ 18.03.2010
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

#include <QBuffer>

#include "TestStubDataManager/Containers/ExportConfiguration/Commands/Include/CmdDataImport.h"

namespace MsgClasses {

QString CmdDataImport::NAME = "MsgClasses::CmdDataImport";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   ImportType  Type of import which shall be performed.
 */
/****************************************************************************/
CmdDataImport::CmdDataImport(int Timeout, const QDataStream &DataStream) :
    Command(Timeout)
{
    m_ImportByteArray = static_cast< QBuffer *>(DataStream.device())->data();
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdDataImport::CmdDataImport() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdDataImport::~CmdDataImport()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdDataImport::GetName() const
{
    return NAME;
}

} // end namespace NetCommands
