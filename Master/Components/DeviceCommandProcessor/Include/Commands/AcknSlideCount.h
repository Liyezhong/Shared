/****************************************************************************/
/*! \file AcknSlideCount.h
 *
 *  \brief Definition file for AcknSlideCount.
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

#ifndef DEVICECOMMANDPROCESSOR_ACKNSLIDECOUNT_H
#define DEVICECOMMANDPROCESSOR_ACKNSLIDECOUNT_H

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
class AcknSlideCount : public Global::Acknowledge {
private:
    /****************************************************************************/
    AcknSlideCount();                                                 ///< Not implemented.
    AcknSlideCount(const AcknSlideCount &);                     ///< Not implemented.
    const AcknSlideCount & operator = (const AcknSlideCount &); ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    DeviceControl::DevInstanceID_t m_DevInstanceID;   ///< Instance ID of the target device

    qint8 m_slideNumber;
    quint32 m_rackId;

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    AcknSlideCount(DeviceControl::DevInstanceID_t DevInstanceID);

    AcknSlideCount(quint32 rackId, qint8 slideNumber);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~AcknSlideCount();
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
}; // end class AcknSlideCount

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_ACKNSLIDECOUNT_H

