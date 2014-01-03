/****************************************************************************/
/*! \file Platform/Master/Components/DataManager/Containers/SWVersions/Source/SWVersionListVerifier.cpp
 *
 *  \brief Implementation file for class CSWVersionListVerifier.
 *         This class verifies the data of the CSWVersion class
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-09-07
 *  $Author:    $ Raju, Ramya GJ
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
#include <QDir>
#include "DataManager/Containers/SWVersions/Include/SWVersionListVerifier.h"
#include <Global/Include/EventObject.h>
#include "DataManager/Helper/Include/DataManagerEventCodes.h"
#include "Global/Include/GlobalEventCodes.h"
#include "Global/Include/Utils.h"

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  Constructor
 */
/****************************************************************************/
CSWVersionListVerifier::CSWVersionListVerifier() : mp_SWVL(NULL)
{

}

/****************************************************************************/
/*!
 *  \brief  Verifies the CSWVersion class data
 *
 *  \iparam p_Configuration = SW version data
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CSWVersionListVerifier::VerifyData(CDataContainerBase* p_Configuration)
{
    // by default make the verification flag to true
    bool VerifiedData = true;

    try {
        // check the existence of the container
        CHECKPTR(p_Configuration);
        // assign pointer to member variable
        mp_SWVL = static_cast<CSWVersionList*>(p_Configuration);

        CHECKPTR(mp_SWVL);

        // all the details should be available in the SW_version.xml file
        if (mp_SWVL->GetSWReleaseDate().compare("") == 0) {
            qDebug() <<"The release date/version is empty in SW_Version.xml";

            m_ErrorMap.insert(EVENT_SWVERSION_RELEASE_DATE_VERION_EMPTY,
                              Global::tTranslatableStringList() << "");
            Global::EventObject::Instance().RaiseEvent(EVENT_SWVERSION_RELEASE_DATE_VERION_EMPTY,
                                                       Global::tTranslatableStringList() << "",
                                                       true,
                                                       Global::GUI_MSG_BOX);
            VerifiedData = false;
        }

        if (mp_SWVL->GetSWReleaseVersion().compare("") == 0) {
            qDebug() <<"The release date/version is empty in SW_Version.xml";

            m_ErrorMap.insert(EVENT_SWVERSION_RELEASE_DATE_VERION_EMPTY,
                              Global::tTranslatableStringList() << "");
            Global::EventObject::Instance().RaiseEvent(EVENT_SWVERSION_RELEASE_DATE_VERION_EMPTY,
                                                       Global::tTranslatableStringList() << "",
                                                       true,
                                                       Global::GUI_MSG_BOX);
            VerifiedData = false;
        }
        // iterate each item from the list and verify the data
        for (qint32 SWDetailsCount = 0; SWDetailsCount < mp_SWVL->GetNumberOfSWDetails(); SWDetailsCount++) {
            CSWDetails SWDetails;
            if (mp_SWVL->GetSWDetails(SWDetailsCount, SWDetails)) {
                if (SWDetails.GetSWDate().compare("") == 0) {
                    qDebug() <<"The release date/version of component ### is empty in SW_Version.xml"
                               << SWDetails.GetSWName();

                    m_ErrorMap.insert(EVENT_SWVERSION_SW_DATE_VERION_EMPTY,
                                      Global::tTranslatableStringList() << SWDetails.GetSWName());
                    Global::EventObject::Instance().RaiseEvent(EVENT_SWVERSION_SW_DATE_VERION_EMPTY,
                                        Global::tTranslatableStringList() << SWDetails.GetSWName(),
                                                               true,
                                                               Global::GUI_MSG_BOX);
                    VerifiedData = false;
                }
                if (SWDetails.GetSWName().compare("") == 0) {
                    qDebug() <<"The SW component name is empty in SW_Version.xml";

                    m_ErrorMap.insert(EVENT_SWVERSION_SW_NAME_EMPTY,
                                      Global::tTranslatableStringList() << "");
                    Global::EventObject::Instance().RaiseEvent(EVENT_SWVERSION_SW_NAME_EMPTY,
                                                               Global::tTranslatableStringList() << "",
                                                               true,
                                                               Global::GUI_MSG_BOX);
                    VerifiedData = false;
                }
                if (SWDetails.GetSWVersion().compare("") == 0) {
                    qDebug() <<"The release date/version of component ### is empty in SW_Version.xml"
                               << SWDetails.GetSWName();

                    m_ErrorMap.insert(EVENT_SWVERSION_SW_DATE_VERION_EMPTY,
                                      Global::tTranslatableStringList() << SWDetails.GetSWName());
                    Global::EventObject::Instance().RaiseEvent(EVENT_SWVERSION_SW_DATE_VERION_EMPTY,
                                        Global::tTranslatableStringList() << SWDetails.GetSWName(),
                                                               true,
                                                               Global::GUI_MSG_BOX);
                    VerifiedData = false;
                }
            }
        }

        // return the boolean flag
        return VerifiedData;
    }
    CATCHALL();

    return false;
}


/****************************************************************************/
/*!
 *  \brief  Gets the last errors which is done by verifier
 *
 *  \return QStringList - List of the errors occured
 */
/****************************************************************************/
ErrorMap_t& CSWVersionListVerifier::GetErrors()
{
    // return the last error which is occured in the verifier
    return m_ErrorMap;
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
void CSWVersionListVerifier::ResetErrors()
{
    m_ErrorMap.clear();
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 *
 *  \return verification status , true or false
 */
/****************************************************************************/
bool CSWVersionListVerifier::IsLocalVerifier()
{
    return true;
}

}
