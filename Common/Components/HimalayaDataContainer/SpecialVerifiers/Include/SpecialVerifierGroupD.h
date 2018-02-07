
/****************************************************************************/
/*! \file SpecialVerifierGroupD.h
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
#ifndef DATAMANAGER_SPECIALVERIFIERGROUPD_H
#define DATAMANAGER_SPECIALVERIFIERGROUPD_H



#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Include/DashboardDataStationList.h"

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  This class implements Special verifier Group D and compares the
 *          data with respect to other containers
 *
 */
/****************************************************************************/
// need to add error interface to special verifier
class CSpecialVerifierGroupD : public IVerifierInterface
{
public:

    // instead of sending all the arguments please put all class objects in a Hash table with a Key
    // so that, in feature if it adds up new arguments and function will not be changed.
    CSpecialVerifierGroupD(CDashboardDataStationList* p_StationList);

    bool VerifyData(CDataContainerBase* p_ContainerBase);

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
    CSpecialVerifierGroupD();
    CSpecialVerifierGroupD(const CSpecialVerifierGroupD&); ///< Not implemented

    CDashboardDataStationList* mp_DStationList; ///< Container for the Station list
    //CDataStationMatrix* mp_StationMatrix; ///< Container for the Station grid
    ErrorMap_t m_ErrorsHash;          //!< To store Error ID and any arguments associated
    bool CheckData();
};

} // namespace DataManager

#endif // DATAMANAGER_SPECIALVERIFIERGROUPD_H
