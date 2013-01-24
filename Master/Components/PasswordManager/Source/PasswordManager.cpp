/****************************************************************************/
/*! \file PasswordManager.cpp
 *
 *  \brief Implementation file for class CPasswordManager.
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

#include <PasswordManager/Include/PasswordManager.h>
#include <DataManager/Helper/Include/DataManagerEventCodes.h>
#include <Global/Include/Exception.h>
#include <PasswordManager/Include/ServicePassword.h>
#include <Global/Include/EventObject.h>

namespace PasswordManager {

/****************************************************************************/
CPasswordManager::CPasswordManager(const QString &MasterPasswordHash) :
    m_MasterPasswordHash(MasterPasswordHash),
    m_FallbackPassword(false)
{
}

/****************************************************************************/
QString CPasswordManager::ComputeHash(const QString &Value) {
    // fill up to 16 characters repeating Value as much as neccessary
    QString Str = Value;
    if(Value.size() != 0) {
        while(Str.size() < 16) {
            Str += Value;
        }
        Str = Str.left(16);
    }
    qDebug() << "Hash value ::  " << QCryptographicHash::hash(Str.toUtf8(), QCryptographicHash::Md5).toHex().toUpper();
    return QCryptographicHash::hash(Str.toUtf8(), QCryptographicHash::Md5).toHex().toUpper();
}

/****************************************************************************/
bool CPasswordManager::CheckPasswordFormat(const QString &Password) {
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
void CPasswordManager::SetPassword(const QString &Name, const QString &Password) {
    if(!CheckPasswordFormat(Password)) {
        LOGANDTHROW(DataManager::EVENT_DM_ERROR_PASSWORD_FORMAT);
    }    
    // checks passed. compute hash and insert password.
    m_Passwords.insert(Name, ComputeHash(Password));
}

/****************************************************************************/
void CPasswordManager::SetPasswordHash(const QString &Name, const QString &Hash) {
    m_Passwords.insert(Name, Hash.toUpper());
}

/****************************************************************************/
void CPasswordManager::Clear() {
    m_Passwords.clear();
}

/****************************************************************************/
bool CPasswordManager::CheckPassword(const QString &Name, const QString &Password)  {
    // reset the fallback password flag value
    SetFallbackPasswordFlag(false);
    // check if default master password
    if((Name == "Administrator") && ((ComputeHash(Password) == m_MasterPasswordHash)
                                     || (CheckFallbackPassword(Password.toLong())))) {
        // default master password
        return true;
    }
    // it is not the default master pasword
    // check if name in list of known names
    if(!m_Passwords.contains(Name)) {
        // name not in list.
        return false;
    }
    // check password hash
    return m_Passwords.value(Name).toUpper() == ComputeHash(Password);
}

/****************************************************************************/
bool CPasswordManager::CheckFallbackPassword(const long &Password)  {
    // check if fallback password matches
    if(ComputeFallbackPassword() == Password) {
        SetFallbackPasswordFlag(true);
        // default master password
        return true;
    }

    // authentication failed
    return false;

}

/****************************************************************************/
long CPasswordManager::ComputeFallbackPassword() {

    // get the current date and time
    QDateTime Now = Global::AdjustedTime::Instance().GetCurrentDateTime();

    // get the year, month and day of the today's date
    qint32 Year = Now.date().year();
    qint32 Month = Now.date().month() + 1;
    qint32 Day = Now.date().day();

    // chack password hash
    return (2 * Year * Year + 3 * Month * Month + Day);
}

/****************************************************************************/
bool CPasswordManager::ServiceAuthentication(const QString &Password, const QString &DeviceName) {

    CServicePassword ServiceUser(Password, DeviceName);

    return ServiceUser.ValidateAuthentication();
}


} // end namespace DataManager
