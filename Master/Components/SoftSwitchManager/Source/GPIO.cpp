#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <QDebug>
#include "SoftSwitchManager/Include/GPIO.h"
namespace SoftSwitchManager {

#define BUF_SIZE 64
#define SYSFS_GPIO_DIRECTORY "/sys/class/gpio"

GPIOPin::GPIOPin(quint32 PinNumber, quint32 Direction, QObject *p_Parent)
  :QObject(p_Parent)
  , m_Fd(-1)
  , m_CurrentValue(100000)
  , m_Direction(Direction)
  , m_PinNumber(PinNumber)
{
    ExportGPIOPin();
    SetDirection(m_Direction);
}

GPIOPin::~GPIOPin()
{
    if (0 <= m_Fd) {
        close(m_Fd);
    }
}

void GPIOPin::ExportGPIOPin()
{
    qint32 Fd = -1;
    qint32 Length;
    char Buf[BUF_SIZE];

    Fd = open(SYSFS_GPIO_DIRECTORY"/export", O_WRONLY);
    if (Fd != -1) {
        Length = qsnprintf(Buf, sizeof(Buf), "%d", m_PinNumber);
        write(Fd, Buf, Length);
        close(Fd);
    }
    else {
        // \todo Log error
        qDebug()<<" GPIOPin::ExportGPIOPin() - file open failed";
    }
}

void GPIOPin::SetDirection(const bool Direction)
{
    qint32 Fd = -1;
    qint32 Length;
    char Buf[BUF_SIZE];
    Length = qsnprintf(Buf, sizeof(Buf), SYSFS_GPIO_DIRECTORY"/gpio%d/direction", m_PinNumber);
    Fd = open(Buf, O_WRONLY);
    if (Fd != -1) {
        if (Direction) {
            write(Fd,"out", 4);
        }
        else {
            write(Fd,"in", 3);
        }
    }
    else {
        // \todo Log error
        qDebug()<<" GPIOPin::SetDirection() - file open failed";
    }
    close(Fd);
}

void GPIOPin::Open()
{
    qint32 Length;
    char Buf[BUF_SIZE];
    Length = qsnprintf(Buf, sizeof(Buf), SYSFS_GPIO_DIRECTORY"/gpio%d/value", m_PinNumber);

    if ( 0 <= m_Fd ) {
        close(m_Fd );
    }

    if( m_Direction )
        m_Fd = open(Buf, O_RDWR);
    else
        m_Fd = open(Buf, O_RDONLY);

    if( 0 > m_Fd ){
        qDebug()<<" GPIOPin::Open() File Open failed";
    }
}

qint32 GPIOPin::GetValue()
{
    if( !m_Direction){
        if ( 0 > m_Fd ) {
            Open();
        }
        if ( 0 > m_Fd ) {
            return -1;
        }
        char Ch;

        read(m_Fd, &Ch, 1);

        if ( Ch != '0' ) {
            return (m_CurrentValue = 1);
        } else {
            return (m_CurrentValue = 0);
        }

    }
    return -1;
}

qint32 GPIOPin::SetEdge(const char *p_Edge)
{
    qint32 Length;
    char Buf[BUF_SIZE];
    Length = qsnprintf(Buf, sizeof(Buf), SYSFS_GPIO_DIRECTORY"/gpio%d/edge", m_PinNumber);

    m_Fd = open(Buf, O_WRONLY);

    if( m_Fd < 0 ){
        return -1;
    }

    write(m_Fd, p_Edge, strlen(p_Edge) + 1);

    close(m_Fd);
    return 0;
}

qint32 GPIOPin::SetValue(const qint32 Value)
{
    if ( m_Direction && m_CurrentValue != Value ) {

        if ( 0 > m_Fd ) {
            Open();
        }
        if ( 0 > m_Fd ) {
            return -1;
        }

        if ( Value ) {
            write(m_Fd, "1", 2);
        } else {
            write(m_Fd, "0", 2);
        }

        m_CurrentValue = Value;
    }
    close(m_Fd);
    return -1;
}
} // End of namespace SoftSwitchManager
