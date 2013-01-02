#ifndef GPIO_H
#define GPIO_H
#include <QObject>
class GPIOPin: public QObject {
private:
    qint32  m_Fd; //!< File descriptor
    quint32 m_CurrentValue; //!< Current Value of the pin
    quint32 m_PinNumber; //!< GPIO number
    bool    m_Direction; //!< GPIO direction - In/ Out

    void SetDirection(const bool Direction);
    void Open();
    void ExportGPIOPin();

public:
    GPIOPin(quint32 PinNumber, quint32 Direction, QObject *p_Parent = NULL);
    ~GPIOPin();
    void SetEdge(const char *p_Edge);
    void SetValue(const quint32 Value);
    quint32 GetValue() const;
};

#endif // GPIO_H
