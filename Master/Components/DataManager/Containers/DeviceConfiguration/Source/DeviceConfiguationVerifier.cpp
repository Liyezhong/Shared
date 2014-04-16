/****************************************************************************/
/*! \file Components/DataManager/Containers/DeviceConfiguration/Source/DeviceConfiguationVerifier.cpp
 *
 *  \brief DeviceConfigurationVerifier class implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-09-04
 *   $Author:  $ Ningu123, Ramya GJ
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

#include <QDebug>
#include <QFile>

#include "DataManager/Containers/DeviceConfiguration/Include/DeviceConfigurationVerifier.h"
#include "DataManager/Containers/DeviceConfiguration/Include/DeviceConfiguration.h"
#include "DataManager/Helper/Include/DataManagerEventCodes.h"
#include "Global/Include/EventObject.h"
//lint -e1536

namespace DataManager {



/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CDeviceConfigurationVerifier::CDeviceConfigurationVerifier() : mp_DeviceConfigInterface(NULL)
{

}

/****************************************************************************/
/*!
 *  \brief  Verifies Device Configuration
 *
 *  \iparam p_DeviceConfigInterface = Device Configuration interface class object
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CDeviceConfigurationVerifier::VerifyData(CDataContainerBase* p_DeviceConfigInterface)
{
    // by default make the verification flag to true
    bool VerifiedData = true;
    try {
        CHECKPTR(p_DeviceConfigInterface);
        // assign pointer to member variable
        mp_DeviceConfigInterface = static_cast<CDeviceConfigurationInterface*>(p_DeviceConfigInterface);
        CHECKPTR(p_DeviceConfigInterface);
        // check content of Device Configuration
        CDeviceConfiguration* p_DeviceConfig = mp_DeviceConfigInterface->GetDeviceConfiguration();
        CHECKPTR(p_DeviceConfig);
        // check the language
        QStringList LanguageList;
        QString Languages;
        LanguageList.clear();
        LanguageList = p_DeviceConfig->GetLanguageList();
        if(!LanguageList.isEmpty()) {
            for (qint32 I = 0; I < LanguageList.count(); I++) {
                Languages = LanguageList.at(I);
                //Need to implement a method to verify the language list
            }
            //need to return false once the language list verification method is freezed
        }

        return VerifiedData;
    }
    CATCHALL();

    return false;
}

/****************************************************************************/
/*!
 *  \brief  Gets the last errors which is done by verifier
 *
 *  \return QStringList - List of the errors occured
 */
/****************************************************************************/
ErrorMap_t& CDeviceConfigurationVerifier::GetErrors()
{
    // return the last error which is occured in the verifier
    return m_ErrorMap;
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
void CDeviceConfigurationVerifier::ResetErrors()
{
    m_ErrorMap.clear();
}

/****************************************************************************/
/*!
 *  \brief  Returns whether verifier is a local verifier or not
 *
 *  \return bool - On successful (True) otherwise (False)
 */
/****************************************************************************/
bool CDeviceConfigurationVerifier::IsLocalVerifier()
{
    return true;
}

}  // namespace DataManager


