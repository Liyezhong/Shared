/****************************************************************************/
/*! \file SpecialVerifierGroupD.cpp
 *
 *  \brief Implementation file for class CSpecialVerifierGroupD.
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2011-08-01, 2012-05-03
 *  $Author:    $ F. Toth, Raju
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



#include "HimalayaDataContainer/SpecialVerifiers/Include/SpecialVerifierGroupD.h"
#include "Global/Include/EventObject.h"
#include "HimalayaDataContainer/Helper/Include/HimalayaDataManagerEventCodes.h"

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CSpecialVerifierGroupD::CSpecialVerifierGroupD() : mp_DStationList(NULL)
{
}

/****************************************************************************/
/*!
 *  \brief  Constructor
 *
 *  \iparam p_DataStationList
 */
/****************************************************************************/
CSpecialVerifierGroupD::CSpecialVerifierGroupD(CDashboardDataStationList* p_DataStationList)
    : mp_DStationList(p_DataStationList)
{
}

/****************************************************************************/
/*!
 *  \brief  Verifies the data
 *
 *  \iparam p_ContainerBase = Pointer to a Container base
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CSpecialVerifierGroupD::VerifyData(CDataContainerBase* p_ContainerBase)
{
    bool Result = true;

    if (p_ContainerBase != NULL) {
        // prepare member pointers according to given container type
        // switch given container to temporary one for check
        // and switch back to real data after check again
        CDataContainerBase* p_TempDataItem = NULL;

        // check the containers consists of data or not if not please log it
        // which container is failed. This is for logger
        if (mp_DStationList == NULL) {
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_GV_STATION_IS_NOT_INITIALIZED);
            qDebug() << "CSpecialVerifierGroupD::VerifyData failed. Station List is NULL";
            return false;
        }


        // copy all the data data to temporary variables
        if (p_ContainerBase->GetDataContainerType() == STATIONS) {
            p_TempDataItem = mp_DStationList;
            mp_DStationList = static_cast<CDashboardDataStationList*>(p_ContainerBase);
        }
        else {
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_GV_INVALID_CONTAINER_TYPE);
            qDebug() << "CSpecialVerifierGroupD::VerifyData failed. Invalid conatiner type";
            return false;
        }

        if (Result) {
            if (!CheckData()) {
                Result = false;
            }
        }

        // revert all the temporary data to local variables
        if (p_ContainerBase->GetDataContainerType() == STATIONS) {
            mp_DStationList = static_cast<CDashboardDataStationList*>(p_TempDataItem);
        }

    }
    else {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_GV_BASE_CONTAINER_IS_NOT_INITIALIZED);
        qDebug() << "CSpecialVerifierGroupD::VerifyData failed. Container base is not initialised";;
        Result = false;
    }

    return Result;

}


/****************************************************************************/
/*!
 *  \brief  Checks the data with other containers (Like Station grid,
 *          Station list)
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CSpecialVerifierGroupD::CheckData()
{
    bool Result = true;

    // check required to avoid lint warning
    //if (mp_DStationList != NULL && mp_StationMatrix != NULL) {
        /*! \todo need to put more test cases*/
        //QStringList StationIDs;
        // row counter
       /* for(int Row = 0; Row < mp_StationMatrix->GetRows(); Row++) {
            // column counter
            for(int Column = 0; Column < mp_StationMatrix->GetColumns(); Column++) {
                // retrieve the station item from the array
                CDataStationItem* p_StationItem = mp_StationMatrix->GetStationItem(Row, Column);
                if (p_StationItem != NULL) {
                    // store the station IDs in the string list
                    StationIDs.append(p_StationItem->GetStationID());
                }
            }
        }
        // Iterate all the stations and check whether Station ID exists in Station Grid or not
        for(int StationCount = 0; StationCount < mp_DStationList->GetNumberOfStations(); StationCount++) {
            CStation* p_Station = const_cast<CStation*>(mp_DStationList->GetStation(StationCount));
            // check station exists or not
            if (p_Station != NULL) {
                if(!StationIDs.contains(p_Station->GetStationID())) {
                    Global::EventObject::Instance().RaiseEvent(EVENT_DM_GV_STATIONID_EXIST_IN_STATION_NOT_IN_STATIONMATRIX,
                                                               Global::FmtArgs() << p_Station->GetStationID());
//                    qDebug() << "CSpecialVerifierGroupD::CheckData failed. Station ID does not exist in Station Matrix but it exists in Station List - "
//                             << p_Station->GetStationID();
                    Result = false;
                }
            }
        }

    }    */

    // check the containers consists of data or not if not please log it
    // which container is failed. This is for logger
    // if everything goes well then Special verifier verified all the data
    return Result;
}

}  // namespace DataManager


