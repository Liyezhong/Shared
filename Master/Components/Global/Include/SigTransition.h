#ifndef SIGTRANSITION_H
#define SIGTRANSITION_H
#include <QSignalTransition>
#include <QStateMachine>

namespace Global
{

template<typename T>

class CSignalTransition : public QSignalTransition
{

typedef bool (T::*TransCall_t)(QEvent *e);

public:
    CSignalTransition(
        QObject *sender,
        const char *signal,
        T &Object,
        TransCall_t callback,
        QAbstractState *target = 0
    ) : QSignalTransition(sender, signal),
        m_Object(Object),
        m_callback(callback)
    {
      setTargetState(target);
    }

protected:
    bool eventTest(QEvent *e)
    {
        if (!QSignalTransition::eventTest(e))
            return false;
        if (! (m_Object.*m_callback)(e) )
            return false;
        return true;
    }

private:
    T &m_Object;
    TransCall_t m_callback;
};

} //namespace
#endif // SIGTRANSITION_H
