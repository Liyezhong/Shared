/****************************************************************************/
/*! \file Platform/Master/Components/DataManager/Containers/ExportConfiguration/Commands/Source/CmdDataImportFiles.cpp
 *
 *  \brief CmdDataImportFiles command implementation.
 *         This command class is used for the to import of the files
 *
 *   $Version: $ 0.1
 *   $Date:    $ 19.06.2013
 *   $Author:  $ Raju
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

#include "DataManager/Containers/ExportConfiguration/Commands/Include/CmdDataImportFiles.h"

namespace MsgClasses {

QString CmdDataImportFiles::NAME = "MsgClasses::CmdDataImportFiles";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   DataStream  Data stream to import.
 */
/****************************************************************************/
CmdDataImportFiles::CmdDataImportFiles(int Timeout, const QStringList FileList) :
    Command(Timeout), m_FileList(FileList)
{
    setBusyStateAllowed(false);
    setErrorStateAllowed(false);
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdDataImportFiles::CmdDataImportFiles() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdDataImportFiles::~CmdDataImportFiles()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdDataImportFiles::GetName() const
{
    return NAME;
}

} // end namespace NetCommands
