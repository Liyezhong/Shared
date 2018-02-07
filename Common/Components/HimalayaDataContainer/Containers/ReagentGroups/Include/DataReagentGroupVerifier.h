/****************************************************************************/
/*! \file DataReagentGroupList.h
 *
 *  \brief DataReagentGroupListVerifier defination.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-01-30
 *   $Author:  $ Swati Tiwari
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
#ifndef DATAMANAGER_DATAREAGENTGROUPLISTVERIFIER_H
#define DATAMANAGER_DATAREAGENTGROUPLISTVERIFIER_H


#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"
#include "HimalayaDataContainer/Containers/ReagentGroups/Include/DataReagentGroupList.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"

namespace DataManager {
// constraints

/****************************************************************************/
/*!
 *  \brief  This class implements verifiers for program sequence
 */
/****************************************************************************/
class CDataReagentGroupListVerifier : public IVerifierInterface
{
public:
    /**
      * \brief constructor
    */
    CDataReagentGroupListVerifier();

    /**
      * \brief verify data
      * \iparam p_ReagentGroupList group list
      * \return bool
    */
    bool VerifyData(CDataContainerBase* p_ReagentGroupList);

    //lint -esym(1536,GetErrors )
    /**
      * \brief get error
      * \return error map
    */
    ErrorMap_t& GetErrors();

    /**
      * \brief reset error
    */
    void ResetErrors();

    /**
      * \brief local verifier
    */
    bool IsLocalVerifier();

private:
    CDataReagentGroupList* mp_DPSL;   //!< Pointer to program Sequence List
    ErrorMap_t m_ErrorsHash;          //!< To store Error ID and any arguments associated
};

} // namespace DataManager

#endif // DATAMANAGER_DATAREAGENTGROUPLISTVERIFIER_H
