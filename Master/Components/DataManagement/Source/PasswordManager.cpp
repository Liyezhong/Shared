/****************************************************************************/
/*! \file PasswordManager.cpp
 *
 *  \brief Implementation file for class PasswordManager.
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

#include <DataManagement/Include/PasswordManager.h>
#include <DataManagement/Include/DataManagementEventCodes.h>
#include <Global/Include/Exception.h>

namespace DataManager {

/****************************************************************************/
PasswordManager::PasswordManager(const QString &MasterPasswordHash) :
    m_MasterPasswordHash(MasterPasswordHash)
{
}

/****************************************************************************/
QString PasswordManager::ComputeHash(const QString &Value) {
    // fill up to 16 characters repeating Value as much as neccessary
    QString Str = Value;
    if(Value.size() != 0) {
        while(Str.size() < 16) {
            Str += Value;
        }
        Str = Str.left(16);
    }
    return QCryptographicHash::hash(Str.toUtf8(), QCryptographicHash::Md5).toHex().toUpper();
}

/****************************************************************************/
bool PasswordManager::CheckPasswordFormat(const QString &Password) {
    // check for password length
    if((Password.size() < 4) || (Password.size() > 16)) {
        return false;
    }
    // check for printable ASCII >= 0x20 && < 0x7F
    for(int idx = 0; idx < Password.size(); idx++) {
        QChar c = Password[idx];
        if((c < 0x20) || (c >= 0x7F)) {
            // not a printable ASCII
            return false;
        }
    }
    return true;
}

/****************************************************************************/
void PasswordManager::SetPassword(const QString &Name, const QString &Password) {
    if(!CheckPasswordFormat(Password)) {
        THROW(EVENT_DATAMANAGER_ERROR_PASSWORD_FORMAT);
    }
    // checks passed. compute hash and insert password.
    m_Passwords.insert(Name, ComputeHash(Password));
}

/****************************************************************************/
void PasswordManager::SetPasswordHash(const QString &Name, const QString &Hash) {
    m_Passwords.insert(Name, Hash.toUpper());
}

/****************************************************************************/
void PasswordManager::Clear() {
    m_Passwords.clear();
}

/****************************************************************************/
bool PasswordManager::CheckPassword(const QString &Name, const QString &Password) const {
    // check if default master password
    if((Name == "Administrator") && (ComputeHash(Password) == m_MasterPasswordHash)) {
        // default master password
        return true;
    }
    // it is not the default master pasword
    // check if name in list of known names
    if(!m_Passwords.contains(Name)) {
        // name not in list.
        return false;
    }
    // chack password hash
    return m_Passwords.value(Name).toUpper() == ComputeHash(Password);
}

} // end namespace DataManager
