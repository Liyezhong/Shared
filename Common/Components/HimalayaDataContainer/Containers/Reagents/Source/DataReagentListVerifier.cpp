
/****************************************************************************/
/*! \file DataReagentListVerifier.cpp
 *
 *  \brief DataReagentListVerifier Implementation.
 *
 *   $Version: $ 0.2
 *   $Date:    $ 2012-04-28, 2013-01-17
 *   $Author:  $ Vikram MK, Swati Tiwari
 *
 *  \b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2012 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "HimalayaDataContainer/Containers/Reagents/Include/DataReagentListVerifier.h"
#include "HimalayaDataContainer/Containers/Reagents/Include/DataReagentList.h"
#include "HimalayaDataContainer/Containers/Reagents/Include/Reagent.h"

namespace DataManager {

CDataReagentListVerifier::CDataReagentListVerifier() : mp_DRL(NULL)
{

}

CDataReagentListVerifier::~CDataReagentListVerifier()
{
    mp_DRL = NULL;
}

/****************************************************************************/
/*!
 *  \brief Verifies the Constraints of Particular Reagent list
 *
 *  \iparam p_DataReagentList = Instance of the CDateReagentList class
 *
 *  \return true or false
 */
/****************************************************************************/
bool CDataReagentListVerifier::VerifyData(CDataContainerBase* p_DataReagentList)
{
    Q_UNUSED(p_DataReagentList);
    return true;
}


/****************************************************************************/
/*!
 *  \brief  Gets the last errors which is done by verifier
 *
 *  \return QStringList - List of the errors occured
 */
/****************************************************************************/
ErrorMap_t& CDataReagentListVerifier::GetErrors()
{
    // return the last error which is occured in the verifier
    return m_ErrorsHash;
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
void CDataReagentListVerifier::ResetErrors()
{
    m_ErrorsHash.clear();
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 *  \return bool
 */
/****************************************************************************/
bool CDataReagentListVerifier::IsLocalVerifier()
{
    return true;
}

}  // namespace DataManager

