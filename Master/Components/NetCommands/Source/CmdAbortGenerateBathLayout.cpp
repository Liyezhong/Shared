/****************************************************************************/
/*! \file CmdAbortGenerateBathLayout.cpp
 *
 *  \brief CmdAbortGenerateBathLayout command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 03.July.2013
 *   $Author:  $ Ningu
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

#include <NetCommands/Include/CmdAbortGenerateBathLayout.h>
#include <QBuffer>


namespace NetCommands {

QString CmdAbortGenerateBathLayout::NAME = "NetCommands::CmdAbortGenerateBathLayout";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \iparam   Timeout                          Timeout for command.
 */
/****************************************************************************/
CmdAbortGenerateBathLayout::CmdAbortGenerateBathLayout(int Timeout, GenBLGProcessFlags_t GenerateBLGProcessStatus) :
    Command(Timeout),
    m_GenerateBLGProcessStatus(GenerateBLGProcessStatus)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdAbortGenerateBathLayout::~CmdAbortGenerateBathLayout()
{
}

/****************************************************************************/
/*!
 *  \brief   This function returns BLG process aborted type
 *
 *  \return BLG process abort type
 */
/****************************************************************************/
GenBLGProcessFlags_t CmdAbortGenerateBathLayout::GetGenerateBLGAbortlType() const
{
    return m_GenerateBLGProcessStatus;
}


/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdAbortGenerateBathLayout::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 */
/****************************************************************************/
CmdAbortGenerateBathLayout::CmdAbortGenerateBathLayout() :
    Command(0)
{
}

} // end namespace NetCommands


