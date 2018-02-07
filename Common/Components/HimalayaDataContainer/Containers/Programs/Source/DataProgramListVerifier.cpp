/****************************************************************************/
/*! \file DataProgramListVerifier.cpp
 *
 *  \brief Implementation file for class CDataProgramListVerifier.
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


#include "HimalayaDataContainer/Containers/Programs/Include/DataProgramListVerifier.h"
#include "HimalayaDataContainer/Containers/Programs/Include/DataProgramList.h"
#include "HimalayaDataContainer/Containers/Programs/Include/ProgramBase.h"
#include "Global/Include/EventObject.h"
#include "HimalayaDataManager/Include/DataContainer.h"

//lint -e641
//lint -e1536

namespace DataManager {
/****************************************************************************/
/*!
 *  \brief Default Constructor
 */
/****************************************************************************/
CDataProgramListVerifier::CDataProgramListVerifier(CDataContainer *p_DataContainer)
    :mp_DPL(NULL)
    ,mp_DataContainer(p_DataContainer)
{

}

CDataProgramListVerifier::CDataProgramListVerifier()
    :mp_DPL(NULL),
    mp_DataContainer(NULL)
{

}

CDataProgramListVerifier::~CDataProgramListVerifier()
{
    mp_DPL = NULL;
    mp_DataContainer = NULL;
}
/****************************************************************************/
/*!
 *  \brief Verifies the data present in the program list against constraints.
 */
/****************************************************************************/
bool CDataProgramListVerifier::VerifyData(CDataContainerBase* p_DataProgramList)
{
    // by default make the verification flag to true
    bool VerifiedData = true;

    // assign pointer to member variable
    mp_DPL = static_cast<CDataProgramList*>(p_DataProgramList);

    if (!mp_DPL) {
        Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_ERROR_NULL_POINTER, Global::GUI_MSG_BOX);
        (void)m_ErrorsHash.insert(EVENT_DM_PROG_INVALID_DURATION_FORMAT, Global::tTranslatableStringList());
        return false;
    }
    // check constraints

    // check max number of programs (12)
    if (mp_DPL->GetNumberOfPrograms() > PROGRAMS_MAX) {
        qDebug() << "### Too many programs (max 12 allowed): " << mp_DPL->GetNumberOfPrograms();
        (void)m_ErrorsHash.insert(EVENT_DM_PROG_COUNT_EXCEEDS_LIMIT, Global::tTranslatableStringList() << QString::number(PROGRAMS_MAX) );
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_PROG_COUNT_EXCEEDS_LIMIT,
                                                   Global::tTranslatableStringList() << QString::number(PROGRAMS_MAX), (int)Global::GUI_MSG_BOX);
        VerifiedData = false;
    }
