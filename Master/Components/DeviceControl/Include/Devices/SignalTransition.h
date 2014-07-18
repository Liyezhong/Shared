/****************************************************************************/
/*! \file   SignalTransition.h
 *
 *  \brief  Definition file for class CSignalTransition.
 *
 *  \version  0.1
 *  \date     2012-09-08
 *  \author   Torsten Scheck
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#ifndef DEVICECONTROL_SIGNALTRANSITION_H
#define DEVICECONTROL_SIGNALTRANSITION_H

#include "DeviceControl/Include/Global/DeviceControlReturnCode.h"
#include <QSignalTransition>
#include <QStateMachine>

namespace DeviceControl
{

template<typename T>

/****************************************************************************/
/**
 * \brief   Definition of class CSignalTransition.
 *          Template class to handle signal transition.
 */
/****************************************************************************/
class CSignalTransition : public QSignalTransition
{

//! Call back function pointer type. To invoke registered function on signal transition
typedef bool (T::*TransCall_t)(QEvent *e);

public:
    /****************************************************************************/
    /*!
     *  \brief      Constructor of class CSignalTransition
     *
     *  \iparam     pSender = signal sending object
     *  \iparam     pSignal = signal
     *  \iparam     device = Call back class
     *  \iparam     callback = Call back routine
     *  \iparam     pTarget = target state
     */
    /****************************************************************************/
    CSignalTransition(
        QObject *pSender,
        const char *pSignal,
        T &device,
        TransCall_t callback,
        QAbstractState *pTarget = 0
    ) : QSignalTransition(pSender, pSignal),
        m_device(device),
        m_callback(callback)
    {
      setTargetState(pTarget);
    }

    /****************************************************************************/
    /*!
     *  \brief  To get return code from the parameter list
     *
     *  \iparam p_Event = Parameter list of triggered signal
     *  \iparam Index = Parameter index
     *
     *  \return ReturnCode_t Return Code
     */
    /****************************************************************************/
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

    /****************************************************************************/
    /*!
     *  \brief  To get required value from the parameter list
     *
     *  \iparam p_Event = Parameter list of triggered signal
     *  \iparam Index = Parameter index
     *  \oparam Value = Parameter value
     *
     *  \return false, on error, else true
     */
    /****************************************************************************/
    template<typename TValue>
    static bool GetEventValue(QEvent *p_Event, qint32 Index, TValue &Value)
    {
        QStateMachine::SignalEvent *p_SignalEvent = static_cast<QStateMachine::SignalEvent *>(p_Event);

        if (p_SignalEvent->arguments().count() > Index)
        {
            QVariant Variant = p_SignalEvent->arguments().at(Index);
            Value = Variant.value<TValue>();
            return Variant.isValid();
        }

        return false;
    }

protected:
    /****************************************************************************/
    /*!
     *  \brief  Function invoked on signal transition. This inturn invokes
     *          registered call back function
     *
     *  \iparam p_Event = Parameter list of triggered signal
     *
     *  \return false, on error, else true
     */
    /****************************************************************************/
    bool eventTest(QEvent *p_Event)
    {
        if (!QSignalTransition::eventTest(p_Event)) {
            return false;
        }
        if (!(m_device.*m_callback)(p_Event)) {
            return false;
        }
        return true;
    }

private:
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CSignalTransition)

    T &m_device;              //!< Device Type
    TransCall_t m_callback;   //!< Call back function
};

} //namespace

#endif /* DEVICECONTROL_SIGNALTRANSITION_H */

// vi: set ts=4 sw=4 et:
