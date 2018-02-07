/****************************************************************************/
/*! \file DataReagentGroupVerifier.cpp
 *
 *  \brief DataReagentGroupList Verifier implementation.
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


#include "Global/Include/EventObject.h"
#include "HimalayaDataContainer/Containers/ReagentGroups/Include/DataReagentGroupVerifier.h"
#include "HimalayaDataContainer/Helper/Include/HimalayaDataManagerEventCodes.h"

namespace DataManager {
/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/

CDataReagentGroupListVerifier::CDataReagentGroupListVerifier() : mp_DPSL(NULL)
{
}

//bool CDataProgramListVerifier::VerifyData(CDataContainerBase* p_DataProgramList)

/****************************************************************************/
/*!
 *  \brief Function to verify the data present in xml

 *  \iparam p_ReagentGroupList
 *
 *  \return Changed something (true) or not (false)
 */
/****************************************************************************/
bool CDataReagentGroupListVerifier::VerifyData(CDataContainerBase *p_ReagentGroupList)
{
    Q_UNUSED(p_ReagentGroupList)
    return true;
}

/****************************************************************************/
/*!
 *  \brief  Gets the last errors which is done by verifier
 *
 *  \return QStringList - List of the errors occured
 */
/****************************************************************************/
ErrorMap_t& CDataReagentGroupListVerifier::GetErrors()
{
    // return the last error which is occured in the verifier
    return m_ErrorsHash;
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
void CDataReagentGroupListVerifier::ResetErrors()
{
    m_ErrorsHash.clear();
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 *  \return bool
 */
/****************************************************************************/
bool CDataReagentGroupListVerifier::IsLocalVerifier()
{
    return true;
}

}  // namespace DataManager


