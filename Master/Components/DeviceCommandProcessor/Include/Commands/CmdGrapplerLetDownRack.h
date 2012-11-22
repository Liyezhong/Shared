/****************************************************************************/
/*! \file CmdGrapplerLetDownRack.h
 *
 *  \brief Definition file for class CmdGrapplerLetDownRack.
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

#ifndef DEVICECOMMANDPROCESSOR_CMDGRAPPLERLETDOWNRACK_H
#define DEVICECOMMANDPROCESSOR_CMDGRAPPLERLETDOWNRACK_H

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
class CmdGrapplerLetDownRack : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    CmdGrapplerLetDownRack();                                                   ///< Not implemented.
    CmdGrapplerLetDownRack(const CmdGrapplerLetDownRack &);                     ///< Not implemented.
    const CmdGrapplerLetDownRack & operator = (const CmdGrapplerLetDownRack &); ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    DeviceControl::DevInstanceID_t m_DevInstanceID;   ///< Instance ID of the target device
    bool m_SlowMode;    ///< Slow or fast movement
    QString m_Station;
    qint64 m_Rack;

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    CmdGrapplerLetDownRack(DeviceControl::DevInstanceID_t DevInstanceID);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdGrapplerLetDownRack();
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
}; // end class CmdGrapplerLetDownRack

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_CMDGRAPPLERLETDOWNRACK_H
