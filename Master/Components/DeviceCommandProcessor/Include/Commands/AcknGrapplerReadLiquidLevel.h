/****************************************************************************/
/*! \file AcknGrapplerReadRFID.h
 *
 *  \brief Definition file for AcknGrapplerReadRFID CmdGrapplerReadRFID.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-09-14
 *  $Author:    $ Norbert Wiedmann
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2011 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef DEVICECOMMANDPROCESSOR_ACKNGRAPPLERREADLIQUIDLEVEL_H
#define DEVICECOMMANDPROCESSOR_ACKNGRAPPLERREADLIQUIDLEVEL_H

#include <Global/Include/Commands/Acknowledge.h>
#include <DeviceControl/Include/Global/DeviceControlGlobal.h>

namespace DeviceCommandProcessor {

/****************************************************************************/
/**
 * \brief \todo comment and implement
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class AcknGrapplerReadLiquidLevel : public Global::Acknowledge {
private:
    /****************************************************************************/
    AcknGrapplerReadLiquidLevel();                                                 ///< Not implemented.
    AcknGrapplerReadLiquidLevel(const AcknGrapplerReadLiquidLevel &);                     ///< Not implemented.
    const AcknGrapplerReadLiquidLevel & operator = (const AcknGrapplerReadLiquidLevel &); ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    DeviceControl::DevInstanceID_t m_DevInstanceID;   ///< Instance ID of the target device

    quint16 m_LiquidLevel;     ///< LiquidLevel as read from function module

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    AcknGrapplerReadLiquidLevel(DeviceControl::DevInstanceID_t DevInstanceID);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~AcknGrapplerReadLiquidLevel();
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
}; // end class AcknGrapplerReadLiquidLevel

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_ACKNGRAPPLERREADRFID_H


