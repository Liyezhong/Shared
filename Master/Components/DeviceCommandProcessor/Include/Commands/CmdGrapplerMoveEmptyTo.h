/****************************************************************************/
/*! \file CmdGrapplerMoveEmptyTo.h
 *
 *  \brief Definition file for class CmdGrapplerMoveEmptyTo.
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

#ifndef DEVICECOMMANDPROCESSOR_CMDGRAPPLERMOVEEMPTYTO_H
#define DEVICECOMMANDPROCESSOR_CMDGRAPPLERMOVEEMPTYTO_H

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
class CmdGrapplerMoveEmptyTo : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    CmdGrapplerMoveEmptyTo();                                                   ///< Not implemented.
    CmdGrapplerMoveEmptyTo(const CmdGrapplerMoveEmptyTo &);                     ///< Not implemented.
    const CmdGrapplerMoveEmptyTo & operator = (const CmdGrapplerMoveEmptyTo &); ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    DeviceControl::DevInstanceID_t m_DevInstanceID;   ///< Instance ID of the target device

    qint16 m_Row;     ///< target row
    qint16 m_Column;  ///< target column

    qint16 m_SourceRow; ///< source row
    qint16 m_SourceColumn; ///< source column

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    CmdGrapplerMoveEmptyTo(DeviceControl::DevInstanceID_t DevInstanceID);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdGrapplerMoveEmptyTo();
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
}; // end class CmdGrapplerMoveEmptyTo

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_CMDGRAPPLERMOVEEMPTYTO_H
