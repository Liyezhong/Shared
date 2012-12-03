#ifndef CMDEXTERNALPROCESSSTATE_H
#define CMDEXTERNALPROCESSSTATE_H

#include <Global/Include/Commands/Command.h>

namespace NetCommands {
enum ProcessType_t{
    COLORADO_GUI_PROCESS,
    SEPIA_MAIN_PROCESS
};

/****************************************************************************/
/**
 * \brief \todo comment and implement
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdExternalProcessState : public Global::Command {
private:
    /****************************************************************************/
    CmdExternalProcessState(const CmdExternalProcessState &);                     ///< Not implemented.
    const CmdExternalProcessState & operator = (const CmdExternalProcessState &); ///< Not implemented.

protected:
public:
    static QString  NAME;           ///< Command name.

    ProcessType_t m_ProcessType;    ///< External Process type
    bool m_Connected;               ///< True - if external process connected, else false.

    /****************************************************************************/
    /*!
     * \brief   Constructor
     */
    /****************************************************************************/
    CmdExternalProcessState();
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    CmdExternalProcessState(int TimeOut, ProcessType_t ProcessType, bool Connected);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdExternalProcessState();
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
}; // end class CmdExternalProcessState

} // end namespace NetCommands

#endif // CMDEXTERNALPROCESSSTATE_H
