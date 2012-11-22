/****************************************************************************/
/*! \file CmdGrapplerReadLiquidLevel.h
 *
 *  \brief Definition file for class CmdGrapplerReadLiquidLevel.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-09-15
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

#ifndef DEVICECOMMANDPROCESSOR_CMDGRAPPLERREADLIQUIDLEVEL_H
#define DEVICECOMMANDPROCESSOR_CMDGRAPPLERREADLIQUIDLEVEL_H

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
class CmdGrapplerReadLiquidLevel : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    CmdGrapplerReadLiquidLevel();                                                   ///< Not implemented.
    CmdGrapplerReadLiquidLevel(const CmdGrapplerReadLiquidLevel &);                     ///< Not implemented.
    const CmdGrapplerReadLiquidLevel & operator = (const CmdGrapplerReadLiquidLevel &); ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    DeviceControl::DevInstanceID_t m_DevInstanceID;   ///< Instance ID of the target device

    qint16 m_Row;     ///< target row
    qint16 m_Column;  ///< target column

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    CmdGrapplerReadLiquidLevel(DeviceControl::DevInstanceID_t DevInstanceID);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdGrapplerReadLiquidLevel();
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
}; // end class CmdGrapplerReadLiquidLevel

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_CMDGRAPPLERREADLIQUIDLEVEL_H

