/****************************************************************************/
/*! \file CmdLockStatus.h
 *
 *  \brief CmdLockStatus command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 26.03.2013
 *   $Author:  $ Abe Yang
 *
 *  \b Company:
 *
  *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2013 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef MSGCLASSES_CMDLOCKSTATUS_H
#define MSGCLASSES_CMDLOCKSTATUS_H

#include <Global/Include/Commands/Command.h>
#include <HimalayaDataContainer/Helper/Include/Global.h>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdLockStatus command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdLockStatus : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdLockStatus &);
    friend QDataStream & operator >> (QDataStream &, CmdLockStatus &);
public:
    CmdLockStatus();                                             ///< Not implemented.

    static QString NAME;    ///< Command name.
    /****************************************************************************/
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CmdLockStatus
     *
     *  \param timeout = int type parameter
     *  \param lockType =  DataManager::LockType_t type parameter
     *  \param isLocked =  bool type parameter
     *
     *  \return from CmdLockStatus
     */
    /****************************************************************************/
    CmdLockStatus(const QString& RetortID, int timeout, DataManager::LockType_t lockType, bool isLocked);
    ~CmdLockStatus();
    virtual QString GetName() const;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function LockType
     *
     *  \return from LockType
     */
    /****************************************************************************/
    inline DataManager::LockType_t LockType() const {return m_LockType;}
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IsLocked
     *
     *  \return from IsLocked
     */
    /****************************************************************************/
    inline bool IsLocked() const {return m_Locked;}

    const QString& RetortName() const {return m_RetortID;}
    
private:
    CmdLockStatus(const CmdLockStatus &);                     ///< Not implemented.
    const CmdLockStatus & operator = (const CmdLockStatus &); ///< Not implemented.
private:
    QString m_RetortID;
    DataManager::LockType_t m_LockType;       ///<  Definition/Declaration of variable m_LockType
    bool m_Locked;       ///<  Definition/Declaration of variable m_Locked
    
}; // end class CmdLockStatus

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdLockStatus &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_RetortID;
    Stream << (int)Cmd.m_LockType;
    Stream << Cmd.m_Locked;
    return Stream;
}

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream from.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdLockStatus &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    Stream >> Cmd.m_RetortID;
    // copy internal data
    int temp;
    Stream >> temp;
    Cmd.m_LockType = (DataManager::LockType_t)temp;
    Stream >> Cmd.m_Locked;
    return Stream;
}
} // end namespace MsgClasses

#endif // MSGCLASSES_CMDLOCKSTATUS_H
