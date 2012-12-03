/****************************************************************************/
/*! \file CmdDataExport.cpp
 *
 *  \brief CmdDataExport command implementation.
 *         This command class is used for the data export (Service or user export)
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

#include "DataManager/Containers/ExportConfiguration/Commands/Include/CmdDataExport.h"

namespace MsgClasses {

QString CmdDataExport::NAME = "MsgClasses::CmdDataExport";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   DataStream  Type of export, which shall be performed
 */
/****************************************************************************/
CmdDataExport::CmdDataExport(int Timeout, const QDataStream &DataStream) :
    Command(Timeout)
{
    m_ExportByteArray = static_cast< QBuffer *>(DataStream.device())->data();
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdDataExport::CmdDataExport() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdDataExport::~CmdDataExport()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdDataExport::GetName() const
{
    return NAME;
}


} // end namespace ImportExport
