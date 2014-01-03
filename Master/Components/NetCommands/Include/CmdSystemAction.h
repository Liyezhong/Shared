/****************************************************************************/
/*! \file CmdSystemAction.h
 *
 *  \brief CmdSystemAction command defination.
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
#ifndef NETCOMMANDS_CMDSYSTEMACTION_H
#define NETCOMMANDS_CMDSYSTEMACTION_H

#include <Global/Include/Commands/Command.h>
#include <Global/Include/GlobalDefines.h>
#include <QStringList>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdSystemAction command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdSystemAction : public Global::Command {

private:
    CmdSystemAction(const CmdSystemAction &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdSystemAction & operator = (const CmdSystemAction &);   ///< Not implemented.
    QString m_StrErrorInfo;  //!< Error string Information
    bool m_Ack; ///< Acknowledgement
    qint8 m_MaxNumberofRetries; ///< Maximum  number of retries.
    bool m_UserRetry;  ///<  State of User Retry.
    qint8 m_CountRetries;  ///< Count of Retries.
    Global::ActionType m_Action; //!< Next Action , after retries are exhausted
    quint32  m_EventKey; //!< Unique Event Key
    quint32 m_EventID; //!< Event ID
    Global::EventSourceType m_SourceComponent; ///< Source Component Type.
    Global::tTranslatableStringList m_StringList;                ///< The translatable string.
public:
    static QString NAME;    ///< Command name.

    /****************************************************************************/
    /*!
     *  \brief   Constructor
     *
     * \iparam   Timeout               Timeout for command.
     * \iparam   StrErrorInfo          The new bath layout.
     * \iparam   Ack                   Acknowledgement
     * \iparam   NumberOfRetries       number of Entries
     * \iparam   UserRetryFlag
     * \iparam   EventKey              Unique Event Key.
     * \iparam   EventID               Event Id.
     *
     */
    /****************************************************************************/
    CmdSystemAction(int Timeout, const QString &StrErrorInfo, bool  Ack, qint8 NumberOfRetries = 0,
                    bool UserRetryFlag = false , quint32 EventKey = 0, quint32 EventID = 0);
    CmdSystemAction(int Timeout, const QString &StrErrorInfo, bool  Ack, qint8 NumberOfRetries = 0,
                    bool UserRetryFlag = false , quint32 EventKey = 0, quint32 EventID = 0,
                    Global::EventSourceType EventSource = Global::EVENTSOURCE_NONE);
    ~CmdSystemAction();
    virtual QString GetName() const;
    QString GetErrorInfo() const;

    /****************************************************************************/
    /*!
     *  \brief   This function gets the Acknowledgement State
     *
     *  \return   Acknowledgement
     */
    /****************************************************************************/
    inline bool GetAckState() const {
        return m_Ack;
    }
    /****************************************************************************/
    /*!
     *  \brief   This function gets the Source Component
     *
     *  \return   Source Component
     */
    /****************************************************************************/
    inline Global::EventSourceType GetSource() const {
        return m_SourceComponent;
    }
    /****************************************************************************/
    /*!
     *  \brief       This function sets the Source Component
     *
     *  \iparam   sourceComponent
     */
    /****************************************************************************/

    inline void SetSource(Global::EventSourceType sourceComponent) {
        m_SourceComponent = sourceComponent;
    }
    /****************************************************************************/
    /*!
     *  \brief   This function gets the StringList
     *
     *  \return   StringList
     */
    /****************************************************************************/
    inline Global::tTranslatableStringList GetStringList() const {
        return m_StringList;
    }
    /****************************************************************************/
    /*!
     *  \brief       This function sets the StringList
     *
     *  \iparam   stringList   String List
     */
    /****************************************************************************/
    inline void SetStringList(Global::tTranslatableStringList stringList) {
        m_StringList = stringList;
    }
    /****************************************************************************/
    /*!
     *  \brief   This function gets the Action Type
     *
     *  \return   Action Type
     */
    /****************************************************************************/
    inline Global::ActionType GetAction() const {
        return m_Action;
    }
    /****************************************************************************/
    /*!
     *  \brief       This function sets the Action Type
     *
     *  \iparam   ActionType   Type of Action
     */
    /****************************************************************************/
    inline void SetAction(Global::ActionType  ActionType) {
        m_Action = ActionType;
    }
    /****************************************************************************/
    /*!
     *  \brief   This function gets the Maximum Retry Attempts
     *
     *  \return   Maximun number of Retries
     */
    /****************************************************************************/
    inline bool GetMaxRetryAttempts() const {
        return m_MaxNumberofRetries;
    }
    /****************************************************************************/
    /*!
     *  \brief       This function sets the  Maximum Retry Attempts
     *
     *  \iparam   NumRetries   Maximun Number of Retries
     *  \iparam   UserRetry    State of User Retry
     */
    /****************************************************************************/
    inline void SetMaxRetryAttempts(int NumRetries = 0, bool UserRetry = false) {
        m_UserRetry = UserRetry;
        m_MaxNumberofRetries = NumRetries;
    }
    /****************************************************************************/
    /*!
     *  \brief   This function gets the Retry Count
     *
     *  \return   Count of Retries
     */
    /****************************************************************************/
    inline bool GetRetryCount() const {
        return m_CountRetries;
    }
    /****************************************************************************/
    /*!
     *  \brief       This function sets the Retry Count
     *
     *  \iparam   CntNumRetries   Count of number of Entries
     */
    /****************************************************************************/
    inline void SetRetryCount(int CntNumRetries) {
        m_CountRetries = CntNumRetries;
    }

    CmdSystemAction();
    /****************************************************************************/
    /*!
     *  \brief   This function gets the EventKey
     *
     *  \return   EventKey
     */
    /****************************************************************************/
    inline quint32 GetEventKey() const {
        return m_EventKey;
    }
    /****************************************************************************/
    /*!
     *  \brief       This function sets the EventKey
     *
     *  \iparam   EventKey
     */
    /****************************************************************************/
    inline void SetEventKey(quint32 EventKey) {
        m_EventKey = EventKey;
    }
    /****************************************************************************/
    /*!
     *  \brief   This function gets the EventId
     *
     *  \return   Eventd
     */
    /****************************************************************************/
    inline quint32 GetEventID() const {
        return m_EventID;
    }
    /****************************************************************************/
    /*!
     *  \brief       This function sets the EventId
     *
     *  \iparam   EventID
     */
    /****************************************************************************/
    inline void SetEventID(quint32 EventID) {
        m_EventID = EventID;
    }



}; // end class CmdSystemAction

}// end namespace NetCommands


#endif // NETCOMMANDS_CMDSYSTEMACTION_H