//    QString NextStepID = mp_DPL->GetNextFreeProgID(false).mid(1);

    // check content of each program
    CProgram *p_Program;
    // empty the string
    m_LeicaReagentIDList.clear();
    for (qint32 I = 0; I < mp_DPL->GetNumberOfPrograms(); I++) {
        p_Program = mp_DPL->GetProgram(I);
        if (p_Program) {

            //Verify program ID
            if ((p_Program->GetID().at(0) != 'U') && (p_Program->GetID().at(0) != 'L') && (p_Program->GetID().at(0) != 'C')) {
                qDebug()<<"Invalid Program Id"<<p_Program->GetID();
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_PROGRAM_ID,
                                                           Global::tTranslatableStringList() << p_Program->GetID()
                                                           << p_Program->GetName(),
                                                           Global::GUI_MSG_BOX);
                (void)m_ErrorsHash.insert(EVENT_DM_INVALID_PROGRAM_ID,
                                   Global::tTranslatableStringList() << p_Program->GetID()
                                   << p_Program->GetName());
                VerifiedData = false;
            }
            bool Ok = false;
            int ProgramId = p_Program->GetID().mid(1).toInt(&Ok);
            if ((!Ok) && (!(ProgramId >= 1))) {
                qDebug()<<"### Invalid program Id:"<<p_Program->GetID();
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_PROGRAM_ID,
                                                           Global::tTranslatableStringList() << p_Program->GetID()
                                                           << p_Program->GetName(),
                                                           Global::GUI_MSG_BOX);
                (void)m_ErrorsHash.insert(EVENT_DM_INVALID_PROGRAM_ID,
                                   Global::tTranslatableStringList() << p_Program->GetID()
                                   << p_Program->GetName());
                VerifiedData = false;
            }

            //Verify name length constraints
            if ((p_Program->GetName().length() < NAME_LENGTH_MIN) ||
                    (p_Program->GetName().length() > NAME_LENGTH_MAX)) {
                qDebug()<<"###Short name length check failed"<<p_Program->GetName();
                (void)m_ErrorsHash.insert(EVENT_DM_PROG_NAME_LENGTH_CHECK_FAILED,
                                   Global::tTranslatableStringList() <<  p_Program->GetName()
                                   <<QString::number(NAME_LENGTH_MIN) << QString::number(NAME_LENGTH_MAX));
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_PROG_NAME_LENGTH_CHECK_FAILED,
                                                           Global::tTranslatableStringList() << p_Program->GetName()
                                                          <<QString::number(NAME_LENGTH_MIN) << QString::number(NAME_LENGTH_MAX),
                                                            Global::GUI_MSG_BOX);
                VerifiedData = false;
            }

            // user program name can not contain leica letter
            if ((p_Program->GetName().contains("LEICA",Qt::CaseInsensitive) && p_Program->GetID().at(0) != 'L')) {
                qDebug()<<"###user program name can not contain leica letter-- check failed.";
                (void)m_ErrorsHash.insert(EVENT_DM_USER_PROGRAM_NAME_CONTAINS_LEICA,
                                   Global::tTranslatableStringList() <<  p_Program->GetName());
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_USER_PROGRAM_NAME_CONTAINS_LEICA,
                                                           Global::tTranslatableStringList() << p_Program->GetName(),
                                                            Global::GUI_MSG_BOX);
                VerifiedData = false;
            }


            //check no of steps
            if (!p_Program->IsLeicaProgram() && p_Program->GetNumberOfSteps() > PROGRAM_STEPS_MAX) {
                (void)m_ErrorsHash.insert(EVENT_DM_PROG_STEP_COUNT_EXCEEDS_LIMIT,
                                   Global::tTranslatableStringList() <<  p_Program->GetName()
                                   << QString::number(PROGRAM_STEPS_MAX));
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_PROG_STEP_COUNT_EXCEEDS_LIMIT,
                                                           Global::tTranslatableStringList() <<  p_Program->GetName()
                                                           << QString::number(PROGRAM_STEPS_MAX),
                                                           Global::GUI_MSG_BOX);
                qDebug("### Program Steps should be less than 14");
                VerifiedData = false;
            }

            if (p_Program->GetNumberOfSteps() < PROGRAM_STEPS_MIN) {
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_PROG_STEP_COUNT_LESS_LIMIT,
                                                           Global::tTranslatableStringList() <<  p_Program->GetName()
                                                           << QString::number(PROGRAM_STEPS_MIN) << QString::number(PROGRAM_STEPS_MAX),
                                                           Global::GUI_MSG_BOX);
                (void)m_ErrorsHash.insert(EVENT_DM_PROG_STEP_COUNT_LESS_LIMIT,
                                    Global::tTranslatableStringList() <<  p_Program->GetName()
                                    << QString::number(PROGRAM_STEPS_MIN));
                qDebug()<<"### ProgramId less than 1";
                VerifiedData = false;
            }

            CheckProgramStep(p_Program, VerifiedData);
        }
    }//End of program for loop

    return VerifiedData;
}

/****************************************************************************/
/*!
 *  \brief  Verifies the program step
 *
 *  \iparam p_Program - Pointer to CProgram
 *  \iparam VerifiedData = verifier flag value
 *
 */
