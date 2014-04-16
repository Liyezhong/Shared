/****************************************************************************/
/*! \file ServicePassword.h
 *
 *  \brief Definition file for class CServicePassword.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-11-15
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
#ifndef PASSWORDMANAGER_CSERVICEPASSWORD_H
#define PASSWORDMANAGER_CSERVICEPASSWORD_H

#include <QString>
#include <QHash>
#include <QCryptographicHash>
#include <QXmlStreamReader>
#include <QProcess>
#include <QFile>
#include <QDir>

namespace PasswordManager {
///< Authentication errors or infos
enum ServiceUserAuthentication {
    AUTHENTICATION_VALID,    ///< Authentication is valid
    NO_SERVICE_KEY_FILE,     ///< No service key file is found
    CHECKSUM_NOT_MATCHING,   ///< Check sum is not matching
    HASH_NOT_MATCHING,       ///< Hash is not matching
    NO_DEVICE_TAG,           ///< Device tag not exists in service key file
    BASIC_TAG_NOT_MATCHING,  ///< Basic tag not matching or might not available
    DATE_EXPIRED             ///< Date is expired for the service key
};

/****************************************************************************/
/**
 * \brief This class encapsulates the functionality for service user.
 *
 * Add and check passwords using md5 hashes.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CServicePassword {    
    friend class TestServicePassword; ///< this is to test the private functions of the class in unit test project
private:
    QByteArray m_ServiceFileContent; ///< To store the servicekey.xml file information
    QString m_ServicePin;            ///< To store the service PIN
    QString m_DeviceName;            ///< To store the device name
    ServiceUserAuthentication m_Authentication;   ///< To store authentication value

    CServicePassword();                          ///< Not implemented
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CServicePassword)

    /****************************************************************************/
    /**
     * \brief Read the service key file from the mounted device.
     *
     * \return On successful (true) or not (false)
     */
    /****************************************************************************/
    bool ReadTheServiceKeyFile();

    /****************************************************************************/
    /**
     * \brief Compute the check sum of the file
     *
     * \return check sum value
     */
    /****************************************************************************/
    QString ComputeCheckSum();

    /****************************************************************************/
    /**
     * \brief Compute the hash value.
     *
     * \return On successful (true) or not (false)
     */
    /****************************************************************************/
    QString ComputeHash();

    /****************************************************************************/
    /**
     * \brief Compare the calculated check sum against the check sum present in the file.
     *
     * \return On successful (true) or not (false)
     */
    /****************************************************************************/
    bool CompareTheCheckSum();

    /****************************************************************************/
    /**
     * \brief Read the device name tag from the file
     *
     * \return On successful (true) or not (false)
     */
    /****************************************************************************/
    bool ReadDeviceNameTagsExistence();

    /****************************************************************************/
    /**
     * \brief Compare the calculated date against the date present in the file.
     *
     * \return On successful (true) or not (false)
     */
    /****************************************************************************/
    bool CompareDate();

    /****************************************************************************/
    /**
     * \brief Compare the calculated hash against the hash present in the file.
     *
     * \return On successful (true) or not (false)
     */
    /****************************************************************************/
    bool CompareHash();    

public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam PinValue      Pin number
     * \iparam DeviceName    Name of the device
     */
    /****************************************************************************/
    CServicePassword(const QString & PinValue, const QString &DeviceName);

    /****************************************************************************/
    /**
     * \brief Validates the authentication.
     *        This compares the PIN number with the existing data in the Servicekey.xml
     *        file     
     *
     * \return On successful (true) or not (false)
     */
    /****************************************************************************/
    bool ValidateAuthentication();

    /****************************************************************************/
    /**
     * \brief Read the service ID from the key file.
     *
     * \return service ID number
     */
    /****************************************************************************/
    QString ReadServiceID();

    /****************************************************************************/
    /**
     * \brief Get the error value if the authentication is not valid.
     *
     * \return Authentication success or error
     */
    /****************************************************************************/
    PasswordManager::ServiceUserAuthentication GetAuthenticationError() {
        return m_Authentication;
    }

}; // end class of CServicePassword
} // end name space of PasswordManager
#endif // PASSWORDMANAGER_CSERVICEPASSWORD_H
