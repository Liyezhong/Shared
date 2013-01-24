/****************************************************************************/
/*! \file GroupVerifier.cpp
 *
 *  \brief Group Verifier implementation.
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
#include <QDebug>
#include <QFile>
#include <QStringList>

#include "ColoradoDataManager/Containers/Parameter/Include/GroupVerifier.h"
#include "ColoradoDataManager/Containers/Parameter/Include/GroupList.h"
#include "ColoradoDataManager/Containers/Parameter/Include/Parameter.h"
#include "Global/Include/EventObject.h"

namespace DataManager {
const int PARAMETERS_MAX = 50;
const int RESULT_CODE_COUNT = 5;
/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/

CGroupVerifier::CGroupVerifier() : mp_DPSL(NULL)
{
}


/****************************************************************************/
/*!
 *  \brief Function to verify the data present in xml

 *  \iparam p_ParameterList
 *
 *  \return Changed something (true) or not (false)
 */
/****************************************************************************/
bool CGroupVerifier::VerifyData(CDataContainerBase *p_ParameterList)
{
    // by default make the verification flag to true
    bool VerifiedData = true;

    // assign pointer to member variable
    mp_DPSL = static_cast<CGroupList*>(p_ParameterList);

    if (!mp_DPSL) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_NULL_PTR);
        return false;
    }
    // check constraints

      // check content of each Group
    CGroup *p_Group;

    QStringList AssignedColorList;
    for (qint32 I = 0; I < mp_DPSL->GetNumberofGroups(); I++) {
        p_Group = mp_DPSL->GetGroup(I);
        if (p_Group) {
            //Verify Group Key
            if (!(p_Group->GetGroupKey().at(0) != 'G' || p_Group->GetGroupKey().at(0) != 'O' ||
                 p_Group->GetGroupKey().at(0) != 'L' || p_Group->GetGroupKey().at(0) != 'R' || p_Group->GetGroupKey().at(0) != 'A')) {
                 qDebug()<<"Invalid Group Key"<<p_Group->GetGroupKey();
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_PROGRAM_ID,
                                                            Global::tTranslatableStringList() << p_Group->GetGroupKey());
                m_ErrorsHash.insert(EVENT_DM_INVALID_PROGRAM_ID,
                                   Global::tTranslatableStringList() <<  p_Group->GetGroupKey());
                VerifiedData = false;
            }
         }
    }//End of Groups for loop

    return VerifiedData;
}

/****************************************************************************/
/*!
 *  \brief Function to get Sync object

 *  \iparam p_ReadWriteLock
 *
 *  \return Changed something (true) or not (false)
 */
/****************************************************************************/
bool CGroupVerifier::GetSyncObject(QReadWriteLock* p_ReadWriteLock)
{
    p_ReadWriteLock = NULL;
    return true;
}

/****************************************************************************/
/*!
 *  \brief  Gets the last errors which is done by verifier
 *
 *  \return QStringList - List of the errors occured
 */
/****************************************************************************/
ErrorHash_t& CGroupVerifier::GetErrors()
{
    // return the last error which is occured in the verifier
    return m_ErrorsHash;
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
void CGroupVerifier::ResetLastErrors()
{
    m_ErrorsHash.clear();
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
bool CGroupVerifier::IsLocalVerifier()
{
    return true;
}


}  // namespace DataManager


