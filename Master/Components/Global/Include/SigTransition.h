#ifndef SIGTRANSITION_H
#define SIGTRANSITION_H
#include <QSignalTransition>
#include <QStateMachine>

namespace Global
{

template<typename T>
/****************************************************************************/
/**
 * \brief class for signal transition
 */
/****************************************************************************/
class CSignalTransition : public QSignalTransition
{
//!<  callback method with event
typedef bool (T::*TransCall_t)(QEvent *e);

public:
    /****************************************************************************/
    /**
     * \brief constructor
     * \iparam Sender = sender object
     * \iparam Signal = signal sent
     * \iparam Object = target object
     * \iparam callback = callback on this signal
     * \iparam target = target state after this signal
     */
    /****************************************************************************/
    CSignalTransition(
        QObject *Sender,
        const char *Signal,
        T &Object,
        TransCall_t callback,
        QAbstractState *target = 0
    ) : QSignalTransition(Sender, Signal),
        m_Object(Object),
        m_callback(callback)
    {
      setTargetState(target);
    }

protected:
    /****************************************************************************/
    /**
     * \brief event test
     * \iparam e = event to be tested
     * \return  true if event test passed
     */
    /****************************************************************************/
    bool eventTest(QEvent *e)
    {
        if (!QSignalTransition::eventTest(e))
            return false;
        if (! (m_Object.*m_callback)(e) )
            return false;
        return true;
    }

private:
    T &m_Object;            ///< object.
    TransCall_t m_callback;     ///< call back
};

} //namespace
#endif // SIGTRANSITION_H