/****************************************************************************/
void CDataProgramListVerifier::CheckProgramStep(CProgram* p_Program, bool &VerifiedData)
{
    CProgramStep p_ProgramStep;
    QStringList ReagentIDList;

    for (qint32 X = 0; X < p_Program->GetNumberOfSteps(); X++) {
        if(p_Program->GetProgramStep(X, p_ProgramStep)){//use order index
            QString ReagentID = p_ProgramStep.GetReagentID();
            // Reagent is empty
            if(ReagentID.isEmpty())
            {
                (void)m_ErrorsHash.insert(EVENT_DM_PROG_VERFIER_INVALID_REAGENT_ID,
                                    Global::tTranslatableStringList() << p_Program->GetName()
                                    << p_ProgramStep.GetReagentID());
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_PROG_VERFIER_INVALID_REAGENT_ID,
                                                           Global::tTranslatableStringList() << p_Program->GetName()
                                                           << p_ProgramStep.GetReagentID(),
                                                           Global::GUI_MSG_BOX);
                qDebug() << "### Reagent is empty ";
                VerifiedData = false;
                break;
            }
            //Validate Reagent ID
            switch(ReagentID.at(0).toLatin1()) {
                case 'U':
                case 'L':
                case 'C':
                    break;
                default:
                    (void)m_ErrorsHash.insert(EVENT_DM_PROG_VERFIER_INVALID_REAGENT_ID,
                                        Global::tTranslatableStringList() << p_Program->GetName()
                                        << p_ProgramStep.GetReagentID());
                    Global::EventObject::Instance().RaiseEvent(EVENT_DM_PROG_VERFIER_INVALID_REAGENT_ID,
                                                               Global::tTranslatableStringList() << p_Program->GetName()
                                                               << p_ProgramStep.GetReagentID(),
                                                               Global::GUI_MSG_BOX);
                    qDebug() << "### Reagent ID doesn't start with S,L,U: ";
                    VerifiedData = false;
                    break;
            }

            ReagentIDList << ReagentID;


            bool IntOk = false;
            //Validate Step ID.
            //ignoring return value of "toInt()"
            (void)(p_ProgramStep.GetStepID().toInt(&IntOk));
            if (!IntOk) {
                (void)m_ErrorsHash.insert(EVENT_DM_INVALID_STEP_ID,
                                    Global::tTranslatableStringList() << p_Program->GetName()
                                    <<  QString::number(X+1));
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_STEP_ID,
                                                           Global::tTranslatableStringList() << p_Program->GetName()
                                                           << QString::number(X+1),
                                                           Global::GUI_MSG_BOX);
                qDebug("### invalid StepId");
                VerifiedData = false;
            }

            QStringList SplitList;
            SplitList = p_ProgramStep.GetDuration().split(" ");

            qDebug()<<"CDataProgramListVerifier::CheckProgramStep  programsetup.getDuration= "<< p_ProgramStep.GetDuration();

            // check the duration format
            CheckDurationFormat(SplitList, VerifiedData, p_Program->GetName());

            // check duration rang
            if(VerifiedData)
            {
                int duration = Helper::ConvertTimeStringToSeconds(p_ProgramStep.GetDuration());
                if(duration > STEP_DURATION_MAX  || duration < STEP_DURATION_MIN)
                {
                    (void)m_ErrorsHash.insert(EVENT_DM_PROG_STEP_DURATION_EXCEED_LIMIT,
                                        Global::tTranslatableStringList() <<  QString::number(X+1) << p_Program->GetName()
                                        <<Helper::ConvertSecondsToTimeString(STEP_DURATION_MIN) << Helper::ConvertSecondsToTimeString(STEP_DURATION_MAX));
                    Global::EventObject::Instance().RaiseEvent(EVENT_DM_PROG_STEP_DURATION_EXCEED_LIMIT,
                                                               Global::tTranslatableStringList() << QString::number(X+1) << p_Program->GetName()
                                                               <<Helper::ConvertSecondsToTimeString(STEP_DURATION_MIN) << Helper::ConvertSecondsToTimeString(STEP_DURATION_MAX),
                                                               Global::GUI_MSG_BOX);
                    qDebug() << "##### Invalid Duration";
                    VerifiedData = false;
                }
            }


        } // End of if
    }// End of for loop
}

/****************************************************************************/
/*!
 *  \brief  Verifies the program step
 *
 *  \iparam SplitList - String list for the date format
 *  \iparam VerifiedData = verifier flag value
 *  \iparam ProgramName
 *
 */
