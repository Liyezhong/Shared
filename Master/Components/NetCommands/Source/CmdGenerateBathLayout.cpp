/****************************************************************************/
/*! \file CmdGenerateBathLayout.cpp
 *
 *  \brief CmdGenerateBathLayout command implementation.
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

#include <NetCommands/Include/CmdGenerateBathLayout.h>

namespace NetCommands {

QString CmdGenerateBathLayout::NAME = "NetCommands::CmdGenerateBathLayout";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 */
/****************************************************************************/
CmdGenerateBathLayout::CmdGenerateBathLayout(int Timeout) :
    Command(Timeout)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdGenerateBathLayout::~CmdGenerateBathLayout()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdGenerateBathLayout::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 */
/****************************************************************************/
CmdGenerateBathLayout::CmdGenerateBathLayout() :
    Command(0)
{
}

} // end namespace NetCommands
