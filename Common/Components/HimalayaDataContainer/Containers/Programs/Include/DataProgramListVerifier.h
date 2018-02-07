/****************************************************************************/
/*! \file DataProgramListVerifier.h
 *
 *  \brief Definition file for class CDataProgramListVerifier.
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

#ifndef DATAMANAGER_DATAPROGRAMLISTVERIFIER_H
#define DATAMANAGER_DATAPROGRAMLISTVERIFIER_H

#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"
#include "HimalayaDataContainer/Containers/Programs/Include/DataProgramList.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"

namespace DataManager {
class CDataContainer;

// constraints
const int PROGRAMS_MAX = 13;                //!< Max program allowed
const int NEXT_STEP_ID_MIN = 1;             //!< Minimum value of step Id
const int NAME_LENGTH_MIN = 1;        //!< name length Min
const int NAME_LENGTH_MAX = 20;        //!< name length Max
const int LONG_NAME_LENGTH_MIN = 1;         //!< Long name length Min
const int LONG_NAME_LENGTH_MAX = 32;        //!< Long name length Max
const int PROGRAM_STEPS_MAX = 14;           //!< Max program steps per program
const int PROGRAM_STEPS_MIN = 1;            //!< Minimum program steps per program
const int STEP_DURATION_MIN = 60;            //!< Minimum value of step duration in secs
const int STEP_DURATION_MAX = 359940;        //!< Maximum value of step duration in secs

const int SECONDS_PER_MIN = 60;             //!< number of secs per minute
const int MINS_PER_HOUR = 60;               //!< Number of mins per se
const int HOURS_PER_DAY = 24;               //!< Number of hours per day
const int ZERO_PERCENT = 0;                 //!< 0 %
const int HUNDRED_PERCENT = 100;            //!< 100 %


/****************************************************************************/
/*!
 *  \brief  This class implements Program list verifier
 */
/****************************************************************************/
class CDataProgramListVerifier:public IVerifierInterface
{
public:
    /**
    * \brief constructor
    */
    CDataProgramListVerifier();

    /**
    * \brief constructor
    * \iparam p_DataContainer data container
    */
    CDataProgramListVerifier(CDataContainer *p_DataContainer);

    /**
    * \brief Verify Data
    * \iparam p_DataProgramList data program list
    * \return verify status
    */
    bool VerifyData(CDataContainerBase* p_DataProgramList);  // use concrete class for concrete verifier

    //lint -esym(1536,GetErrors )
    /**
    * \brief get error
    * \return error map
    */
    ErrorMap_t &GetErrors();

    /**
    * \brief reset error
    */
    void ResetErrors();

    /**
    * \brief IsLocalVerifier
    * \return local verify type
    */
    bool IsLocalVerifier();

    /****************************************************************************/
    /*!
     *  \brief  Destructor
     */
    /****************************************************************************/
    virtual ~CDataProgramListVerifier();

private:
    CDataProgramList* mp_DPL;           //!< pointer to program list container
    ErrorMap_t m_ErrorsHash;           //!< To store Error ID and any arguments associated
    QStringList m_LeicaReagentIDList;   //!< To store the Leica reagent IDs
    CDataContainer *mp_DataContainer;   //!< DataContainer

    /**
    * \brief CheckProgramStep
    * \iparam p_Program program
    * \iparam VerifiedData VerifiedData
    */
    void CheckProgramStep(CProgram* p_Program,bool &VerifiedData);

    /**
    * \brief CheckDurationFormat
    * \iparam SplitList SplitList
    * \iparam VerifiedData VerifiedData
    * \iparam ProgramName program name
    */
    void CheckDurationFormat(QStringList SplitList, bool &VerifiedData, QString ProgramName);
};

} // namespace DataManager

#endif // DATAMANAGER_DATAPROGRAMLISTVERIFIER_H
