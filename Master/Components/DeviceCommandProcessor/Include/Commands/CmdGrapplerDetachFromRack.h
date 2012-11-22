/****************************************************************************/
/*! \file CmdGrapplerDetachFromRack.h
 *
 *  \brief Definition file for class CmdGrapplerDetachFromRack.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-08-31
 *  $Author:    $ Norbert Wiedmann
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

#ifndef DEVICECOMMANDPROCESSOR_CMDGRAPPLERDETACHFROMRACK_H
#define DEVICECOMMANDPROCESSOR_CMDGRAPPLERDETACHFROMRACK_H

#include <Global/Include/Commands/Command.h>
#include <DeviceControl/Include/Global/DeviceControlGlobal.h>

namespace DeviceCommandProcessor {

/****************************************************************************/
/**
 * \brief \todo comment and implement
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdGrapplerDetachFromRack : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    CmdGrapplerDetachFromRack();                                                        ///< Not implemented.
    CmdGrapplerDetachFromRack(const CmdGrapplerDetachFromRack &);                       ///< Not implemented.
    const CmdGrapplerDetachFromRack & operator = (const CmdGrapplerDetachFromRack &);   ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    DeviceControl::DevInstanceID_t m_DevInstanceID;   ///< Instance ID of the target device

    QString m_Station;  ///< only for simulation
    qint64 m_Rack;      ///< only for simulation

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    CmdGrapplerDetachFromRack(DeviceControl::DevInstanceID_t DevInstanceID);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdGrapplerDetachFromRack();
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
}; // end class CmdGrapplerDetachFromRack

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_CMDGRAPPLERDETACHFROMRACK_H
