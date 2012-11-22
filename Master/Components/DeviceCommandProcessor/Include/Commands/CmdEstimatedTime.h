/****************************************************************************/
/*! \file CmdEstimatedTime.h
 *
 *  \brief Definition file for class CmdEstimatedTime.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-06-13
 *  $Author:    $ Michael Thiel
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

#ifndef DEVICECOMMANDPROCESSOR_CMDESTIMATEDTIME_H
#define DEVICECOMMANDPROCESSOR_CMDESTIMATEDTIME_H

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
class CmdEstimatedTime : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    CmdEstimatedTime();                                                   ///< Not implemented.
    CmdEstimatedTime(const CmdEstimatedTime &);                     ///< Not implemented.
    const CmdEstimatedTime & operator = (const CmdEstimatedTime &); ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    DeviceControl::DevInstanceID_t m_DevInstanceID;   ///< Instance ID of the target device
    QString m_commandType;
    QByteArray m_configData;

    //Global::Command m_requestedCommand;
    //Global::CommandShPtr_t m_requestedCommand;

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    CmdEstimatedTime(DeviceControl::DevInstanceID_t DevInstanceID);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdEstimatedTime();
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
}; // end class CmdEstimatedTime

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_CMDESTIMATEDTIME_H
