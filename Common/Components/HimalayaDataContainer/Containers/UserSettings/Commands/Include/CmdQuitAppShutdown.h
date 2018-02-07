/****************************************************************************/
/*! \file CmdQuitAppShutdown.h
 *
 *  \brief Notice Scheduler that System will shutdown or quit application.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-06-27
 *  $Author:    $ Abe Yang
 *
 *  \b Company:
 *
 *       Leica Biosystems R&D Center Shanghai.
 *
 *  (C) Copyright 2010 by LBS R&D Center Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef MSGCLASSES_CMDQUITAPPSHUTDOWN_H
#define MSGCLASSES_CMDQUITAPPSHUTDOWN_H

#include "Global/Include/Commands/Command.h"
#include <HimalayaDataContainer/Helper/Include/Global.h>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class CmdQuitAppShutdown
 */
/****************************************************************************/
class CmdQuitAppShutdown : public Global::Command
{
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of friend operator<<
     */
    /****************************************************************************/
    friend QDataStream & operator << (QDataStream &, const CmdQuitAppShutdown &);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of friend operator>>
     */
    /****************************************************************************/
    friend QDataStream & operator >> (QDataStream &, CmdQuitAppShutdown &);

public:
    CmdQuitAppShutdown(int TimeOut, DataManager::QuitAppShutdownActionType_t quitAppShutdownActionType);
    CmdQuitAppShutdown(void);

    ~CmdQuitAppShutdown(void);

    virtual QString GetName(void) const;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function QuitAppShutdownActionType
     *
     *  \return from QuitAppShutdownActionType
     */
    /****************************************************************************/
    inline DataManager::QuitAppShutdownActionType_t QuitAppShutdownActionType() const {return m_quitAppShutdownActionType;}

    static QString NAME; ///< Command name.
  
private:
    CmdQuitAppShutdown(const CmdQuitAppShutdown &); ///< Not implemented.
    const CmdQuitAppShutdown &operator = (const CmdQuitAppShutdown &); ///< Not implemented.
    DataManager::QuitAppShutdownActionType_t m_quitAppShutdownActionType;       ///<  Definition/Declaration of variable m_quitAppShutdownActionType

 }; // end class CmdQuitAppShutdown

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function operator<<
 *
 *  \return from operator<<
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdQuitAppShutdown &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    Stream << (int)Cmd.QuitAppShutdownActionType();
    return Stream;
}

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function operator>>
 *
 *  \return from operator>>
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdQuitAppShutdown &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    int temp;
    Stream >> temp;
    Cmd.m_quitAppShutdownActionType = (DataManager::QuitAppShutdownActionType_t)temp;
    return Stream;
}

} // end namespace MsgClasses

#endif // MSGCLASSES_CMDQUITAPPSHUTDOWN_H
