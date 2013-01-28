/****************************************************************************/
/*! \file Group.cpp
 *
 *  \brief Implementation file for class CGroup.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-03-01
 *  $Author:    $ Thirumalesha R
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

#include "DataManager/Containers/Parameter/Include/Group.h"

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Default Constructor
 */
/****************************************************************************/
CGroup::CGroup()
    : m_Key("")
{
    Init();
}

/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam Key = Group Key
 */
/****************************************************************************/
CGroup::CGroup(const QString Key)
              : m_Key(Key)
{
    Init();
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CGroup::~CGroup()
{
    DeleteAllParameter();
}

/****************************************************************************/
/*!
 *  \brief Initializes the object
 */
/****************************************************************************/
void CGroup::Init()
{
    m_ParameterList.clear();
    (void)DeleteAllParameter();
}

/****************************************************************************/
/*!
 *  \brief Function to retrieve Group for given index
 *  \iparam Index
 *
 *  \return Parameter
 */
/****************************************************************************/
const CParameter* CGroup::GetParameter(const unsigned int Index) const
{
    if (Index < (unsigned int)m_OrderedListOfParameterKeys.count()) {
        QString Key = m_OrderedListOfParameterKeys.value(Index);
        return m_ParameterList.value(Key, NULL);
    } else {
        return NULL;
    }
}

/****************************************************************************/
/*!
 *  \brief Function to retrieve Parameter for given index
 *  \iparam Index
 *  \iparam Parameter
 *
 *  \return true - success , false - failure
 */
/****************************************************************************/
bool CGroup::GetParameter(unsigned int Index, CParameter &Parameter) const
{
    if (Index < (unsigned int) m_OrderedListOfParameterKeys.count()) {
        QString Key = m_OrderedListOfParameterKeys.value(Index);
        Parameter = *m_ParameterList.value(Key, NULL);
        return true;
    }
    else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief Adds Parameter to Group
 *  \iparam p_Parameter = Parameter to add
 *
 *  \return true = add success , false - add failed
 */
/****************************************************************************/
bool CGroup::AddParameter(CParameter* p_Parameter)
{
    if (p_Parameter == NULL) return false;

    if (m_ParameterList.contains(p_Parameter->GetParameterKey())) {
        return false;
    } else {
        m_ParameterList.insert(p_Parameter->GetParameterKey(), p_Parameter);
        m_OrderedListOfParameterKeys.append(p_Parameter->GetParameterKey());
        return true;
    }
}

/****************************************************************************/
/*!
 *  \brief Adds Parameter offset to Group
 *  \iparam p_Parameter = Parameter to add
 *
 *  \return true = add success , false - add failed
 */
/****************************************************************************/
bool CGroup::AddParameterOffset(CParameter* p_Parameter, int ParameterOffset )
{
    if (p_Parameter == NULL) return false;

    if (m_ParameterList.contains(p_Parameter->GetParameterKey())) {
        p_Parameter->SetParameterOffset(ParameterOffset);
        m_ParameterList.insert(p_Parameter->GetParameterKey(),p_Parameter );
        m_OrderedListOfParameterKeys.append(p_Parameter->GetParameterKey());
        return true;
    }
}

/****************************************************************************/
/*!
 *  \brief Adds Parameter offset to Group
 *  \iparam p_Parameter = Parameter to add
 *
 *  \return true = add success , false - add failed
 */
/****************************************************************************/
bool CGroup::ResetParameterOffset(CParameter* p_Parameter)
{
    if (p_Parameter == NULL) return false;

    if (m_ParameterList.contains(p_Parameter->GetParameterKey())) {
        p_Parameter->SetParameterOffset(0);
        m_ParameterList.insert(p_Parameter->GetParameterKey(),p_Parameter );
        m_OrderedListOfParameterKeys.append(p_Parameter->GetParameterKey());
        return true;
    }
}
/****************************************************************************/
/*!
 *  \brief  Updates  Parameter of the Group
 *  \iparam p_Parameter = p_Parameter which needs to be updated
 *  \return true - update success, false - update failed
 */
/****************************************************************************/
bool CGroup::UpdateParameter(CParameter* p_Parameter)
{
    if (p_Parameter == NULL) return false;

    QString Key = p_Parameter->GetParameterKey();
    if (m_ParameterList.contains(Key)) {
        *m_ParameterList[Key] = *p_Parameter;
         return true;

    } else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Deletes the Group parameter
 *  \warning  If this function is called explicitly, then one should explicitly
 *            refresh expanded steplist.
 *  \iparam ParameterKey = Parameter Key of the Parameter to delete
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CGroup::DeleteParameter(const QString ParameterKey)
{
    if (m_ParameterList.contains(ParameterKey)) {
        //get Parameter from ParameterList and free memory
        delete m_ParameterList.value(ParameterKey);

        //remove pointer to ParameterStep from StepList
        m_ParameterList.remove(ParameterKey);

        int MatchIndex = -1;
        for (int i=0; i<m_OrderedListOfParameterKeys.count(); i++) {
            if (m_OrderedListOfParameterKeys[i] == ParameterKey) {
                MatchIndex = i;
                break;
            }
        }
        // MatchIndex MUST never be -1 !!!
        Q_ASSERT(MatchIndex != -1);
        m_OrderedListOfParameterKeys.removeAt(MatchIndex);

        return true;
    } else {
        // there is no program step with the given Parameter
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Deletes the Group parameter
 *  \iparam Index
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CGroup::DeleteParameter(const unsigned int Index)
{
    if (Index < (unsigned int)m_OrderedListOfParameterKeys.count()) {
        QString ParameterKey = m_OrderedListOfParameterKeys.value(Index);
        return DeleteParameter(ParameterKey);
    } else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Deletes all the Group parameter
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CGroup::DeleteAllParameter()
{
    bool Result = true;

    while (m_ParameterList.size() > 0) {
        if (DeleteParameter(0) == false) {
            Result = false;
        }
    }

    m_ParameterList.clear();
    m_OrderedListOfParameterKeys.clear();
    return Result;
}

/****************************************************************************/
/*!
 *  \brief  Used to swap Parameter index
 *  \iparam FromIndex = Index to move from.
 *  \iparam ToIndex = Index to move the Parameter step to.
 *  \return true - move success , false - move failed
 */
/****************************************************************************/
bool CGroup::MoveParameter(qint32 FromIndex, qint32 ToIndex)
{
    if (FromIndex >= 0 && FromIndex < m_ParameterList.size() && ToIndex >= 0 && ToIndex < m_ParameterList.size()) {
        m_OrderedListOfParameterKeys.move(FromIndex, ToIndex);
        return true;
    }
    return false;
}

}  // namespace DataManager
