/****************************************************************************/
/*! \file DeviceCommandProcessorThreadController.h
 *
 *  \brief Definition file for class DeviceCommandProcessorThreadController.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-08-22
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

#ifndef DEVICECOMMANDPROCESSOR_DEVICECOMMANDPROCESSORTHREADCONTROLLER_H
#define DEVICECOMMANDPROCESSOR_DEVICECOMMANDPROCESSORTHREADCONTROLLER_H

#include <Threads/Include/ThreadController.h>
#include <Global/Include/Commands/AckOKNOK.h>

//Adaption to device processing
#include "DeviceControl/Include/Global/DeviceControl.h"
//#include "DeviceControl/Include/Interface/IDeviceProcessing.h"

//#include "HimalayaDataContainer/Include/DataManager.h"

#include <QMutex>

using namespace DeviceControl;

// DestroyObjects is the cleanup function for class DeviceCommandProcessorThreadController,
// so we inform lint about this.
//lint -sem(DeviceCommandProcessor::DeviceCommandProcessorThreadController::DestroyObjects, cleanup)

namespace DeviceControl {

    class IDeviceProcessing;
}

namespace DeviceCommandProcessor {

class CmdDeviceProcessingInit;
class CmdDeviceProcessingCleanup;


#define CMD_REF_CNT  25   ///< array size of the command date buffer

/****************************************************************************/
/**
 * \brief This class encapsulates the thread controller for the device command processing.
 *
 * This class encapsulates the thread controller for the device command
 * processing. It communicates with the real hardware.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class DeviceCommandProcessorThreadController : public Threads::ThreadController {
    Q_OBJECT

private:
    /****************************************************************************/
    DeviceCommandProcessorThreadController();                                                                   ///< Not implemented.
    DeviceCommandProcessorThreadController(const DeviceCommandProcessorThreadController &);                     ///< Not implemented.
    const DeviceCommandProcessorThreadController & operator = (const DeviceCommandProcessorThreadController &); ///< Not implemented.
    /****************************************************************************/
    /**
     * \brief Destroy all created objects.
     */
    /****************************************************************************/
    void DestroyObjects();
    /****************************************************************************/

    virtual void ConnectDevices() = 0;

protected:
    /****************************************************************************/
    /**
     * \brief This method is called when the base class received the \ref Go signal.
     *
     * This means that everything is fine and normal operation started.
     * We are running in our own thread now.\n
     */
    /****************************************************************************/
    virtual void OnGoReceived();
    /****************************************************************************/
    /**
     * \brief This method is called when the base class received the \ref Stop signal.
     *
     * This means that normal operation will stop after processing this signal.
     * We are still running in our own thread.\n
     */
    /****************************************************************************/
    virtual void OnStopReceived();
    /****************************************************************************/
    /**
     * \brief Power will fail shortly.
     */
    /****************************************************************************/
    virtual void OnPowerFail();
    /****************************************************************************/
    /**
     * \brief Default handler for command timeouts.
     *
     * Will be called when no other handler is installed.
     *
     * \param[in]       Ref         The command reference.
     * \param[in]       CmdName     Name of command.
     */
    /****************************************************************************/
    virtual void OnCmdTimeout(Global::tRefType Ref, const QString &CmdName);

public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   TheHeartBeatSource    Logging source to be used.
     * \param[in]   name                  Name of the Thread Controller
     */
    /****************************************************************************/
    DeviceCommandProcessorThreadController(Global::gSourceType TheHeartBeatSource,
                                           QString name);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~DeviceCommandProcessorThreadController();
    /****************************************************************************/
    /**
     * \brief Create and configure needed objects.
     *
     * If something goes wrong, the already created objects must be deleted.
     */
    /****************************************************************************/
    virtual void CreateAndInitializeObjects();
    /****************************************************************************/
    /**
     * \brief Cleanup and destroy the created objects.
     */
    /****************************************************************************/
    virtual void CleanupAndDestroyObjects();
    /****************************************************************************/
    /**
     * \brief Command of type CmdDeviceProcessingInit received.
     *
     * \param[in]       Ref                 Reference of command.
     * \param[in]       Cmd                 Command.
     */
    /****************************************************************************/
    virtual void OnDeviceProcessingInit(Global::tRefType Ref, const CmdDeviceProcessingInit &Cmd);
    /****************************************************************************/
    /**
     * \brief Command of type CmdDeviceProcessingCleanup received.
     *
     * \param[in]       Ref                 Reference of command.
     * \param[in]       Cmd                 Command.
     */
    /****************************************************************************/
    virtual void OnDeviceProcessingCleanup(Global::tRefType Ref, const CmdDeviceProcessingCleanup &Cmd);

public slots:
    void DevProcInitialisationAckn(ReturnCode_t configResult);
    void DevProcConfigurationAckn(ReturnCode_t configResult);
    void DevProcStartNormalOpModeAckn(ReturnCode_t hdlInfo);
    void StartAdjustmentServiceAckn(ReturnCode_t hdlInfo);
    void DevProcModeChangeInfo(ReturnCode_t mode, quint8 newState);

protected:
    DeviceControl::IDeviceProcessing *m_pDeviceProcessing;    //!< Device processing instance

    /*! command state definition*/
    typedef enum {
        DCP_TC_CMD_STATE_FREE  = 0x00,  ///< command state free
        DCP_TC_CMD_STATE_ACTIV = 0x01   ///< command state activ
    } CommandRefState_t;

    /*! command container element*/
    typedef struct {
        CommandRefState_t               m_State;         ///< command state
        Global::tRefType                m_RefType;       ///< RefType from command
        DeviceControl::DevInstanceID_t  m_DevInstanceID; ///< Device instance identification
        QString                         m_CommandName;   ///< command name
    } CommandRef_t;

    CommandRef_t  m_CommandRef[CMD_REF_CNT];   ///< command container array

    //this is a quick solution to store the reftypes of incomming commands
    // until they will be acknowledged
    Global::tRefType   m_RefInitDCL;            ///< command reference of the initialisation command

    bool GetFreeCommandCtrlContainer(quint8& CmdIdx);
    bool GetCommandCtrlContainer(quint8& CmdIdx,
                                 DeviceControl::DevInstanceID_t InstanceID,
                                 QStringList& CommandNameList);

private:
    ReturnCode_t ReqRFIDData(LoaderRFIDChannel_t RfidChannel);



    Global::tRefType   m_InLoaderChangedRef;     ///< Expected LoaderChanged acknowledge reference.
    Global::tRefType   m_InUnloaderChangedRef;   ///< Expected LoaderChanged acknowledge reference.

    QMutex m_Mutex;   //!< Access serialization
}; // end class DeviceCommandProcessorThreadController

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_DEVICECOMMANDPROCESSORTHREADCONTROLLER_H
