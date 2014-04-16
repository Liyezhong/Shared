/****************************************************************************/
/*! \file StateEvent.h
 *
 *  \brief Definition file for class StateEvent.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-12-02
 *  $Author:    $ Y.Novak
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

#ifndef STATEMACHINES_STATEEVENT_H
#define STATEMACHINES_STATEEVENT_H

#include <QObject>

namespace StateMachines {

typedef void * StateEventDataPointer_t;        ///< typedef for the Event's pointer to data
typedef unsigned short StateEventIndexType_t;  ///< typedef for the type of Event Index

/****************************************************************************/
/**
 * \brief  This is a base class for a generic Event to be used with
 *         the State objects.
 */
/****************************************************************************/
class StateEvent
{

public:

    /****************************************************************************/
    /*!
    *  \brief    Constructor.
    *
    ****************************************************************************/
    inline StateEvent() :
            m_myIndex(0),
            m_myData(NULL)
    {
    }

    /****************************************************************************/
    /*!
    *  \brief     Constructor.
    *
    *  \iparam t = event index
    *  \iparam dp = pointer to data
    *
    ****************************************************************************/
    inline StateEvent(StateEventIndexType_t t, StateEventDataPointer_t dp) :
            m_myIndex(t),
            m_myData(dp)
    {
    }

    /****************************************************************************/
    /*!
    *  \brief     Constructor.
    *
    *  \iparam t = event index
    *
    ****************************************************************************/
    inline StateEvent(StateEventIndexType_t t) :
            m_myIndex(t),
            m_myData(NULL)
    {
    }

    /****************************************************************************/
    /*!
    *  \brief     Copy Constructor.
    *
    *  \iparam rOther = event object to copy
    *
    ****************************************************************************/
    inline StateEvent(const StateEvent & rOther) :
            m_myIndex(0),
            m_myData(NULL)
    {
        CopyFrom(rOther);
    }

    /****************************************************************************/
    /*!
    *  \brief    Destructor.
    *
    ****************************************************************************/
    inline ~StateEvent()
    {
        m_myData = NULL;
    }

    /****************************************************************************/
    /**
     * \brief     Copy from other instance.
     *
     * \iparam rOther = Instance to copy from.
     */
    /****************************************************************************/
    inline void CopyFrom(const StateEvent &rOther) {
        m_myIndex = rOther.m_myIndex;
        m_myData = rOther.m_myData;
    }

    /****************************************************************************/
    /*!
    *  \brief    Set function for the index.
    *  \iparam   t state event index
    *
    ****************************************************************************/
    inline void SetIndex(StateEventIndexType_t t)
    {
        m_myIndex = t;
    }

    /****************************************************************************/
    /*!
    *  \brief    Get function for the index.
    *
    *  \return   event index
    *
    ****************************************************************************/
    inline StateEventIndexType_t GetIndex() const
    {
        return m_myIndex;
    }

    /****************************************************************************/
    /*!
    *  \brief     Set function for the data pointer.
    *
    *  \iparam dp = pointer to event's data
    *
    ****************************************************************************/
    inline void SetData(StateEventDataPointer_t dp)
    {
        m_myData = dp;
    }

    /****************************************************************************/
    /*!
    *  \brief    Get function for the data pointer.
    *
    *  \return   pointer to data
    *
    ****************************************************************************/
    inline StateEventDataPointer_t GetData()
    {
        return m_myData;
    }


private:

    StateEventIndexType_t    m_myIndex;  ///< this event's index, which identifies this event
    StateEventDataPointer_t  m_myData;   ///< this event's pointer to data, supplied with this event
};

} // end namespace StateMachines

#endif // STATEMACHINES_STATEEVENT_H
