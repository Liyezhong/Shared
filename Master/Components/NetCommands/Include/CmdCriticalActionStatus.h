#ifndef CMDCRITICALACTIONSTATUS_H
#define CMDCRITICALACTIONSTATUS_H
#include <Global/Include/Commands/Command.h>

namespace NetCommands {
typedef enum {
    NO_CRITICAL_ACTION,
    CRITICAL_ACTION_IN_PROGRESS,
    CRITICAL_ACTION_COMPLETE
}CriticalActionStatus_t;
/****************************************************************************/
/**
 * \brief \todo comment and implement
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdCriticalActionStatus : public Global::Command {
private:
    /****************************************************************************/
    CmdCriticalActionStatus(const CmdCriticalActionStatus &);                     ///< Not implemented.
    const CmdCriticalActionStatus & operator = (const CmdCriticalActionStatus &); ///< Not implemented.

protected:
public:
    static QString  NAME;           ///< Command name.
    CriticalActionStatus_t m_CriticalActionStatus;
    /****************************************************************************/
    /*!
     * \brief   Constructor
     */
    /****************************************************************************/
    CmdCriticalActionStatus();
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    CmdCriticalActionStatus(int TimeOut, CriticalActionStatus_t CriticalActionStatus);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdCriticalActionStatus();
    /****************************************************************************/
    /**
     * \brief Get command name.
     *
     * \return  Command name.
     */
    /****************************************************************************/
    virtual QString GetName() const {
        return NAME;
    }
}; // end class CmdCriticalActionStatus

}

#endif // CMDCRITICALACTIONSTATUS_H
