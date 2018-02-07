
/****************************************************************************/
/*! \file DataReagentListVerifier.h
 *
 *  \brief Definition file for class CDataReagentListVerifier.
 *
 *   $Version: $ 0.2
 *   $Date:    $ 2012-05-07, 2013-01-03
 *   $Author:  $ Vikram MK,Swati Tiwari
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

#ifndef DATAMANAGER_DATAREAGENTLISTVERIFIER_H
#define DATAMANAGER_DATAREAGENTLISTVERIFIER_H


#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"
#include "HimalayaDataContainer/Containers/Reagents/Include/DataReagentList.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "Global/Include/EventObject.h"

namespace DataManager {

// These constants are used to check in local verifiers
const int   MIN_REAGENTS = 1;                           //!< minimum number of reagents allowed
const int   MAX_REAGENTS = 100;                         //!< maximum number of reagents allowed
const int   MAX_CHARACTERS_NAME = 32;                   //!< Maximum characters size for long name
const int   MIN_REAGENT_USABLE_DAYS_INSECONDS = 180000; //!< Minimum usable days for 1 day = 86400 seconds
const int   MAX_CASSETTES_TOREPLACE_REAGENT = 500;      //!< Maximum number of Cassettes allowed
const int   HEATED_CUVETE_UNUSED_TEMPERATURE = 0;       //!< Heated Cuvette is not Used for reagent

/****************************************************************************/
/*!
 *  \brief CDataReagentListVerifier class Inherited from IVerifierInterface
 */
/****************************************************************************/
class CDataReagentListVerifier : public IVerifierInterface
{
public:
    /**
      * \brief constructor
    */
    CDataReagentListVerifier();

    /**
       * \brief verify data
       * \iparam p_DataReagentList reagent list
       * \return bool
    */
    bool VerifyData(CDataContainerBase* p_DataReagentList);  // use concrete class for concrete verifier

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
      * \brief local verifier
      * \return bool
    */
    bool IsLocalVerifier();

    /****************************************************************************/
    /*!
     *  \brief  Destructor
     */
    /****************************************************************************/
    virtual ~CDataReagentListVerifier();
private:
    CDataReagentList* mp_DRL;         //!< Reagent Data Container
    ErrorMap_t m_ErrorsHash;          //!< To store Error ID and any arguments associated
};

} // namespace DataManager

#endif // DATAMANAGER_DATAREAGENTLISTVERIFIER_H
