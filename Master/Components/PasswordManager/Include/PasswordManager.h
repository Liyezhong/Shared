/****************************************************************************/
/*! \file PasswordManager.h
 *
 *  \brief Definition file for class CPasswordManager.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-08-19, 2012-10-30
 *  $Author:    $ J.Bugariu, Raju
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

#ifndef PASSWORDMANAGER_CPASSWORDMANAGER_H
#define PASSWORDMANAGER_CPASSWORDMANAGER_H

#include <QString>
#include <QHash>
#include <QCryptographicHash>

namespace PasswordManager {

typedef QHashIterator<QString, QString> tPasswordIterator;  //!< typedef for password iterator

/****************************************************************************/
/**
 * \brief This class encapsulates the functionality for a password manager.
 *
 * Add and check passwords using md5 hashes.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CPasswordManager {
friend class TestPasswordManager;
private:
    QHash<QString, QString>     m_Passwords;            //!< List of names with password hashes.
    QString                     m_NewPassword;          //!< New password for the current user
    bool                        m_FallbackPassword;     //!< Fallback password flag
    /****************************************************************************/
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CPasswordManager)
    /****************************************************************************/
    /**
     * \brief Compute hash from a value.
     *
     * The computed hash is converted to upper string.
     *
     * \iparam   Value   String to compute a hash from.
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
     * \iparam   Password    The password.
     * \return      true if format iscorrect.
     */
    /****************************************************************************/
    static bool CheckPasswordFormat(const QString &Password);

    /****************************************************************************/
    /**
     * \brief Compute the fallback password value.
     *
     * \return  calculated the password number.
     */
    /****************************************************************************/
    long ComputeFallbackPassword();

    /****************************************************************************/
    /**
     * \brief Set the fall back password flag.
     *
     * \iparam   Value  Fall back pwd flag value.
     */
    /****************************************************************************/
    inline void SetFallbackPasswordFlag(const bool &Value) {
        m_FallbackPassword = Value;
    }

protected:
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     */
    /****************************************************************************/
    CPasswordManager();
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    inline ~CPasswordManager() {
    }

    /****************************************************************************/
    /**
     * \brief Get the fall back password flag.
     *
     * \return On successful (True) or not (false).
     */
    /****************************************************************************/
    inline bool GetFallbackPasswordFlag() {
        return m_FallbackPassword;
    }

    /****************************************************************************/
    /**
     * \brief Set new password for the current user.
     *
     * The password will be compread against to the confirm password.
     *
     * \iparam   Password    The password.
     */
    /****************************************************************************/
    inline void SetNewPassword(const QString &Password) {
        m_NewPassword = Password;
    }

    /****************************************************************************/
    /**
     * \brief Get new password for the current user.
     *
     * \return   password of the current user.
     */
    /****************************************************************************/
    inline QString GetNewPassword() {
        return m_NewPassword;
    }

    /****************************************************************************/
    /**
     * \brief Set a name - password pair.
     *
     * The hash of the password string is computed and saved, not the password itself.
     * If the password does not meet the requirements an exception will be thrown.
     *
     * \iparam   Name        The name.
     * \iparam   Password    The password.
     */
    /****************************************************************************/
    void SetPassword(const QString &Name, const QString &Password);
    /****************************************************************************/
    /**
     * \brief Set a name - password pair.
     *
     * The password is already a hash. We can not check it any more.
     *
     * \iparam   Name    The name.
     * \iparam   Hash    The password hash.
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
     * \iparam   Name        The name.
     * \iparam   Password    The password.
     * \return                  True on success.
     */
    /****************************************************************************/
    bool CheckPassword(const QString &Name, const QString &Password);
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

    /****************************************************************************/
    /**
     * \brief Check the fallback pasword against with entered password.
     *
     *
     * \iparam   Password    The password.
     *
     * \return                  True on success.
     */
    /****************************************************************************/
    bool CheckFallbackPassword(const long &Password);

    /****************************************************************************/
    /**
     * \brief Check the service pasword against with entered password.
     *
     *
     * \iparam   Password    The password.
     * \iparam   DeviceName  Name of the device.
     *
     * \return                  True on success.
     */
    /****************************************************************************/
    bool ServiceAuthentication(const QString &Password, const QString &DeviceName);


}; // end class CPasswordManager

} // end namespace PasswordManager

#endif // PASSWORDMANAGER_CPASSWORDMANAGER_H
