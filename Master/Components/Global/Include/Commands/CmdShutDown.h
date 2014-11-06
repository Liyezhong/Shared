/****************************************************************************/
/*! \file Global/Include/Commands/CmdShutDown.h
 *
 *  \brief Definition file for class CmdShutDown
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-02-19
 *  $Author:    $ N.Kamath
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
#ifndef CMDSHUTDOWN_H
#define CMDSHUTDOWN_H

#include <Global/Include/Commands/Command.h>

namespace Global {

/****************************************************************************/
/**
 * \brief Command to inform main to initiate shutdown
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdShutDown : public Command {
private:
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \param[in]   rOther  Instance to copy from.
     */
    /****************************************************************************/
    void CopyFrom(const CmdShutDown &rOther);
    bool m_IsReboot;    ///< need to reboot or not
protected:
public:
    static QString  NAME;                   ///< Command name.
    /****************************************************************************/
    /**
     * \brief Constructor.
     */
    /****************************************************************************/
    CmdShutDown(bool Reboot = false);
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \param[in]   rOther  Instance to copy from.
     */
    /****************************************************************************/
    CmdShutDown(const CmdShutDown &rOther);
    /****************************************************************************/
    /**
     * \brief Assignment operator.
     *
     * \param[in]   rOther  Instance to copy from.
     * \return              Const reference to self.
     */
    /****************************************************************************/
    const CmdShutDown & operator = (const CmdShutDown &rOther);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdShutDown();
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
    /****************************************************************************/
    /**
     * \brief Set reboot
     * \param[in]   Reboot  set reboot flag.
     *
     * \return  void
     */
    /****************************************************************************/
    void SetReboot(bool Reboot){
        m_IsReboot = Reboot;
    }
    /****************************************************************************/
    /**
     * \brief Get reboot.
     *
     * \return  true - need reboot, false - shutdown only
     */
    /****************************************************************************/
    bool GetReboot() const{
        return m_IsReboot;
    }
}; // end class CmdShutDown
}// end of namespace global
#endif // CMDSHUTDOWN_H
