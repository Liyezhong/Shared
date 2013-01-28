/****************************************************************************/
/*! \file GPIO.h
 *
 *  \brief GPIO class definition
 *
 *   $Version: $ 0.1
 *   $Date:    $ 01.01.2013
 *   $Author:  $ N.Kamath
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
#ifndef GPIO_H
#define GPIO_H
#include <QObject>

namespace SoftSwitchManager {
/****************************************************************************/
/**
 * \brief  This class shall be used to create an instance of GPIO pin. The
 *         following features are allowed:
 *         1)Setting direction
 *         2)Setting Edge
 *         3)Setting value for the GPIO and retrieving the value.
 */
/****************************************************************************/
class GPIOPin: public QObject {
private:
    qint32  m_Fd;           //!< File descriptor
    qint32  m_CurrentValue; //!< Current Value of the pin
    /*! GPIO number calculation
     *  GPIO1_0 would be calculated as ( (1-1) * 32 + 0 )
     */
    quint32 m_PinNumber; //!< GPIO number
    bool    m_Direction; //!< GPIO direction - In/ Out

    void SetDirection(const bool Direction);

    void ExportGPIOPin();

public:
    GPIOPin(quint32 PinNumber, quint32 Direction, QObject *p_Parent = NULL);
    ~GPIOPin();
    Q_DISABLE_COPY(GPIOPin) //!< Disable copy
    qint32 SetEdge(const char *p_Edge);
    qint32 SetValue(const qint32 Value);
    qint32 GetValue();
    qint32 GetGpioFd() { return m_Fd;}
    qint32 Open();
};

} //End of namespace SoftSwitchManager
#endif // GPIO_H
