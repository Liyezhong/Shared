/****************************************************************************/
/*! \file GPIO.cpp
 *
 *  \brief GPIO class implmentation
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
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <QDebug>
#include <GPIOManager/Include/GPIO.h>
#include <GPIOManager/Include/GPIOManagerEventCodes.h>
#include <Global/Include/EventObject.h>
#include <Global/Include/GlobalEventCodes.h>

namespace GPIOManager {

/****************************************************************************/
/*!
 *  \brief    Constructor
 *  \iparam PinNumber - Gpio pin number
 *  \iparam Direction - 1 for o/p, 0 (Zero) for i/p
 *  \iparam p_Parent  - Parent object
 *  \return
 *
 */
/****************************************************************************/
GPIOPin::GPIOPin(quint32 PinNumber, quint32 Direction, QObject *p_Parent)
  :QObject(p_Parent)
  , m_Fd(-1)
  , m_CurrentValue(-1)
  , m_PinNumber(PinNumber)
  , m_Direction(Direction)
  , mp_ValueFile(NULL)
{
#if defined(__arm__) || defined(__TARGET_ARCH_ARM) || defined(_M_ARM)
    ExportGPIOPin();
    SetDirection(m_Direction);
    mp_ValueFile = new QFile(SYSFS_GPIO_DIRECTORY + QString("/gpio%1/value").arg(m_PinNumber), this);
    Open();
#else
    mp_ValueFile = new QFile(SYSFS_GPIO_DIRECTORY + QString("/gpio%1/value").arg(m_PinNumber), this);
#endif
}

/****************************************************************************/
/*!
 *  \brief  Desctructor
 *
 */
/****************************************************************************/
GPIOPin::~GPIOPin()
{
}
/****************************************************************************/
/*!
 *  \brief Exports the GPIO pin in sysfs directory
 */
/****************************************************************************/
void GPIOPin::ExportGPIOPin()
{
    QFile GPIOFile(SYSFS_GPIO_DIRECTORY + QString("/gpio%1").arg(m_PinNumber));
    if (!GPIOFile.exists()) { //Export only if file doesn't exist.
        QFile ExportFile(SYSFS_GPIO_DIRECTORY + "/export");
        if (ExportFile.open(QIODevice::WriteOnly)) {
            const char *p_PinNumber = QString::number(m_PinNumber).toStdString().c_str();
            (void)ExportFile.write(p_PinNumber, qstrlen(p_PinNumber) + 1);
        }
        else {
            Global::EventObject::Instance().RaiseEvent(EVENT_GPIO_ERROR, Global::FmtArgs() <<"Exporting gpio failed");
        }
    }
}

/****************************************************************************/
/*!
 *  \brief Set direction for the GPIO
 *  \iparam Direction - 1 for O/P , 0 for I/P
 */
/****************************************************************************/
void GPIOPin::SetDirection(const quint32 Direction)
{
    quint32 CurrentDirection = GetDirection();
    QFile DirectionFile(SYSFS_GPIO_DIRECTORY + QString("/gpio%1/direction").arg(m_PinNumber));
    if (DirectionFile.open(QIODevice::WriteOnly)) {
        if (Direction != CurrentDirection) {
            if (Direction == DIRECTION_IS_OUTPUT) {
                (void)DirectionFile.write("out", 4);
            }
            else {
                (void)DirectionFile.write("in", 3);
            }
        }
    }
    else {
        Global::EventObject::Instance().RaiseEvent(EVENT_GPIO_ERROR, Global::FmtArgs() << "Set direction failed");
    }
}

/****************************************************************************/
/*!
 *  \brief  Get current direction of the GPIO.
 *  \return Direction - 1 for O/P , 0 for I/P, 2 for Error.
 */
