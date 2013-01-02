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

} // End of namespace SoftSwitchManager
