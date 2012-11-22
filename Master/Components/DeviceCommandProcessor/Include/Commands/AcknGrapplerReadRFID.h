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

#ifndef DEVICECOMMANDPROCESSOR_ACKNGRAPPLERREADRFID_H
#define DEVICECOMMANDPROCESSOR_ACKNGRAPPLERREADRFID_H

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
class AcknGrapplerReadRFID : public Global::Acknowledge {
private:
    /****************************************************************************/
    AcknGrapplerReadRFID();                                                 ///< Not implemented.
    AcknGrapplerReadRFID(const AcknGrapplerReadRFID &);                     ///< Not implemented.
    const AcknGrapplerReadRFID & operator = (const AcknGrapplerReadRFID &); ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    DeviceControl::DevInstanceID_t m_DevInstanceID;   ///< Instance ID of the target device

    quint32 m_RFIDData;     ///< RFID data as read from the tag

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    AcknGrapplerReadRFID(DeviceControl::DevInstanceID_t DevInstanceID);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~AcknGrapplerReadRFID();
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
}; // end class AcknGrapplerReadRFID

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_ACKNGRAPPLERREADRFID_H

