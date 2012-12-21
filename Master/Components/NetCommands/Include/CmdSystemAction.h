#ifndef NETCOMMANDS_CMDSYSTEMACTION_H
#define NETCOMMANDS_CMDSYSTEMACTION_H

#include <Global/Include/Commands/Command.h>
#include <Global/Include/GlobalDefines.h>
#include <QStringList>

namespace NetCommands {

class CmdSystemAction : public Global::Command {

private:
    CmdSystemAction(const CmdSystemAction &);                       ///< Not implemented.
    const CmdSystemAction & operator = (const CmdSystemAction &);   ///< Not implemented.
    QString m_StrErrorInfo;
    bool m_Ack;
    qint8 m_MaxNumberofRetries;
    bool m_UserRetry;
    qint8 m_CountRetires;
    Global::ActionType m_Action; /// < Next Action , after retries are exhausted
    quint32  m_EventKey; //!< Unique Event Key
    quint32 m_EventID; //!< Event ID
    Global::EventSourceType m_SourceComponent;
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdSystemAction(int Timeout, const QString &StrErrorInfo, bool  Ack, qint8 NumberOfRetries = 0, bool UserRetryFlag = false , quint32 EventKey = 0, quint32 EventID = 0);
    CmdSystemAction(int Timeout, const QString &StrErrorInfo, bool  Ack, qint8 NumberOfRetries = 0, bool UserRetryFlag = false , quint32 EventKey = 0, quint32 EventID = 0, Global::EventSourceType EventSource = Global::EVENTSOURCE_NONE);
    ~CmdSystemAction();
    virtual QString GetName() const;
    QString GetErrorInfo() const;
    inline bool GetAckState() const {
        return m_Ack;
    }
    inline Global::EventSourceType GetSource() const {
        return m_SourceComponent;
    }

    inline void SetSource(Global::EventSourceType  sourceComponent) {
        m_SourceComponent = sourceComponent;
    }



    inline Global::ActionType GetAction() const {
        return m_Action;
    }

    inline void SetAction(Global::ActionType  ActionType) {
        m_Action = ActionType;
    }

    inline bool GetMaxRetryAttempts() const {
        return m_MaxNumberofRetries;
    }

    inline void SetMaxRetryAttempts(int NumRetries = 0, bool UserRetry = false) {
        m_UserRetry = UserRetry;
        m_MaxNumberofRetries = NumRetries;
    }

    inline bool GetRetryCount() const {
        return m_CountRetires;
    }

    inline void SetRetryCount(int CntNumRetries) {
        m_CountRetires = CntNumRetries;
    }

    CmdSystemAction();

    inline quint32 GetEventKey() const {
        return m_EventKey;
    }

    inline void SetEventKey(quint32 EventKey) {
        m_EventKey = EventKey;
    }

    inline quint32 GetEventID() const {
        return m_EventID;
    }

    inline void SetEventID(quint32 EventID) {
        m_EventID = EventID;
    }



}; // end class CmdSystemAction

}// end namespace NetCommands


#endif // NETCOMMANDS_CMDSYSTEMACTION_H
