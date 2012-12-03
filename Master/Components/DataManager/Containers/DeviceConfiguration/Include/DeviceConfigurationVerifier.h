/****************************************************************************/
/*! \file DeviceConfigurationVerifier.h
 *
 *  \brief DeviceConfigurationVerifier definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-09-04
 *   $Author:  $ Ningu
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
#ifndef DATAMANAGER_DEVICECONFIGURATIONVERIFIER_H
#define DATAMANAGER_DEVICECONFIGURATIONVERIFIER_H

#include <QString>
#include <QXmlStreamReader>

#include "DataManager/Containers/DeviceConfiguration/Include/DeviceConfigurationInterface.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  This class implements a list of DeviceConfiguration data verifiers.
 */
/****************************************************************************/
class CDeviceConfigurationVerifier : public IVerifierInterface
{
public:
    CDeviceConfigurationVerifier();

    bool VerifyData(CDataContainerBase* p_DataDeviceConfigList);  // use concrete class for concrete verifier

    bool GetSyncObject(QReadWriteLock* p_ReadWriteLock);

    ErrorHash_t &GetErrors();

    void ResetLastErrors();
    bool IsLocalVerifier();

    /****************************************************************************/
    /*!
     *  \brief  Destructor
     */
    /****************************************************************************/
    virtual ~CDeviceConfigurationVerifier() {}

private:
    CDeviceConfigurationInterface* mp_DeviceConfigInterface;   ///< Local pointer to a DeviceConfig list
    ErrorHash_t m_ErrorsHash;          //!< To store Error ID and any arguments associated
}; // CDeviceConfigurationVerifier

} // namespace DataManager
#endif // DATAMANAGER_DEVICECONFIGURATIONVERIFIER_H
