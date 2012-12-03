/****************************************************************************/
/*! \file SWVersionListVerifier.cpp
 *
 *  \brief Implementation file for class CSWVersionListVerifier.
 *         This class verifies the data of the CSWVersion class
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-09-07
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

#include <QDebug>
#include <QDir>
#include "DataManager/Containers/SWVersions/Include/SWVersionListVerifier.h"


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
    // to store the error description
    QString ErrorDescription;
    // by default make the verification flag to true
    bool VerifiedData = true;
    // assign pointer to member variable
    mp_SWVL = static_cast<CSWVersionList*>(p_Configuration);

    // check the existence of the container
    if (mp_SWVL != NULL) {
        // all the details should be available in the SW_version.xml file
        if (mp_SWVL->GetSWReleaseDate().compare("") == 0) {
            VerifiedData = false;
        }

        if (mp_SWVL->GetSWReleaseVersion().compare("") == 0) {
            VerifiedData = false;
        }
        // iterate each item from the list and verify the data
        for (qint32 SWDetailsCount = 0; SWDetailsCount < mp_SWVL->GetNumberOfSWDetails(); SWDetailsCount++) {
            CSWDetails SWDetails;
            if (mp_SWVL->GetSWDetails(SWDetailsCount, SWDetails)) {
                if (SWDetails.GetSWDate().compare("") == 0) {
                    VerifiedData = false;
                }
                if (SWDetails.GetSWName().compare("") == 0) {
                    VerifiedData = false;
                }
                if (SWDetails.GetSWVersion().compare("") == 0) {
                    VerifiedData = false;
                }
            }
        }

        if (!VerifiedData) {
            // store error string with verification code
            //m_ErrorsList.append(QString("0,") + ErrorDescription);
        }
    }

    // return the boolean flag
    return VerifiedData;
}


/****************************************************************************/
/*!
 *  \brief  Get the synchronous object
 *
 *  \iparam p_ReadWriteLock = Lock for the threads
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CSWVersionListVerifier::GetSyncObject(QReadWriteLock* p_ReadWriteLock)
{
    p_ReadWriteLock = NULL;
    return true;
}


/****************************************************************************/
/*!
 *  \brief  Gets the last errors which is done by verifier
 *
 *  \return QStringList - List of the errors occured
 */
/****************************************************************************/
ErrorHash_t& CSWVersionListVerifier::GetErrors()
{
    // return the last error which is occured in the verifier
    return m_ErrorsHash;
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
void CSWVersionListVerifier::ResetLastErrors()
{
    m_ErrorsHash.clear();
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
bool CSWVersionListVerifier::IsLocalVerifier()
{
    return true;
}

}
