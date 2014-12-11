/****************************************************************************/
/*! \file Global/Include/PriorityQueue.h
 *
 *  \brief Definition file for class PriorityQueue
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-10-18
 *  $Author:    $ N.Kamath
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <QMultiMap>

namespace Global {

/****************************************************************************/
/**
 * \brief A Simple priority Queue template, based on QMultimap
 * \note Priority is based on Integer value. Higher the value, Higher
 *       the priority. If two (or more) items have same priority then,
 *       the last inserted among the two will be placed at top of queue.
 */
/****************************************************************************/
template <typename T>
class PriorityQueue {

private:
    QMultiMap <int, T> m_Element;           //!< Queue Element
    Q_DISABLE_COPY(PriorityQueue)          //!< Disable copy construction and assignment operations

public:
    /****************************************************************************/
    /** \brief Constructor. Clears the queue.
     */
    /****************************************************************************/
    PriorityQueue() {
        m_Element.clear();
    }

    /****************************************************************************/
    /** \brief Push an item in to the queue
     *  \iparam Priority = Priority of the item
     *  \iparam Item = Item to Insert
     */
    /****************************************************************************/
    void Push(int Priority, T &Item) {
        m_Element.insertMulti(Priority, Item);
    }

    /****************************************************************************/
    /** \brief Returns the element at the top of the queue
     */
    /****************************************************************************/
    T Top() const {
        //Keys in Multimap are in ascending order, hence last key is the
        //one with highest priority
        int key = m_Element.keys().last();
        //return m_Element.value(key);
        return m_Element.values(key).last();
    }

    /****************************************************************************/
    /** \brief Returns True if the queue is empty
     */
    /****************************************************************************/
    bool IsEmpty() const {
        return m_Element.isEmpty();
    }

    /****************************************************************************/
    /** \brief Remove an item in from the queue
     *  \iparam Priority = Priority of the item to remove
     *  \iparam Item = Item to remove
     */
    /****************************************************************************/
    void Remove(int Priority, T &Item) {
        if (!IsEmpty()) {
            m_Element.remove(Priority, Item);
        }
    }

    /****************************************************************************/
    /** \brief Returns the size of the queue
     */
    /****************************************************************************/
    int Size() const {
        return m_Element.size();
    }

    /****************************************************************************/
    /** \brief Clears all the items in the queue
     */
    /****************************************************************************/
    void Clear() {
        m_Element.clear();
    }

};

} // EONS Global

#endif // PRIORITYQUEUE_H
