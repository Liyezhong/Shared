/****************************************************************************/
/*! \file AcknEstimatedTime.h
 *
 *  \brief Definition file for AcknEstimatedTime.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-06-18
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

#ifndef DEVICECOMMANDPROCESSOR_ACKNESTIMATEDTIME_H
#define DEVICECOMMANDPROCESSOR_ACKNESTIMATEDTIME_H

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
class AcknEstimatedTime : public Global::Acknowledge {
private:
    /****************************************************************************/
    AcknEstimatedTime();                                                 ///< Not implemented.
    AcknEstimatedTime(const AcknEstimatedTime &);                     ///< Not implemented.
    const AcknEstimatedTime & operator = (const AcknEstimatedTime &); ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    DeviceControl::DevInstanceID_t m_DevInstanceID;   ///< Instance ID of the target device

    QByteArray m_duration;
    QByteArray m_configData;

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    AcknEstimatedTime(DeviceControl::DevInstanceID_t DevInstanceID);

    AcknEstimatedTime(QByteArray configData, QByteArray duration);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~AcknEstimatedTime();
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
}; // end class AcknEstimatedTime

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_ACKNESTIMATEDTIME_H

