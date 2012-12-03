
/****************************************************************************/
/*! \file CmdSystemError.cpp
 *
 *  \brief CmdSystemError command implementation.
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

#include <NetCommands/Include/CmdSystemError.h>

namespace NetCommands {

QString CmdSystemError::NAME = "NetCommands::CmdSystemError";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   theStrErrorInfo  The new bath layout.
 */
/****************************************************************************/
CmdSystemError::CmdSystemError(int Timeout, const QString &theStrErrorInfo, bool  Ack ) :
    Command(Timeout),
    m_StrErrorInfo(theStrErrorInfo),
    m_Ack(Ack)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 */
/****************************************************************************/
CmdSystemError::CmdSystemError() :
    Command(5000)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdSystemError::~CmdSystemError()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdSystemError::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns the bath layout.
 *
 */
/****************************************************************************/
QString CmdSystemError::GetErrorInfo() const
{
    return m_StrErrorInfo;
}

} // end namespace NetCommands
