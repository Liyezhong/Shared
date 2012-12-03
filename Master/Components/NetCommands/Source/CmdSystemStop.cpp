
/****************************************************************************/
/*! \file CmdSystemStop.cpp
 *
 *  \brief CmdSystemStop command implementation.
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

#include <NetCommands/Include/CmdSystemStop.h>

namespace NetCommands {

QString CmdSystemStop::NAME = "NetCommands::CmdSystemStop";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   theStrErrorInfo  The new bath layout.
 */
/****************************************************************************/
CmdSystemStop::CmdSystemStop(int Timeout, const QString &theStrErrorInfo) :
    Command(Timeout),
    m_StrErrorInfo(theStrErrorInfo)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   Bathlayout  The new bath layout.
 */
/****************************************************************************/
CmdSystemStop::CmdSystemStop() :
    Command(5000)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdSystemStop::~CmdSystemStop()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdSystemStop::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns the bath layout.
 *
 *  \return  Bath layout
 */
/****************************************************************************/
QString CmdSystemStop::GetErrorInfo() const
{
    return m_StrErrorInfo;
}

} // end namespace NetCommands
