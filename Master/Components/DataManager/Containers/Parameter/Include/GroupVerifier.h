/****************************************************************************/
/*! \file GroupVerifier.h
 *
 *  \brief Verifies the Parameter data .
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-03-01
 *   $Author:  $ Thirumalesha R
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
#ifndef GROUPVERIFIER_H
#define GROUPVERIFIER_H

#include <QString>
#include <QXmlStreamReader>

#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"
#include "ColoradoDataManager/Containers/Parameter/Include/GroupList.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"

namespace DataManager {
/****************************************************************************/
/*!
 *  \brief  This class implements verifiers for Parameter
 */
/****************************************************************************/
class CGroupVerifier : public IVerifierInterface
{
public:
    CGroupVerifier();

    bool VerifyData(CDataContainerBase* p_ParameterList);  // use concrete class for concrete verifier
    void CheckParameterStep(CGroup* p_Group, int ParameterCount, bool &VerifiedData);
    bool GetSyncObject(QReadWriteLock* p_ReadWriteLock);

    /*! \todo complete implementation*/
    ErrorHash_t& GetErrors();

    void ResetLastErrors();

    bool IsLocalVerifier();

    /****************************************************************************/
    /*!
     *  \brief  Destructor
     */
    /****************************************************************************/
    virtual ~CGroupVerifier() {}

private:
    CGroupList* mp_DPSL;               //!< Pointer to CGroupList
    ErrorHash_t m_ErrorsHash;          //!< To store Error Key and any arguments associated
};

} // namespace DataManager

#endif // PARAMETERVERIFIER_H
