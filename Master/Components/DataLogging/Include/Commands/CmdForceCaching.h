/****************************************************************************/
/*! \file DataLogging/Include/Commands/CmdForceCaching.h
 *
 *  \brief Definition file for class CmdForceCaching.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-08-22
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

#ifndef DATALOGGING_CMDFORCECACHING_H
#define DATALOGGING_CMDFORCECACHING_H

#include <Global/Include/Commands/Command.h>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Command to enforce caching in data logger instances.
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdForceCaching : public Global::Command {
private:
    bool    m_ForceCaching;     ///< Flag indicating that caching has to be done.
    /****************************************************************************/
    CmdForceCaching();                                              ///< Not implemented.
    CmdForceCaching(const CmdForceCaching &);                       ///< Not implemented.
    const CmdForceCaching & operator = (const CmdForceCaching &);   ///< Not implemented.
protected:
public:
    static QString  NAME;       ///< Command name.
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam   Timeout         Timeout for command.
     * \iparam   ForceCaching    True if caching forced.
     */
    /****************************************************************************/
    CmdForceCaching(int Timeout, bool ForceCaching);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdForceCaching();
    /****************************************************************************/
    /**
     * \brief Get command name.
     *
     * \return  Command name.
     */
    /****************************************************************************/
    virtual QString GetName() const {
        return NAME;
    }
    /****************************************************************************/
    /**
     * \brief Get caching mode.
     *
     * \return  Caching mode.
     */
    /****************************************************************************/
    inline bool IsCachingForced() const {
        return m_ForceCaching;
    }
}; // end class CmdForceCaching

} // end namespace DataLogging

#endif // DATALOGGING_CMDFORCECACHING_H
