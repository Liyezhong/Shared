#ifndef CMDSOFTSWITCHPRESSED_H
#define CMDSOFTSWITCHPRESSED_H
#include <Global/Include/Commands/Command.h>
#include <DeviceControl/Include/Global/DeviceControl.h>

namespace Global {

/****************************************************************************/
/**
 * \brief Command Created when softswitch is pressed
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdSoftSwitchPressed : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    CmdSoftSwitchPressed(const CmdSoftSwitchPressed &);                     ///< Not implemented.
    const CmdSoftSwitchPressed & operator = (const CmdSoftSwitchPressed &); ///< Not implemented.
protected:
public:
    static QString  NAME;       ///< Command name.
    bool   PressedAtStartUp;    //!< True - if softswitch Pressed at startup

    CmdSoftSwitchPressed();

    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdSoftSwitchPressed();
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
}; // end class CmdSoftSwitchPressed

} // end namespace Global
#endif // CMDSOFTSWITCHPRESSED_H
