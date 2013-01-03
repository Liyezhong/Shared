/****************************************************************************/
/*! \file UserSettingsVerifier.h
 *
 *  \brief UserSettingsVerifier definition.
 *
 *   $Version: $ 0.2
 *   $Date:    $ 23.04.2012
 *   $Author:  $ M.Scherer, Raju
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
#ifndef DATAMANAGER_CUSERSETTINGSVERIFIER_H
#define DATAMANAGER_CUSERSETTINGSVERIFIER_H

#include <QString>
#include <QXmlStreamReader>

#include "DataManager/Containers/UserSettings/Include/UserSettingsInterface.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"

//lint -e1540

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  This class implements a list of rack data verifiers.
 */
/****************************************************************************/
class CUserSettingsVerifier : public IVerifierInterface
{
public:
    CUserSettingsVerifier();

    bool VerifyData(CDataContainerBase* p_DataRackList);  // use concrete class for concrete verifier

    ErrorHash_t &GetErrors();

    void ResetLastErrors();
    bool IsLocalVerifier();

    /****************************************************************************/
    /*!
     *  \brief  Destructor
     */
    /****************************************************************************/
    virtual ~CUserSettingsVerifier() {}
private:
    CUserSettingsInterface* mp_USettingsInterface;   ///< Local pointer to a rack list
    ErrorHash_t m_ErrorHash;          //!< To store Error ID and any arguments associated

    bool CheckLoaderReagentID(QString LoaderReagentID);
    void CheckSoundLevelWarnings(CUserSettings* UserSettings, bool& VerifiedData, QString& ErrorDescription);
    void CheckLoaderReagents(CUserSettings* UserSettings, bool& VerifiedData, QString& ErrorDescription);
};

} // namespace DataManager
#endif // DATAMANAGER_CUSERSETTINGSVERIFIER_H
