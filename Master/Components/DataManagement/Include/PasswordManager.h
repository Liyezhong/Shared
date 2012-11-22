/****************************************************************************/
/*! \file PasswordManager.h
 *
 *  \brief Definition file for class PasswordManager.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-08-19
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

#ifndef DATAMANAGEMENT_PASSWORDMANAGER_H
#define DATAMANAGEMENT_PASSWORDMANAGER_H

#include <QString>
#include <QHash>
#include <QCryptographicHash>

namespace DataManager {

typedef QHashIterator<QString, QString> tPasswordIterator;  ///< typedef for password iterator

/****************************************************************************/
/**
 * \brief This class encapsulates the functionality for a password manager.
 *
 * Add and check passwords using md5 hashes.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class PasswordManager {
friend class TestPasswordManager;
private:
    QHash<QString, QString>     m_Passwords;            ///< List of names with password hashes.
    QString                     m_MasterPasswordHash;   ///< Hash for master password.
    /****************************************************************************/
    PasswordManager();                                              ///< Not implemented.
    PasswordManager(const PasswordManager &);                       ///< Not implemented.
    const PasswordManager & operator = (const PasswordManager &);   ///< Not implemented.
    /****************************************************************************/
    /**
     * \brief Compute hash from a value.
     *
     * The computed hash is converted to upper string.
     *
     * \param[in]   Value   String to compute a hash from.
     * \return              Hash as hex string.
     */
    /****************************************************************************/
    static QString ComputeHash(const QString &Value);
    /****************************************************************************/
    /**
     * \brief Set a name - password pair.
     *
     * Check if password format is OK:
     * The password has to be at least 4 and at most 16 characters long.
     *
     * \param[in]   Password    The password.
     */
    /****************************************************************************/
    static bool CheckPasswordFormat(const QString &Password);
protected:
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   MasterPasswordHash  hash of master password.
     */
    /****************************************************************************/
    PasswordManager(const QString &MasterPasswordHash);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    inline ~PasswordManager() {
    }
    /****************************************************************************/
    /**
     * \brief Set a name - password pair.
     *
     * The hash of the password string is computed and saved, not the password itself.
     * If the password does not meet the requirements an exception will be thrown.
     *
     * \param[in]   Name        The name.
     * \param[in]   Password    The password.
     */
    /****************************************************************************/
    void SetPassword(const QString &Name, const QString &Password);
    /****************************************************************************/
    /**
     * \brief Set a name - password pair.
     *
     * The password is already a hash. We can not check it any more.
     *
     * \param[in]   Name    The name.
     * \param[in]   Hash    The password hash.
     */
    /****************************************************************************/
    void SetPasswordHash(const QString &Name, const QString &Hash);
    /****************************************************************************/
    /**
     * \brief Clear all saved names and passwords.
     */
    /****************************************************************************/
    void Clear();
    /****************************************************************************/
    /**
     * \brief Check if a password fits.
     *
     * First the built in Administrator password is checked. Then the hash of the
     * provided password is computed and verified.
     *
     * \param[in]   Name        The name.
     * \param[in]   Password    The password.
     * \return                  True on success.
     */
    /****************************************************************************/
    bool CheckPassword(const QString &Name, const QString &Password) const;
    /****************************************************************************/
    /**
     * \brief Iterator for the passwords.
     *
     * \return  A new iterator.
     */
    /****************************************************************************/
    inline tPasswordIterator Iterator() const {
        return tPasswordIterator(m_Passwords);
    }
}; // end class PasswordManager

} // end namespace DataManager

#endif // DATAMANAGEMENT_PASSWORDMANAGER_H
