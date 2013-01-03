#ifndef GPIO_H
#define GPIO_H
#include <QObject>

namespace SoftSwitchManager {

class GPIOPin: public QObject {
private:
    qint32  m_Fd; //!< File descriptor
    qint32 m_CurrentValue; //!< Current Value of the pin
    quint32 m_PinNumber; //!< GPIO number
    bool    m_Direction; //!< GPIO direction - In/ Out

    void SetDirection(const bool Direction);
    void Open();
    void ExportGPIOPin();

public:
    GPIOPin(quint32 PinNumber, quint32 Direction, QObject *p_Parent = NULL);
    ~GPIOPin();
    qint32 SetEdge(const char *p_Edge);
    qint32 SetValue(const qint32 Value);
    qint32 GetValue();
};

} //End of namespace SoftSwitchManager
#endif // GPIO_H