/****************************************************************************/
quint32 GPIOPin::GetDirection()
{
    QFile DirectionFile(SYSFS_GPIO_DIRECTORY + QString("/gpio%1/direction").arg(m_PinNumber));
    if (DirectionFile.open(QIODevice::ReadOnly)) {
        QTextStream File(&DirectionFile);
        while (!File.atEnd()) {
            QString Direction = File.readLine();
            if (Direction == QString("out")) {
                return DIRECTION_IS_OUTPUT;
            }
            else if (Direction == QString("in")) {
                return DIRECTION_IS_INPUT;
            }
            else {
                return DIRECTION_ERROR;
            }
        }
    }
    else {
        return DIRECTION_ERROR;
    }

    return DIRECTION_ERROR;
}

/****************************************************************************/
/*!
 *  \brief  Opens the sysfs file for the required GPIO
 *  \return File descriptor of the file on success, -1 on error
 */
/****************************************************************************/
qint32 GPIOPin::Open()
{
    if (mp_ValueFile->isOpen()) {
        return mp_ValueFile->handle();
    }
    if (m_Direction == DIRECTION_IS_OUTPUT) {
        if (!mp_ValueFile->open(QIODevice::ReadWrite)) {
            Global::EventObject::Instance().RaiseEvent(EVENT_GPIO_ERROR, Global::FmtArgs() << "Gpio Value file open failed");
            return -1;
        }
    }
    else {
        if (!mp_ValueFile->open(QIODevice::ReadOnly)) {
            Global::EventObject::Instance().RaiseEvent(EVENT_GPIO_ERROR, Global::FmtArgs() << "Gpio Value file open failed");
            return -1;
        }
    }
    m_Fd = mp_ValueFile->handle();
    if( 0 > m_Fd ){
        Global::EventObject::Instance().RaiseEvent(EVENT_GPIO_ERROR, Global::FmtArgs() << "File open failed");
    }
    return m_Fd;
}

/****************************************************************************/
/*!
 *  \brief  Returns the value on the GPIO pin
 *  \return GPIO pin value on success, -1 on error.
 */
/****************************************************************************/
qint32 GPIOPin::GetValue()
{
    if (m_Direction == DIRECTION_IS_INPUT) {
        if ( 0 > m_Fd ) {
            return -1;
        }
        lseek(m_Fd, 0, SEEK_SET); /* Start of file */
        char GpioVal;
        int Return = read(m_Fd, &GpioVal, 1);
        if (Return < 0) {
            return -1;
        }
        if ( GpioVal != '0' ) {
            return (m_CurrentValue = 1);
        } else {
            return (m_CurrentValue = 0);
        }
    }
    return -1;
}

/****************************************************************************/
/*!
 *  \brief  Sets the Edge type
 *  \iparam p_Edge "falling" / "rising" / "both" / "none"
 *
 *  \return '-1' for error , '0' for success.
 */
/****************************************************************************/
qint32 GPIOPin::SetEdge(const char *p_Edge)
{
    QFile EdgeFile(SYSFS_GPIO_DIRECTORY + QString("/gpio%1/edge").arg(m_PinNumber));
    if(!EdgeFile.open(QIODevice::WriteOnly)){
        return -1;
    }
    (void)EdgeFile.write(p_Edge, strlen(p_Edge) + 1);
    return 0;
}

/****************************************************************************/
/*!
 *  \brief  Sets the value on the GPIO pin
 *  \iparam Value - 1 or 0
 *
 *  \return -1 for error , 0 for success.
 */
/****************************************************************************/
qint32 GPIOPin::SetValue(const qint32 Value)
{
    if ( m_Direction == DIRECTION_IS_OUTPUT && m_CurrentValue != Value ) {
        if ( 0 > m_Fd ) {
            Open();
        }
        if ( 0 > m_Fd ) {
            return -1;
        }
        lseek(m_Fd, 0, SEEK_SET); /* Start of file */
        if ( Value ) {
            if (write(m_Fd, "1", 2) == -1) {
                return -1;
            }
        } else {
            if (write(m_Fd, "0", 2) == -1) {
                return -1;
            }
        }

        m_CurrentValue = Value;
    }
    return 0;
}

} // End of namespace GPIOManager
