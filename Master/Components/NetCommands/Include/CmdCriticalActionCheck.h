#ifndef CMDCRITICALACTIONCHECK_H
#define CMDCRITICALACTIONCHECK_H
#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/**
 * \brief \todo comment and implement
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdCriticalActionCheck : public Global::Command {
private:
    /****************************************************************************/
    CmdCriticalActionCheck(const CmdCriticalActionCheck &);                     ///< Not implemented.
    const CmdCriticalActionCheck & operator = (const CmdCriticalActionCheck &); ///< Not implemented.

protected:
public:
    static QString  NAME;           ///< Command name.

    /****************************************************************************/
    /*!
     * \brief   Constructor
     */
    /****************************************************************************/
    CmdCriticalActionCheck();
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    CmdCriticalActionCheck(int TimeOut);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdCriticalActionCheck();
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
}; // end class CmdCriticalActionCheck
}
#endif // CMDCRITICALACTIONCHECK_H
