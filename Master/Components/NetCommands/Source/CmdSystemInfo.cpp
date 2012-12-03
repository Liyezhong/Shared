
/****************************************************************************/
/*! \file CmdSystemInfo.cpp
 *
 *  \brief CmdSystemInfo command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 14.09.2012
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

#include <NetCommands/Include/CmdSystemInfo.h>

namespace NetCommands {

QString CmdSystemInfo::NAME = "NetCommands::CmdSystemInfo";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   theStrErrorInfo  The new bath layout.
 */
/****************************************************************************/
CmdSystemInfo::CmdSystemInfo(int Timeout, const QString &theStrErrorInfo) :
    Command(Timeout),
    m_StrErrorInfo(theStrErrorInfo)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 */
/****************************************************************************/
CmdSystemInfo::CmdSystemInfo() :
    Command(5000)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdSystemInfo::~CmdSystemInfo()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdSystemInfo::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns the bath layout.
 *
 */
/****************************************************************************/
QString CmdSystemInfo::GetErrorInfo() const
{
    return m_StrErrorInfo;
}

} // end namespace NetCommands

