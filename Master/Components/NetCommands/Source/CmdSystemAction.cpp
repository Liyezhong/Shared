/****************************************************************************/
/*! \file CmdSystemAction.cpp
 *
 *  \brief CmdSystemAction command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 5.11.2012
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

#include <NetCommands/Include/CmdSystemAction.h>



namespace NetCommands {

QString CmdSystemAction::NAME = "NetCommands::CmdSystemAction";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \iparam   Timeout               Timeout for command.
 * \iparam   theStrErrorInfo
 * \iparam   Ack
 * \iparam   NumberOfRetries
 * \iparam   UserRetryFlag
 * \iparam   EventKey              Unique Event Key.
 * \iparam   EventID               Event Id.
 * \iparam   EventSource
 */
/****************************************************************************/
CmdSystemAction::CmdSystemAction(int Timeout, const QString &theStrErrorInfo, bool  Ack, qint8 NumberOfRetries,
                                 bool UserRetryFlag, quint32 EventKey, quint32 EventID, Global::EventSourceType EventSource) :
    Command(Timeout),
    m_StrErrorInfo(theStrErrorInfo),
    m_Ack(Ack),
    m_MaxNumberofRetries (NumberOfRetries),
    m_UserRetry(UserRetryFlag),
    m_EventKey(EventKey),
    m_EventID(EventID),
    m_SourceComponent(EventSource),
    m_ActionString("")
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 */
/****************************************************************************/
CmdSystemAction::CmdSystemAction() :
    Command(15000),
    m_StrErrorInfo(""),
    m_Ack(false),
    m_MaxNumberofRetries (0),
    m_UserRetry(false),
    m_EventKey(0),
    m_EventID(0),
    m_SourceComponent(Global::EVENTSOURCE_NONE)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdSystemAction::~CmdSystemAction()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdSystemAction::GetName() const
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
QString CmdSystemAction::GetErrorInfo() const
{
    return m_StrErrorInfo;
}

} // end namespace NetCommands

