/****************************************************************************/
/*! \file CmdLoaderRacksChanged.h
 *
 *  \brief Definition file for class CmdLoaderRacksChanged.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-09-22
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

#ifndef DEVICECOMMANDPROCESSOR_CMDLOADERRACKSCHANGED_H
#define DEVICECOMMANDPROCESSOR_CMDLOADERRACKSCHANGED_H

#include <Global/Include/Commands/Command.h>
#include <DeviceControl/Include/Global/DeviceControlGlobal.h>

namespace DeviceCommandProcessor {

/****************************************************************************/
/**
 * \brief \todo [MW] comment and implement
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdLoaderRacksChanged : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    CmdLoaderRacksChanged();                                                    ///< Not implemented.
    CmdLoaderRacksChanged(const CmdLoaderRacksChanged &);                       ///< Not implemented.
    const CmdLoaderRacksChanged & operator = (const CmdLoaderRacksChanged &);   ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    DeviceControl::DevInstanceID_t m_DevInstanceID;   ///< Instance ID of the target device

    quint32  m_RFIDData[5];  //RFID-data rack 1 - 5

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DevInstanceID       Instance ID of the concerned device.
     */
    /****************************************************************************/
    CmdLoaderRacksChanged(DeviceControl::DevInstanceID_t DevInstanceID);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdLoaderRacksChanged();
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
}; // end class CmdRacksInLoaderChanged

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_CMDLOADERRACKSCHANGED_H

