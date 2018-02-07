/****************************************************************************/
/*! \file SpecialVerifierGroupC.h
 *
 *  \brief Definition file for class CSpecialVerifierGroupB.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-06-15
 *  $Author:    $ Raju
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
#ifndef DATAMANAGER_SPECIALVERIFIERGROUPC_H
#define DATAMANAGER_SPECIALVERIFIERGROUPC_H


#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"
#include "HimalayaDataContainer/Containers/Reagents/Include/DataReagentList.h"
#include "DataManager/Containers/UserSettings/Include/UserSettingsInterface.h"

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  This class implements Special verifier Group C
 */
/****************************************************************************/
// need to add error interface to special verifier
class CSpecialVerifierGroupC : public IVerifierInterface
{
public:

    // instead of sending all the arguments please put all class objects in a Hash table with a Key
    // so that, in feature if it adds up new arguments and function will not be changed.
    CSpecialVerifierGroupC(CDataReagentList* p_DataReagentList, CUserSettingsInterface* p_USInterface);

    bool VerifyData(CDataContainerBase* p_DataProgramList);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetErrors
     *
     *  \return from GetErrors
     */
    /****************************************************************************/
    ErrorMap_t& GetErrors() { return m_ErrorsHash;}

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ResetErrors
     *
     *  \return from ResetErrors
     */
    /****************************************************************************/
    void ResetErrors(){m_ErrorsHash.clear();}

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IsLocalVerifier
     *
     *  \return from IsLocalVerifier
     */
    /****************************************************************************/
    bool IsLocalVerifier(){return false;}    

private:
    CSpecialVerifierGroupC();
    CSpecialVerifierGroupC(const CSpecialVerifierGroupC&); ///< Not implemented
    CDataReagentList* mp_DReagentList; ///< Container for the Reagent list
    CUserSettingsInterface* mp_USInterface; ///< Container for the User settings
    ErrorMap_t m_ErrorsHash;          //!< To store Error ID and any arguments associated
    bool CheckData();
};

} // namespace DataManager

#endif // DATAMANAGER_SPECIALVERIFIERGROUPC_H
