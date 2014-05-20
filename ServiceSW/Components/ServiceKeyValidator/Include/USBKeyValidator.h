/****************************************************************************/
/*! \file USBKeyValidator.h
 *
 *  \brief USBKeyValidator class definition
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-04-09
 *   $Author:  $ Soumya. D
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

#ifndef SERVICEKEYVALIDATOR_USBKEYVALIDATOR_H
#define SERVICEKEYVALIDATOR_USBKEYVALIDATOR_H

#include <QWidget>
#include <../Include/PlatformServiceDefines.h>
#include <PasswordManager/Include/ServicePassword.h>
#include <KeyBoard/Include/KeyBoard.h>
#include "MainMenu/Include/MessageDlg.h"
#include <QMutex>
#include <QMutexLocker>


namespace ServiceKeyValidator {

/****************************************************************************/
/**
 * \brief This class handles the USB Key Validation.
 */
/****************************************************************************/
class CUSBKeyValidator : public QWidget
{
    Q_OBJECT

public:
    CUSBKeyValidator(QString DeviceName);
    CUSBKeyValidator(QString DeviceName, bool SystemLogOff);
    ~CUSBKeyValidator();

    void SetEnteredPin(QString PinEntered) {
        m_EnteredPin = PinEntered;
    }

    QString GetEnteredPin() {
        return m_EnteredPin;
    }


    bool GetSystemLogOff() {
        return m_SystemLogOff;
    }  

    QString GetDeviceName() {
        return m_DeviceName;
    }
    void HideKeyBoard();
private:
    QString m_DeviceName;                               //!< Name of the device
    QString m_EnteredPin;                               //!< Service or Manufacturing Pin
    KeyBoard::CKeyBoard *mp_KeyBoardWidget;             //!< KeyBoard widget    
    MainMenu::CMessageDlg *mp_MessageDlg;               //!< Information dialog    
    QString m_CurrentUser;                              //!< Current user
    bool m_SystemLogOff;                                //!< Stores System log off - true/false
    QString m_ManufacturingKey;                         //!< Name manufacturing key starts with

    void ConnectKeyBoardSignalSlots();
    void DisconnectKeyBoardSignalSlots();

signals:
    void SetSoftwareMode(PlatformService::SoftwareModeType_t, QString);

private slots:
    void OnOkClicked(QString EnteredString);
    void OnESCClicked();       

public slots:
    void DisplayKeyBoard();
    void SetDeviceName(QString DeviceName);

};

}   // end of namespace ServiceKeyValidator


#endif // SERVICEKEYVALIDATOR_USBKEYVALIDATOR_H
