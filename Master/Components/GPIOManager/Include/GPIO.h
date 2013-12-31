/****************************************************************************/
/*! \file GPIO.h
 *
 *  \brief GPIO class definition.
 *         This class shall be used to create an instance of GPIO pin. The
 *         following features are allowed:
 *         -1)Setting direction
 *         -2)Setting Edge
 *         -3)Setting value for the GPIO and retrieving the value.
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
#include <unistd.h>
#include <QFile>

namespace GPIOManager {

static const QString SYSFS_GPIO_DIRECTORY =  "/sys/class/gpio"; ///< SYSFS directory path on target
static const quint32 DIRECTION_IS_OUTPUT  = 1;                  ///< Value for output direction of GPIO
static const quint32 DIRECTION_IS_INPUT   = 0;                  ///< Value for input direction of GPIO
static const quint32 DIRECTION_ERROR      = 2;                  ///< Error! couldn't retrieve direction
/****************************************************************************/
/**
 * \brief  This class shall be used to create an instance of GPIO pin. The
 *         following features are allowed:
 *         -1)Setting direction
 *         -2)Setting Edge
 *         -3)Setting value for the GPIO and retrieving the value.
 */
/****************************************************************************/
class GPIOPin: public QObject {
    friend class TestGPIOThreadController; //!< Unit test class
private:
    qint32  m_Fd;           //!< File descriptor
    qint32  m_CurrentValue; //!< Current Value of the pin
    /*! GPIO number calculation
     *  GPIO1_0 would be calculated as ( (1-1) * 32 + 0 )
     */
    quint32 m_PinNumber; //!< GPIO number
    quint32 m_Direction; //!< GPIO direction.
    QFile   *mp_ValueFile; //!< Sysfs GPIO value file

    void SetDirection(const quint32 Direction);
    quint32 GetDirection();
    void ExportGPIOPin();
    /****************************************************************************/
    /**
     * \brief  Disable copy and assignment operator
     */
    /****************************************************************************/
    Q_DISABLE_COPY(GPIOPin)

public:
    GPIOPin(quint32 PinNumber, quint32 Direction, QObject *p_Parent = NULL);
    ~GPIOPin();
    qint32 SetEdge(const char *p_Edge);
    qint32 SetValue(const qint32 Value);
    qint32 GetValue();
    qint32 Open();
    /****************************************************************************/
    /**
     * \brief  Returns file descriptor for GPIO value file
     * \return File handle
     */
    /****************************************************************************/
    qint32 GetGpioFd() { return m_Fd;}
    /****************************************************************************/
    /**
     * \brief  Close GPIO value file
     */
    /****************************************************************************/
    void CloseGPIOFd()
    {
        close(m_Fd);
        m_Fd = -1;
    }
};

} //End of namespace GPIOManager
#endif // GPIO_H
