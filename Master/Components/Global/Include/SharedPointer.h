/****************************************************************************/
/*! \file Global/Include/SharedPointer.h
 *
 *  \brief Definition file for class SharedPointer.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 11.03.2010
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

#ifndef GLOBAL_SHAREDPOINTER_H
#define GLOBAL_SHAREDPOINTER_H

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"

#include <QMutex>
#include <QAtomicInt>

// Inform Lint about cleanup functions:
//
//lint -sem(Global::SharedPointer::NonsafeClear, cleanup)
//lint -e198
//lint -e344

namespace Global {

/****************************************************************************/
/**
 * \brief Base class for thread-safe reference-counting pointers.
 *
 * \warning Why don't we use the QExplicitlySharedDataPointer or
 *          the QSharedDataPointer Qt classes?
 *          Because, as we found out, they are not thread-safe. At least in
 *          the Qt version 4.6.3. The documentation of both classes says
 *          they are, but it is not correct.
 */
/****************************************************************************/
template<class ClassT> class SharedPointer {
friend class TestSharedPointer;

private:

    /****************************************************************************/
    /**
     * \brief This private class realizes reference-counter of target pointer.
     */
    /****************************************************************************/
    class PointerRefCounter {

    public:

        /****************************************************************************/
        /*!
         *  \brief   Default constructor.
         *
         ****************************************************************************/
        inline PointerRefCounter() :
                m_PointerToUserData(NULL),
                m_RefCounter(0)
        {
        }

        /****************************************************************************/
        /*!
         *  \brief   Constructor.
         *
         *  \param   UPtr = pointer to user object
         *
         ****************************************************************************/
        inline PointerRefCounter(ClassT* UPtr) :
                m_PointerToUserData(UPtr),
                m_RefCounter(1)
        {
        }

        /****************************************************************************/
        /*!
         *  \brief   Destructor.
         *
         ****************************************************************************/
        inline ~PointerRefCounter()
        {
        }

        /****************************************************************************/
        /*!
         *  \brief   Increment reference counter.
         *
         *  \return  true  if reference value is NON-Zero
         *           false if reference value is Zero
         *
         ****************************************************************************/
        inline bool IncrementRefCounter()
        {
            return m_RefCounter.ref();
        }

        /****************************************************************************/
        /*!
         *  \brief   Decrement reference counter.
         *
         *  \return  true  if there are still references available.
         *           false if there are no more references available and the
         *                 target object is destroyed.
         *
         ****************************************************************************/
        bool DecrementRefCounter()
        {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
            Q_ASSERT(m_RefCounter > 0); /// \todo: this shall actually never happen --> error
#endif

            // decrement counter and check if it became Zero:
            if(!m_RefCounter.deref()) {
                // reference became ZERO --> delete target object
                if (m_PointerToUserData != NULL) {
                    delete m_PointerToUserData;
                    m_PointerToUserData = NULL;
                }
                return false;
            }
            // reference is not yet Zero --> return true
            return true;
        }

    public:

        /// Pointer to user data
        ClassT*         m_PointerToUserData;
        /// Counter of pointee object users:
        QAtomicInt      m_RefCounter;
    };

public:

    /****************************************************************************/
    /*!
     *  \brief   Default constructor.
     *
     ****************************************************************************/
    inline SharedPointer() :
      m_PointerToRefCounter(NULL)
    {
    }

    /****************************************************************************/
    /*!
     *  \brief   Constructor.
     *
     *  \param   UdPtr = pointer to user object
     *
     ****************************************************************************/
    inline SharedPointer(ClassT* UdPtr) :
        m_PointerToRefCounter(NULL)
    {
        m_PointerToRefCounter = new PointerRefCounter(UdPtr);
    }

    /****************************************************************************/
    /*!
     *  \brief   Destructor.
     *
     *  Decrement refcount and delete pointer if needed.
     *
     ****************************************************************************/
    virtual ~SharedPointer()
    {
        try {
            NonsafeClear();
        }
        CATCHALL_DTOR();
    }

    /****************************************************************************/
    /*!
     *  \brief   Copy Constructor.
     *
     *  \param   rOther = reference to other class instance.
     *
     ****************************************************************************/
    inline SharedPointer(const SharedPointer &rOther) :
      m_PointerToRefCounter(NULL)
    {
        // lock other object:
        QMutexLocker GetOtherGuard(&(rOther.m_GuardMutex));
        // increment reference count
        if(rOther.m_PointerToRefCounter != NULL) {
            rOther.m_PointerToRefCounter->IncrementRefCounter();
        }
        // copy the pointer:
        m_PointerToRefCounter = rOther.m_PointerToRefCounter;
    }

    /****************************************************************************/
    /*!
     *  \brief   Assignment operator.
     *
     * Use temporary copy to avoid cross locking instances (possible deadlocks).
     *  \param   rOther = reference to other class instance.
     *  \return  Instance of this
     *
     ****************************************************************************/
    const SharedPointer & operator = (const SharedPointer &rOther)
    {
        if(this != &rOther) {
            PointerRefCounter   *pTmp = NULL;
            {
                // lock other object:
                QMutexLocker GetOtherGuard(&(rOther.m_GuardMutex));
                // increment reference count
                if(rOther.m_PointerToRefCounter != NULL) {
                    rOther.m_PointerToRefCounter->IncrementRefCounter();
                }
                // copy the pointer:
                pTmp = rOther.m_PointerToRefCounter;
            }

            {
                QMutexLocker GetGuard(&m_GuardMutex);
                // First of all try to clear own instance
                NonsafeClear();
                // now copy object
                m_PointerToRefCounter = pTmp;
            }
        }

        return *this;
    }

    /****************************************************************************/
    /*!
     *  \brief   Operator "->"
     *  \return  ClassT*
     *
     ****************************************************************************/
    ClassT* operator->() const
    {
        if (m_PointerToRefCounter == NULL) {
            return NULL;
        }
        return m_PointerToRefCounter->m_PointerToUserData;
    }

    /****************************************************************************/
    /*!
     *  \brief   Get pointer to the tracked object.
     * \return   ClassT*
     *
     ****************************************************************************/
    ClassT* GetPointerToUserData() const
    {
        if (m_PointerToRefCounter == NULL) {
            return NULL;
        }
        return m_PointerToRefCounter->m_PointerToUserData;
    }

    /****************************************************************************/
    /*!
     *  \brief   Pointer NULL check.
     *
     *  \return  True if one of m_PointerToRefCounter or m_PointerToUserData is NULL.
     *
     ****************************************************************************/
    bool IsNull() const
    {
        // lock object
        QMutexLocker GetGuard(&m_GuardMutex);
        if (m_PointerToRefCounter == NULL) {
            return true;
        }
        return (m_PointerToRefCounter->m_PointerToUserData == NULL);
    }

    /****************************************************************************/
    /*!
     *  \brief   Equality operator.
     *
     * Use temporary copy to avoid cross locking instances (possible deadlocks).
     *  \param[in]   rOther      Const reference to other.
     *  \return                  True if same instance or same pointers.
     */
     /***************************************************************************/
    bool operator == (const SharedPointer &rOther) const
    {
        // compare if identical
        if(this == &rOther) {
            return true;
        }
        PointerRefCounter *pTmp = NULL;
        {
            QMutexLocker GetOtherGuard(&(rOther.m_GuardMutex));
            pTmp = rOther.m_PointerToRefCounter;
        }
        // now lock objects
        {
            QMutexLocker GetGuard(&m_GuardMutex);
            // compare pointers. Comparing pointer to refcounters should be enaugh.
            return m_PointerToRefCounter == pTmp;
        }
    }

    /****************************************************************************/
    /*!
     *  \brief   Set contained pointer to NULL.
     *
     * Decrement refcount and delete pointer if needed. This method is thread safe.
     */
     /***************************************************************************/
    void Clear()
    {
        // now lock object
        QMutexLocker GetGuard(&m_GuardMutex);
        // call nonsafe clear
        NonsafeClear();
    }

private:

    /****************************************************************************/
    /*!
     *  \brief   Set contained pointer to NULL.
     *
     * Decrement refcount and delete pointer if needed. This method is not thread safe
     * and should be called only in a safe way.
     */
     /***************************************************************************/
//suppress message 423(=Creation of memory leak) over the entire function
//lint -e{423}
    void NonsafeClear()
    {
        // now lock object
        if (m_PointerToRefCounter != NULL) {
            if (!m_PointerToRefCounter->DecrementRefCounter()) {
                // reference became ZERO and target object is deleted -->
                // m_PointerToRefCounter object needs to be destroyed too:
                delete m_PointerToRefCounter;
            }            
            // set pointer to NULL
            m_PointerToRefCounter = NULL;
        }
    }

private:

    PointerRefCounter   *m_PointerToRefCounter; ///< Pointer to user data
    mutable QMutex      m_GuardMutex;           ///< Guard Mutex for thread safety.

}; // end class SharedPointer

} // end namespace Global

#endif // GLOBAL_SHAREDPOINTER_H
