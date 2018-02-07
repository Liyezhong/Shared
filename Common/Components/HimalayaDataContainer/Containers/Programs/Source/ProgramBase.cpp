/****************************************************************************/
/*! \file ProgramBase.cpp
 *
 *  \brief Implementation file for class CProgramBase.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-08-01
 *  $Author:    $ F. Toth, N.Kamath
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

#include "HimalayaDataContainer/Containers/Programs/Include/ProgramBase.h"
#include "Global/Include/UITranslator.h"

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Default Constructor
 */
/****************************************************************************/
CProgramBase::CProgramBase()
    : m_Name(""), m_ID(""), m_LongName(""), m_NextFreeStepID(0)
{
    InitObject();
}

/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam ID = program ID
 *  \iparam Name = program short name
 *
 */
/****************************************************************************/
CProgramBase::CProgramBase(const QString ID, const QString Name)
              : m_Name(Name), m_ID(ID), m_NextFreeStepID(0)
{
    InitObject();
}
/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam ID = program ID
 *  \iparam Name = program short name
 *  \iparam LongName = program long name
 */
/****************************************************************************/
CProgramBase::CProgramBase(const QString ID, const QString Name, const QString LongName)
              : m_Name(Name), m_ID(ID), m_LongName(LongName), m_NextFreeStepID(0)
{
    InitObject();
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CProgramBase::~CProgramBase()
{
    try {
        (void)DeleteAllProgramSteps();
    }
    catch(...) {
    }
}

/****************************************************************************/
/*!
 *  \brief Initializes the object
 */
/****************************************************************************/
void CProgramBase::InitObject()
{
    m_NextFreeStepID = 0;  // this has to be a persistent value and unique for every single program and macro
    m_StepList.clear();
    (void)DeleteAllProgramSteps(); // to avoid lint warning 534
}

/****************************************************************************/
/*!
 *  \brief returns the Next available step Id
 *  \iparam AutoCount = true to increase step ID , false - doesn't increase
 *                       step ID.
 *  \return Next Step ID
 */
/****************************************************************************/
QString  CProgramBase::GetNextFreeStepID(bool AutoCount)
{
    QString TempID = "";
    (void)TempID.setNum(false); //to suppress lint-534
    TempID = QString::number(m_NextFreeStepID);
    if (AutoCount) m_NextFreeStepID++;
    return TempID;
}

/****************************************************************************/
/*!
 *  \brief Sets next free step id
 *  \iparam Value = next step id to set
 */
/****************************************************************************/
void CProgramBase::SetNextFreeStepID(const QString Value)
{
    m_NextFreeStepID = Value.toUInt();
}



/****************************************************************************/
/*!
 *  \brief Function to retrieve program step for given index
 *  \iparam Index
 *
 *  \return program step
 */
/****************************************************************************/
const CProgramStep* CProgramBase::GetProgramStep(const unsigned int Index) const
{
    if (Index < (unsigned int)m_OrderedListOfStepIDs.count()) {
        QString Key = m_OrderedListOfStepIDs.value(Index);
        return m_StepList.value(Key, NULL);
    } else {
        return NULL;
    }
}

/****************************************************************************/
/*!
 *  \brief Function to retrieve program step for given index
 *  \iparam Index
 *  \iparam ProgramStep
 *
 *  \return true - success , false - failure
 */
/****************************************************************************/
bool CProgramBase::GetProgramStep(unsigned int Index, CProgramStep &ProgramStep) const
{
    if (Index < (unsigned int) m_OrderedListOfStepIDs.count()) {
        QString Key = m_OrderedListOfStepIDs.value(Index);
        ProgramStep = *m_StepList.value(Key, NULL);
        return true;
    }
    else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief Adds program step to program
 *  \iparam p_ProgramStep = Program step to add
 *  \param RowIndex = quint8 type parameter
 *
 *  \return true = add success , false - add failed
 */
/****************************************************************************/
bool CProgramBase::AddProgramStep(quint8 RowIndex, CProgramStep* p_ProgramStep)
{
    if (p_ProgramStep == NULL) return false;

    if (m_StepList.contains(p_ProgramStep->GetStepID())) {
        // there is already a program step with the given StepID
        // => use UpdateProgramStep instead
        //    or call DeleteProgramStep before AddProgramStep
        return false;
    } else {
        m_StepList.insert(p_ProgramStep->GetStepID(), p_ProgramStep);
        //m_OrderedListOfStepIDs.append(p_ProgramStep->GetStepID());
        m_OrderedListOfStepIDs.insert(RowIndex + 1, p_ProgramStep->GetStepID());
        return true;
    }
}

/****************************************************************************/
/*!
 *  \brief  Updates program step of the program
 *  \iparam p_ProgramStep = program step which needs to be updated
 *  \return true - update success, false - update failed
 */
/****************************************************************************/
bool CProgramBase::UpdateProgramStep(CProgramStep* p_ProgramStep)
{
    if (p_ProgramStep == NULL) return false;

    QString ID = p_ProgramStep->GetStepID();
    if (m_StepList.contains(ID)) {
        *m_StepList[ID] = *p_ProgramStep;
         //delete p_ProgramStep;
         //p_ProgramStep = NULL; // Should be done by user
        return true;

    } else {
        // there is no program step with the given ID
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Deletes the progarm step
 *  \warning  If this function is called explicitly, then one should explicitly
 *            refresh expanded steplist.
 *  \iparam StepID = Step ID of the program to delete
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CProgramBase::DeleteProgramStep(const QString StepID)
{
    if (m_StepList.contains(StepID)) {
        //get ProgramStep from StepList and free memory
        delete m_StepList.value(StepID);

        //remove pointer to ProgramStep from StepList
        m_StepList.remove(StepID);

        //remove StepID from ID list
        int MatchIndex = -1;
        for (int i=0; i<m_OrderedListOfStepIDs.count(); i++) {
            if (m_OrderedListOfStepIDs[i] == StepID) {
                MatchIndex = i;
                break;
            }
        }
        // MatchIndex MUST never be -1 !!!
        Q_ASSERT(MatchIndex != -1);
        m_OrderedListOfStepIDs.removeAt(MatchIndex);

        return true;
    } else {
        // there is no program step with the given StepID
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Deletes the progarm step
 *  \iparam Index
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CProgramBase::DeleteProgramStep(const unsigned int Index)
{
    if (Index < (unsigned int)m_OrderedListOfStepIDs.count()) {
        QString StepID = m_OrderedListOfStepIDs.value(Index);
        return DeleteProgramStep(StepID);
    } else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Deletes all the progarm step
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CProgramBase::DeleteAllProgramSteps()
{
    bool Result = true;

    while (m_StepList.size() > 0) {
        if (DeleteProgramStep(0) == false) {
            Result = false;
        }
    }

    m_StepList.clear();
    m_OrderedListOfStepIDs.clear();

    return Result;
}

/****************************************************************************/
/*!
 *  \brief  Used to swap step index
 *  \iparam FromIndex = Index to move from.
 *  \iparam ToIndex = Index to move the program step to.
 *  \return true - move success , false - move failed
 */
/****************************************************************************/
bool CProgramBase::MoveProgramStep(qint32 FromIndex, qint32 ToIndex)
{
    if (FromIndex >= 0 && FromIndex < m_OrderedListOfStepIDs.size() && ToIndex >= 0 && ToIndex < m_OrderedListOfStepIDs.size()) {
        m_OrderedListOfStepIDs.move(FromIndex, ToIndex);
        return true;
    }
    return false;
}

QString CProgramBase::GetName(bool forWriteXml)const
{
    bool ok = false;
    if(!GetNameID().isEmpty() && !forWriteXml)
    {
        quint32 strid = GetNameID().toUInt(&ok);
        if(ok)
        {
            return Global::UITranslator::TranslatorInstance().Translate(
                    Global::TranslatableString(strid));
        }
    }
    return m_Name;
}

void CProgramBase::SetName(const QString Value)
{
    m_Name = Value;
}

/****************************************************************************/
/*!
 *  \brief Copy Data from another instance.
 *         This function should be called from CopyConstructor or
 *         Assignment operator only.
 *
 *  \iparam OtherProgramBase = Reference to the other programbase to be copied
 *
 */
/****************************************************************************/
void CProgramBase::CopyFromOther(const CProgramBase &OtherProgramBase)
{
    m_ID = OtherProgramBase.GetID();
    m_Name = OtherProgramBase.GetName();
    m_LongName = OtherProgramBase.GetLongName();
    m_NextFreeStepID = const_cast<CProgramBase&>(OtherProgramBase).GetNextFreeStepID(false).toUInt();
    /*! \note Steplist and Step Id list are updated automatically when AddProgramStep is
     called in derived class */
}

}  // namespace DataManager
