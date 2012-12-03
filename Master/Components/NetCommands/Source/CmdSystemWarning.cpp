/****************************************************************************/
/*! \file CmdSystemWarning.cpp
 *
 *  \brief CmdSystemWarning command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 13.10.2012
 *   $Author:  $ Aparna
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

//To be enhanced after further discussions with component owners

#include <NetCommands/Include/CmdSystemWarning.h>

namespace NetCommands {

QString CmdSystemWarning::NAME = "NetCommands::CmdSystemWarning";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   theStrErrorInfo  The new bath layout.
 */
/****************************************************************************/
CmdSystemWarning::CmdSystemWarning(int Timeout, const QString &StrWarningInfo) :
    Command(Timeout),
    m_StrWarningInfo(StrWarningInfo)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 */
/****************************************************************************/
CmdSystemWarning::CmdSystemWarning() :
    Command(5000)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdSystemWarning::~CmdSystemWarning()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdSystemWarning::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns the bath layout.
 *
 */
/****************************************************************************/
QString CmdSystemWarning::GetWarningInfo() const
{
    return m_StrWarningInfo;
}

} // end namespace NetCommands

