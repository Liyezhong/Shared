#ifndef DEVICECONTROL_SIGNALTRANSITION_H
#define DEVICECONTROL_SIGNALTRANSITION_H

#include "DeviceControl/Include/Global/DeviceControl.h"
#include <QSignalTransition>
#include <QStateMachine>

namespace DeviceControl
{

template<typename T>

class CSignalTransition : public QSignalTransition
{

typedef bool (T::*TransCall_t)(QEvent *e);

public:
    CSignalTransition(
        QObject *sender,
        const char *signal,
        T &device,
        TransCall_t callback,
        QAbstractState *target = 0
    ) : QSignalTransition(sender, signal),
        m_device(device),
        m_callback(callback)
    {
      setTargetState(target);
    }

    static ReturnCode_t GetEventValue(QEvent *p_Event, qint32 Index)
    {
        QStateMachine::SignalEvent *p_SignalEvent = static_cast<QStateMachine::SignalEvent *>(p_Event);

        if (p_SignalEvent->arguments().count() > Index)
        {
            QVariant Variant = p_SignalEvent->arguments().at(Index);

            if (Variant.isValid()) {
                return Variant.value<ReturnCode_t>();
            }
        }

        return DCL_ERR_INVALID_PARAM;
    }

    template<typename TValue>
    static bool GetEventValue(QEvent *p_Event, qint32 Index, TValue &Value)
    {
        QStateMachine::SignalEvent *p_SignalEvent = static_cast<QStateMachine::SignalEvent *>(p_Event);

        if (p_SignalEvent->arguments().count() > Index)
        {
            QVariant Variant = p_SignalEvent->arguments().at(Index);
            // TODO: if not isValid(), Value is set to QVariant::Invalid (0); okay?
            Value = Variant.value<TValue>();
            return Variant.isValid();
        }

        return false;
    }

protected:
    bool eventTest(QEvent *e)
    {
        if (!QSignalTransition::eventTest(e))
            return false;
        if (! (m_device.*m_callback)(e) )
            return false;
        return true;
    }

private:
    T &m_device;
    TransCall_t m_callback;
};

} //namespace

#endif /* DEVICECONTROL_SIGNALTRANSITION_H */

// vi: set ts=4 sw=4 et:
