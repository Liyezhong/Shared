/****************************************************************************/
/*! \file GenericState.h
 *
 *  \brief Generic State Definition.
 *         - Extends QState.
 *         - Calls registered call back function on Entry to state.
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
#ifndef GENERICSTATE_H
#define GENERICSTATE_H

#include <QState>

namespace GPIOManager {

/****************************************************************************/
/**
 * \brief  This class extends the QState Class. Calls the registerd call back
 *         on entering the state.
 */
/****************************************************************************/
template<typename T>
class GenericState: public QState
{
    typedef void (T::*OnEntryCallBack_t)(); //!< Callback function typedef=>simplifies syntax
public:
    /****************************************************************************/
    /**
     * \brief  Constructor
     * \iparam CallBackObject = Object in which call back function is present
     * \iparam CallBackFunc = Call back function to call.
     */
    /****************************************************************************/
    GenericState(T &CallBackObject, OnEntryCallBack_t CallBackFunc)
        :QState(0)
        ,m_CallBackObject(CallBackObject)
        ,m_CallBack(CallBackFunc)
    {

    }
    /****************************************************************************/
    /**
     * \brief  Destructor
     */
    /****************************************************************************/
  ~GenericState(){}

protected:
    /****************************************************************************/
    /**
     * \brief  Overloaded function of QState OnEntry().Calls the registered call
     *         back function.
     *  \iparam p_Event =  Event which caused entry to this state.
     */
    /****************************************************************************/
    virtual void onEntry(QEvent *p_Event)
    {
        //!< First call the base class OnEntry function
        QState::onEntry(p_Event);
        //!< Now call the registered call back function
        (m_CallBackObject.*m_CallBack)();
    }

private:
    T &m_CallBackObject;//!< Object in which call back is present
    OnEntryCallBack_t m_CallBack;//!< Call back function to be called from OnEntry;
};

} //End of namespace GPIOManager
#endif // GENERICSTATE_H
