/****************************************************************************/
/*! \file Components/DataManager/Containers/DeviceConfiguration/Include/DeviceConfigurationVerifier.h
 *
 *  \brief DeviceConfigurationVerifier definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-09-04
 *   $Author:  $ Ningu, Ramya GJ
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

    ErrorMap_t &GetErrors();

    void ResetErrors();
    bool IsLocalVerifier();

protected:
    CDeviceConfigurationInterface* mp_DeviceConfigInterface;   ///< Local pointer to a DeviceConfig list
    ErrorMap_t         m_ErrorMap;                     //!< Event List for GUI and for logging purpose. This member is not copied when using copy constructor/Assignment operator

private:
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CDeviceConfigurationVerifier)
}; // CDeviceConfigurationVerifier

} // namespace DataManager
#endif // DATAMANAGER_DEVICECONFIGURATIONVERIFIER_H
