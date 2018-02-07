/****************************************************************************/
/*! \file ProgramBase.h
 *
 *  \brief Definition file for class CProgramBase.
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

#ifndef DATAMANAGER_PROGRAMBASE_H
#define DATAMANAGER_PROGRAMBASE_H


#include "HimalayaDataContainer/Containers/Programs/Include/ProgramStep.h"
#include "DataManager/Helper/Include/Helper.h"

namespace DataManager {

typedef QHash<QString, CProgramStep*> ListOfProgramSteps_t; //!< Program Step List
/****************************************************************************/
/*!
 *  \brief   This is the base class for program
 */
/****************************************************************************/
class CProgramBase
{

protected:
    QString m_Name;                     //!<  program name
    QString m_NameID;       ///<  Definition/Declaration of variable m_NameID       ///<  Definition/Declaration of variable m_NameID

    ListOfIDs_t m_OrderedListOfStepIDs; //!< Ordered list of step id's
    QString m_ID;                               //!< program ID
    QString m_LongName;                         //!< Program long name
    unsigned int m_NextFreeStepID;              //!< Next available program ID
    ListOfProgramSteps_t m_StepList;            //!< Step List
    //This is made protected so that CProgram can set NextFreeStepId
    //void SetNextFreeStepID(const QString Value);

    //!< Made protected so that client classes don't call this explicitly.
    /****************************************************************************/
    /*!
     *  \brief Copy constructor
     *
     *  \iparam SourceProgramBase = const reference to sourceprogram base object
     */
    /****************************************************************************/
    CProgramBase(const CProgramBase& SourceProgramBase)
        :m_Name(""), m_NameID(""),m_ID(""), m_LongName(""), m_NextFreeStepID(0)
    {
        Q_UNUSED(SourceProgramBase);
        //!< Empty since data is getting copied in the derived class assingment operator func.
        //lint -e1529
    }

    //!< Made protected so that client classes don't call this explicitly.
    /****************************************************************************/
    /*!
     *  \brief Assignment operator
     *
     *  \iparam ProgramBase = const reference to sourceprogram base object
     *
     *  \return from CProgramBase& operator =
     */
    /****************************************************************************/
    //lint -efunc(1529, =)
    CProgramBase& operator = (const CProgramBase& ProgramBase)
    {
        CopyFromOther(ProgramBase);
        return *this;
        //!< Empty since data is getting copied in the derived class assingment operator func.
        //!< Defined so that compiler wont provide one !
    }
    void CopyFromOther(const CProgramBase &OtherProgramBase);
public:
    CProgramBase();
    CProgramBase(const QString ID, const QString Name, const QString LongName);
    CProgramBase(const QString ID, const QString Name);
    virtual ~CProgramBase();

    void InitObject();

    void SetNextFreeStepID(const QString Value);

    /****************************************************************************/
    /*!
     *  \brief  Retrieve the program id
     *
     *  \return ProgramID
     */
    /****************************************************************************/
    QString GetID() const {return m_ID;}
    /****************************************************************************/
    /*!
     *  \brief Sets the program ID
     *
     *  \iparam Value = Program ID
     */
    /****************************************************************************/
    void SetID(const QString Value){m_ID = Value;}

    /****************************************************************************/
    /*!
     *  \brief  Retrieve the program name
     *  \param forWriteXml true if the name is used for writing to xml file
     *  \return  name
     */
    /****************************************************************************/
    QString GetName(bool forWriteXml = false) const;
    /****************************************************************************/
    /*!
     *  \brief Sets the program name
     *
     *  \iparam Value = Program name
     */
    /****************************************************************************/
    void SetName(const QString Value);

    /****************************************************************************/
    /*!
     *  \brief  Retrieve the program name string id
     *
     *  \return  name string id
     */
    /****************************************************************************/
    QString GetNameID() const {return m_NameID;}
    /****************************************************************************/
    /*!
     *  \brief Sets the program name string id
     *
     *  \iparam Value = name string id
     */
    /****************************************************************************/
    void SetNameID(const QString Value){m_NameID = Value;}

    /****************************************************************************/
    /*!
     *  \brief  Retrieve the program long name
     *
     *  \return Program long name
     */
    /****************************************************************************/
    QString GetLongName() const{return m_LongName;}
    /****************************************************************************/
    /*!
     *  \brief Sets the program long name
     *
     *  \iparam Value = Program long name
     */
    /****************************************************************************/
    void SetLongName(const QString Value){m_LongName = Value;}

//    /****************************************************************************/
//    /*!
//     *  \brief Retrieves list of program steps in a program
//     *
//     *  \return Step list
//     */
//    /****************************************************************************/
//    ListOfProgramSteps_t* GetStepList()
//    {
//        return &m_StepList;
//    }

//    ListOfIDs_t* OrderedListOfStepIDs()
//    {
//        return &m_OrderedListOfStepIDs;
//    }
    //STEPS
    /****************************************************************************/
    /*!
     *  \brief  Retrieve the program step count
     *
     *  \return Program step count
     */
    /****************************************************************************/

    int  GetNumberOfSteps() const {return m_StepList.count();}
    /****************************************************************************/
    /*!
     *  \brief  Retrieve the next free program step id
     *
     *  \return Program step id
     */
    /****************************************************************************/

    QString GetNextFreeStepID(bool AutoCount);
    // the CreateStep functions won't add the created step to the internal step list!
    /****************************************************************************/
    /*!
     *  \brief  Creates a program step
     *
     *  \return Program Step
     */
    /****************************************************************************/
    CProgramStep* CreateProgramStep() {return new CProgramStep();}
    /****************************************************************************/
    /*!
     *  \brief  Creates a program step for the given step id
     *  \iparam StepID = ID of the program step
     *  \return Program step
     */
    /****************************************************************************/
    CProgramStep* CreateProgramStep(const QString StepID) {return new CProgramStep(StepID);} // given ID will be used
    /****************************************************************************/
    /*!
     *  \brief  Retrieves the program step associated with the given step id
     *  \iparam StepID = ID of the program step
     *  \return Program Step
     */
    /****************************************************************************/
    const CProgramStep* GetProgramStep(const QString StepID) const {return m_StepList.value(StepID, NULL);}  // uses unique step ID
    /****************************************************************************/
    /*!
     *  \brief  Retrieve the program step associated with given index
     *
     *  \return Program step
     */
    /****************************************************************************/
    const CProgramStep* GetProgramStep(const unsigned int Index) const; // uses order index
    bool GetProgramStep(const unsigned int Index, CProgramStep& ProgramStep) const;
    bool AddProgramStep(quint8 RowIndex, CProgramStep* p_ProgramStep);   // p_ProgramStep is added directly => don't delete outside!
    bool UpdateProgramStep(CProgramStep* p_ProgramStep);  // content of p_ProgramStep will be copied  => delete outside!
    bool DeleteProgramStep(const QString StepID);   // uses unique step ID
    virtual bool DeleteProgramStep(const unsigned int Index);  // uses order index
    bool DeleteAllProgramSteps();
    bool MoveProgramStep(qint32 FromIndex, qint32 ToIndex); // moves step inside a program step list
};

} // namespace DataManager

#endif // DATAMANAGER_PROGRAMBASE_H
