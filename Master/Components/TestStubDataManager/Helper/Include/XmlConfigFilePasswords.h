/****************************************************************************/
/*! \file XmlConfigFilePasswords.h
 *
 *  \brief Definition file for class XmlConfigFilePasswords.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-08-17
 *  $Author:    $ J.Bugariu
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

#ifndef DATAMANAGEMENT_XMLCONFIGFILEPASSWORDS_H
#define DATAMANAGEMENT_XMLCONFIGFILEPASSWORDS_H

#include <DataManager/Helper/Include/XmlConfigFile.h>
#include <PasswordManager/Include/PasswordManager.h>

namespace DataManager {

/****************************************************************************/
/**
 * \brief Class for reading / writing XML based configuration file for passwords.
 *
 * The read serial number has to coincide with the expected one, or reading
 * from file will fail!
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class XmlConfigFilePasswords : public XmlConfigFile {
friend class TestXmlConfigFilePasswords;
private:
    QString             m_ExpectedSerialNumber; ///< This serial number is expected in the password file.
    /****************************************************************************/
    XmlConfigFilePasswords();                                                   ///< Not implemented.
    XmlConfigFilePasswords(const XmlConfigFilePasswords &);                     ///< Not implemented.
    const XmlConfigFilePasswords & operator = (const XmlConfigFilePasswords &); ///< Not implemented.
    /****************************************************************************/
    /**
     * \brief Read one password entry for format version1.
     *
     * \param[in, out]  rReader             xml stream reader.
     * \param[in, out]  rPasswordManager    Reference to password manager.
     */
    /****************************************************************************/
    void ReadPassword_V1(QXmlStreamReader &rReader, PasswordManager::CPasswordManager &rPasswordManager);
    /****************************************************************************/
    /**
     * \brief Read passwords for format version1.
     *
     * \param[in, out]  rReader             xml stream reader.
     * \param[in, out]  rPasswordManager    Reference to password manager.
     */
    /****************************************************************************/
    void ReadPasswords_V1(QXmlStreamReader &rReader, PasswordManager::CPasswordManager &rPasswordManager);
protected:
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]       ExpectedSerialNumber    The serial number expected in the password file.
     */
    /****************************************************************************/
    XmlConfigFilePasswords(const QString &ExpectedSerialNumber);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~XmlConfigFilePasswords() {
    }
    /****************************************************************************/
    /**
     * \brief Read passwords from file.
     *
     * Read passwords from file.
     *
     * \param[in]       FileName            File from which configuration is to be read.
     * \param[in, out]  rPasswordManager    Reference to password manager.
     */
    /****************************************************************************/
    void ReadPasswords(const QString &FileName, PasswordManager::CPasswordManager &rPasswordManager);
    /****************************************************************************/
    /**
     * \brief Write passwords to file.
     *
     * Write passwords to file.
     *
     * \param[in]   FileName            File from which configuration is to be read.
     * \param[in]   ThePasswordManager  The password manager.
     * \param[in]   SerialNumber        Serial number.
     */
    /****************************************************************************/
    void WritePasswords(const QString &FileName, const PasswordManager::CPasswordManager &ThePasswordManager, const QString &SerialNumber);
}; // end class XmlConfigFilePasswords

} // end namespace DataManager

#endif // DATAMANAGEMENT_XMLCONFIGFILEPASSWORDS_H
