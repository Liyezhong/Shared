/****************************************************************************/
/*! \file RackListVerifier.h
 *
 *  \brief CRackListVerifier definition.
 *
 *   $Version: $ 0.2
 *   $Date:    $ 2012-01-02, 2012-04-23
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

#ifndef DATAMANAGER_DATARACKLISTVERIFIER_H
#define DATAMANAGER_DATARACKLISTVERIFIER_H

#include <QString>
#include <QStringList>
#include <QXmlStreamReader>

#include "DataManager/Containers/Racks/Include/RackList.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "DataManager/Helper/Include/Types.h"
#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  This class implements a list of rack data verifiers.
 */
/****************************************************************************/
class CRackListVerifier : public IVerifierInterface
{
public:
    CRackListVerifier();

    bool VerifyData(CDataContainerBase* p_RackList);  // use concrete class for concrete verifier

    ErrorHash_t& GetErrors();

    void ResetLastErrors();

    bool IsLocalVerifier();

    /****************************************************************************/
    /*!
     *  \brief  Destructor
     */
    /****************************************************************************/
    virtual ~CRackListVerifier() {}
private:
    CRackList* mp_DRL;       ///< Local pointer to a rack list
    ErrorHash_t m_ErrorsHash;          //!< To store Error ID and any arguments associated

    bool CompareDateTime(QDateTime Source, QDateTime Destination);
    void ValidateRackDateTime(CRack* p_Rack, bool& VerifiedData, QString& ErrorDescription);
    void CompareRackDataTime(CRack* p_Rack, bool CheckColoradoDateTime, bool& VerifiedData, QString& ErrorDescription);
};

} // namespace DataManager

#endif // DATAMANAGER_DATARACKLISTVERIFIER_H
