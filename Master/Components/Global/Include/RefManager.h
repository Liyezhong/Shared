/****************************************************************************/
/*! \file Global/Include/RefManager.h
 *
 *  \brief Definition file for class RefManager.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-09-07
 *  $Author:    $ J.Bugariu
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

#ifndef GLOBAL_REFMANAGER_H
#define GLOBAL_REFMANAGER_H

#include <Global/Include/GlobalDefines.h>
#include <Global/Include/GlobalEventCodes.h>
#include <Global/Include/Exception.h>

#include <QSet>

namespace Global {

/****************************************************************************/
/**
 * \brief This class used to manage references for commands.
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
template<typename REFTYPE> class RefManager {
friend class TestRefManager;
public:
    static REFTYPE  INVALID;    ///< "Invalid" value
private:
    REFTYPE         m_LastRef;      ///< Last used command reference.
    QSet<REFTYPE>   m_BlockedRefs;  ///< Do not use this Refs anymore since they were already used in "timeouted" commands.
    /****************************************************************************/
    /****************************************************************************/
    /**
     * \brief Disable copy and assignment
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(RefManager)
protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    RefManager() :
        m_LastRef(RefManager::INVALID)
    {
        // block invalid value
        static_cast<void>(
                // we DO NOT NEED the return value of remove
                m_BlockedRefs.insert(RefManager::INVALID)
        );
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    inline virtual ~RefManager() {
    }
    /****************************************************************************/
    /**
     * \brief Compute new command reference.
     *
     * Compute new command reference. The computed reference is marked as blocked
     * and will not be reused until it is unblocked.
     *
     * \return   New command reference.
     */
    /****************************************************************************/
    inline REFTYPE GetNewRef() {
        // remember current value
        REFTYPE NewRef = m_LastRef;
        // compute new value
        NewRef++;
        // remember search start
        REFTYPE LoopStartRef = NewRef;
        bool oLoop = false;
        bool oContinueLoop = true;
        // check if can use it
        while(m_BlockedRefs.contains(NewRef) && oContinueLoop) {
            if(oLoop && (NewRef == LoopStartRef)) {
                // seems we looped around and reached start again
                oContinueLoop = false;
            } else {
                // go to next value
                oLoop = true;
                NewRef++;
            }
        }
        // check if we looped around without computing an new reference
        if(oLoop && (NewRef == LoopStartRef)) {
            // we looped around and reached start again without finding a new reference
            THROW(EVENT_GLOBAL_ERROR_COMPUTING_NEW_REF);
        }
        // block computed reference
        static_cast<void>(
                // we DO NOT NEED the return value of remove
                m_BlockedRefs.insert(NewRef)
        );
        // return remembered value.
        m_LastRef = NewRef;
        return m_LastRef;
    }
    /****************************************************************************/
    /**
     * \brief Unblock reference.
     *
     * Unblock reference. This means, this reference can be reused.
     *
     * \param[in]   Ref     reference to unblock.
     */
    /****************************************************************************/
    inline void UnblockReference(const REFTYPE &Ref) {
        static_cast<void>(
                // we DO NOT NEED the return value of remove
                m_BlockedRefs.remove(Ref)
        );
    }
}; // end class RefManager

template<typename REFTYPE> REFTYPE RefManager<REFTYPE>::INVALID = 0;

} // end namespace Global

#endif // GLOBAL_REFMANAGER_H
