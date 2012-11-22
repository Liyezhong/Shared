/****************************************************************************/
/*! \file CmdGrapplerTransportRackTo.h
 *
 *  \brief Definition file for class CmdGrapplerTransportRackTo.
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

#ifndef DEVICECOMMANDPROCESSOR_CMDGRAPPLERTRANSPORTRACKTO_H
#define DEVICECOMMANDPROCESSOR_CMDGRAPPLERTRANSPORTRACKTO_H

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
class CmdGrapplerTransportRackTo : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    CmdGrapplerTransportRackTo();                                                       ///< Not implemented.
    CmdGrapplerTransportRackTo(const CmdGrapplerTransportRackTo &);                     ///< Not implemented.
    const CmdGrapplerTransportRackTo & operator = (const CmdGrapplerTransportRackTo &); ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    DeviceControl::DevInstanceID_t m_DevInstanceID;   ///< Instance ID of the target device

    qint16 m_Row;     ///< target row
    qint16 m_Column;  ///< target column
    qint64 m_Rack;    ///< rack Id

    qint16 m_SourceRow; ///< source row
    qint16 m_SourceColumn; ///< source column

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    CmdGrapplerTransportRackTo(DeviceControl::DevInstanceID_t DevInstanceID);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdGrapplerTransportRackTo();
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
}; // end class CmdGrapplerTransportRackTo

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_CMDGRAPPLERTRANSPORTRACKTO_H