/****************************************************************************/
void CDataProgramListVerifier::CheckDurationFormat(QStringList SplitList, bool &VerifiedData, QString ProgramName)
{
    int Value = 0;
    bool Ok = true;

    foreach(QString SplitString, SplitList)
    {
        if (SplitString.endsWith("d")) {
            SplitString.chop(1);
            Value = SplitString.toInt(&Ok);

            if (!Ok) {
                (void)m_ErrorsHash.insert(EVENT_DM_PROG_INVALID_DURATION_FORMAT,
                                    Global::tTranslatableStringList() << ProgramName);
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_PROG_INVALID_DURATION_FORMAT,
                                                           Global::tTranslatableStringList() << ProgramName,
                                                           Global::GUI_MSG_BOX);
                qDebug() << "##### Invalid Day Duration";
                VerifiedData = false;
            }

            if (Value > 4 || Value < 0) {  // The max time supported is 99:59 , so the max day is 4
                (void)m_ErrorsHash.insert(EVENT_DM_PROG_INVALID_DURATION_FORMAT,
                                    Global::tTranslatableStringList() << ProgramName);
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_PROG_INVALID_DURATION_FORMAT,
                                                           Global::tTranslatableStringList() << ProgramName,
                                                           Global::GUI_MSG_BOX);
                qDebug() << "##### Invalid Day Duration";
                VerifiedData = false;
            }

        } else if (SplitString.endsWith("h")) {
            SplitString.chop(1);
            Value = SplitString.toInt(&Ok);
            if (!Ok) {
                (void)m_ErrorsHash.insert(EVENT_DM_PROG_INVALID_DURATION_FORMAT,
                                    Global::tTranslatableStringList() << ProgramName);
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_PROG_INVALID_DURATION_FORMAT,
                                                           Global::tTranslatableStringList() << ProgramName,
                                                           Global::GUI_MSG_BOX);
                qDebug() << "##### Invalid Hour Duration";
                VerifiedData = false;
            }
            if ( Value > 24 || Value < 0) {
                (void)m_ErrorsHash.insert(EVENT_DM_PROG_INVALID_DURATION_FORMAT,
                                    Global::tTranslatableStringList() << ProgramName);
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_PROG_INVALID_DURATION_FORMAT,
                                                           Global::tTranslatableStringList() << ProgramName,
                                                           Global::GUI_MSG_BOX);
                qDebug() << "##### Invalid Hour Duration";
                VerifiedData = false;
            }
        } else if (SplitString.endsWith("m")) {
            SplitString.chop(1);
            Value = SplitString.toInt(&Ok);
            if (!Ok) {
                (void)m_ErrorsHash.insert(EVENT_DM_PROG_INVALID_DURATION_FORMAT,
                                    Global::tTranslatableStringList() << ProgramName);
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_PROG_INVALID_DURATION_FORMAT,
                                                           Global::tTranslatableStringList() << ProgramName,
                                                           Global::GUI_MSG_BOX);
                qDebug() << "#####Invalid Minute Duration ";
                VerifiedData = false;
            }
            if (Value > 60 || Value < 0) {
                (void)m_ErrorsHash.insert(EVENT_DM_PROG_INVALID_DURATION_FORMAT,
                                    Global::tTranslatableStringList() << ProgramName);
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_PROG_INVALID_DURATION_FORMAT,
                                                           Global::tTranslatableStringList() << ProgramName,
                                                           Global::GUI_MSG_BOX);
                qDebug() << "##### Invalid Minute Duration";
                VerifiedData = false;
            }
        } else if (SplitString.endsWith("s")) {
            SplitString.chop(1);
            Value = SplitString.toInt(&Ok);
            if (!Ok) {
                (void)m_ErrorsHash.insert(EVENT_DM_PROG_INVALID_DURATION_FORMAT,
                                    Global::tTranslatableStringList() << ProgramName);
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_PROG_INVALID_DURATION_FORMAT,
                                                           Global::tTranslatableStringList() << ProgramName,
                                                           Global::GUI_MSG_BOX);
                qDebug() << "##### Invalid Seconds Duration";
                VerifiedData = false;
            }
            if (Value > 60 || Value < 0) {
                (void)m_ErrorsHash.insert(EVENT_DM_PROG_INVALID_DURATION_FORMAT,
                                    Global::tTranslatableStringList() << ProgramName);
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_PROG_INVALID_DURATION_FORMAT,
                                                           Global::tTranslatableStringList() << ProgramName,
                                                           Global::GUI_MSG_BOX);
                qDebug() << "##### Invalid Seconds Duration";
                VerifiedData = false;
            }

        }

    }//End of foreach
}


/****************************************************************************/
/*!
 *  \brief  Gets the last errors which is done by verifier
 *
 *  \return QStringList - List of the errors occured
 */
/****************************************************************************/
ErrorMap_t& CDataProgramListVerifier::GetErrors()
{
    // return the last error which is occured in the verifier
    return m_ErrorsHash;
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
void CDataProgramListVerifier::ResetErrors()
{
    m_ErrorsHash.clear();
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
bool CDataProgramListVerifier::IsLocalVerifier()
{
    return true;
}

}  // namespace DataManager

