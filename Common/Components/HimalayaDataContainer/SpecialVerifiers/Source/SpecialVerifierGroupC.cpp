/****************************************************************************/
/*! \file SpecialVerifierGroupC.cpp
 *
 *  \brief Implementation file for class CSpecialVerifierGroupC.
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


#include "HimalayaDataContainer/SpecialVerifiers/Include/SpecialVerifierGroupC.h"
#include "Global/Include/EventObject.h"
#include "HimalayaDataContainer/Helper/Include/HimalayaDataManagerEventCodes.h"

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CSpecialVerifierGroupC::CSpecialVerifierGroupC() : mp_DReagentList(NULL), mp_USInterface(NULL)
{
}

/****************************************************************************/
/*!
 *  \brief  Constructor
 *
 *  \iparam p_DataReagentList = Pointer to a Reagent list
 *  \iparam p_USInterface = Pointer to a User settings Interface
 */
/****************************************************************************/
CSpecialVerifierGroupC::CSpecialVerifierGroupC(CDataReagentList* p_DataReagentList, CUserSettingsInterface* p_USInterface)
    : mp_DReagentList(p_DataReagentList), mp_USInterface(p_USInterface)
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
bool CSpecialVerifierGroupC::VerifyData(CDataContainerBase* p_ContainerBase)
{
    bool Result = true;

    if (p_ContainerBase != NULL) {
        // prepare member pointers according to given container type
        // switch given container to temporary one for check
        // and switch back to real data after check again
        CDataContainerBase* p_TempDataItem = NULL;

        // check the containers consists of data or not if not please log it
        // which container is failed. This is for logger
        if (mp_DReagentList == NULL) {
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_GV_REAGENT_IS_NOT_INITIALIZED);
            qDebug() << "CSpecialVerifierGroupC::VerifyData failed. Reagent List is NULL";
            Result = false;
        }
        if (mp_USInterface == NULL) {
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_GV_USINTERFACE_IS_NOT_INITIALIZED);
            qDebug() << "CSpecialVerifierGroupC::VerifyData failed. Usersettings container is NULL";
            Result = false;
        }

        // copy all the data data to temporary variables
        if (p_ContainerBase->GetDataContainerType() == REAGENTS) {
            p_TempDataItem = mp_DReagentList;
            mp_DReagentList = static_cast<CDataReagentList*>(p_ContainerBase);
        }
        else if (p_ContainerBase->GetDataContainerType() == USERSETTINGS) {
            p_TempDataItem = mp_USInterface;
            mp_USInterface = static_cast<CUserSettingsInterface*>(p_ContainerBase);
        }
        else {
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_GV_INVALID_CONTAINER_TYPE);
            qDebug() << "CSpecialVerifierGroupC::VerifyData failed. Invalid conatiner type";
            return false;
        }

       /* if (Result) {
            if (!CheckData()) {
                Result = false;
            }
        }*/

        // revert all the temporary data to local variables
        if (p_ContainerBase->GetDataContainerType() == REAGENTS) {
            mp_DReagentList = static_cast<CDataReagentList*>(p_TempDataItem);
        }
        else if (p_ContainerBase->GetDataContainerType() == USERSETTINGS) {
            mp_USInterface = static_cast<CUserSettingsInterface*>(p_TempDataItem);
        }
    }
    else {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_GV_BASE_CONTAINER_IS_NOT_INITIALIZED);
        qDebug() << "CSpecialVerifierGroupC::VerifyData failed. Container base is not initialised";;
        Result = false;
    }

    return Result;

}

/****************************************************************************/
/*!
 *  \brief  Check the data with other containers (Like Reagent list,
 *          UserSettings Interface)
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CSpecialVerifierGroupC::CheckData()
{
    ListOfIDs_t ReagentIDs; ///< Store all reagent IDs

    bool Result = true;

    // check required to avoid lint warning
    if (mp_DReagentList != NULL && mp_USInterface != NULL) {
        // get the number of reagent from the list
        for (int ReagentCount = 0; ReagentCount < mp_DReagentList->GetNumberOfReagents(); ReagentCount++) {
            CReagent* Reagent = const_cast<CReagent*>(mp_DReagentList->GetReagent(ReagentCount));
            if (Reagent != NULL) {
                // store the all the reagent IDs
                ReagentIDs.append(Reagent->GetReagentID());
            }
        }

        // get the user setting to check the loader reagents
        CUserSettings* UserSettings = mp_USInterface->GetUserSettings();
        // check the user settings
        if(UserSettings != NULL) {
            // check user settind loader reagent1 exist in the reagent list
            if (!ReagentIDs.contains(UserSettings->GetValue("LOADER_REAGENT1"))) {
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_GV_REAGENTID_EXIST_IN_SETTINGS_NOT_IN_REAGENT_LIST,
                                                           Global::FmtArgs() << UserSettings->GetValue("LOADER_REAGENT1"));
//                qDebug() << "CSpecialVerifierGroupC::Reagent ID does not exist in Reagent List but it exists in User settings container - "
//                         << UserSettings->GetValue("LOADER_REAGENT1");
                Result = false;
            }
            // check user settind loader reagent1 exist in the reagent list
            if (!ReagentIDs.contains(UserSettings->GetValue("LOADER_REAGENT2"))) {
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_GV_REAGENTID_EXIST_IN_SETTINGS_NOT_IN_REAGENT_LIST,
                                                           Global::FmtArgs() << UserSettings->GetValue("LOADER_REAGENT2"));
//                qDebug() << "CSpecialVerifierGroupC::Reagent ID does not exist in Reagent List but it exists in User settings container - "
//                         << UserSettings->GetValue("LOADER_REAGENT2");
                Result = false;
            }
            // check user settind loader reagent1 exist in the reagent list
            if (!ReagentIDs.contains(UserSettings->GetValue("LOADER_REAGENT3"))) {
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_GV_REAGENTID_EXIST_IN_SETTINGS_NOT_IN_REAGENT_LIST,
                                                           Global::FmtArgs() << UserSettings->GetValue("LOADER_REAGENT3"));
//                qDebug() << "CSpecialVerifierGroupC::Reagent ID does not exist in Reagent List but it exists in User settings container - "
//                         << UserSettings->GetValue("LOADER_REAGENT3");
                Result = false;
            }
            // check user settind loader reagent1 exist in the reagent list
            if (!ReagentIDs.contains(UserSettings->GetValue("LOADER_REAGENT4"))) {
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_GV_REAGENTID_EXIST_IN_SETTINGS_NOT_IN_REAGENT_LIST,
                                                           Global::FmtArgs() << UserSettings->GetValue("LOADER_REAGENT4"));
//                qDebug() << "CSpecialVerifierGroupC::Reagent ID does not exist in Reagent List but it exists in User settings container - "
//                         << UserSettings->GetValue("LOADER_REAGENT4");
                Result = false;
            }
            // check user settind loader reagent1 exist in the reagent list
            if (!ReagentIDs.contains(UserSettings->GetValue("LOADER_REAGENT5"))) {
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_GV_REAGENTID_EXIST_IN_SETTINGS_NOT_IN_REAGENT_LIST,
                                                           Global::FmtArgs() << UserSettings->GetValue("LOADER_REAGENT5"));
//                qDebug() << "CSpecialVerifierGroupC::Reagent ID does not exist in Reagent List but it exists in User settings container - "
//                         << UserSettings->GetValue("LOADER_REAGENT5");
                Result = false;
            }
        }
    }
    // if everything goes well then Special verifier verified all the data
    return Result;
}


}  // namespace DataManager
