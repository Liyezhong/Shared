/****************************************************************************/
/*! \file CmdRetortStatus.h
 *
 *  \brief CmdRetortStatus command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 27.03.2013
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

#ifndef MSGCLASSES_CMDRETORTSTATUS_H
#define MSGCLASSES_CMDRETORTSTATUS_H

#include <Global/Include/Commands/Command.h>
#include <HimalayaDataContainer/Helper/Include/Global.h>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdRetortStatus command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdRetortStatus : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdRetortStatus &);
    friend QDataStream & operator >> (QDataStream &, CmdRetortStatus &);
public:
    CmdRetortStatus();                                             ///< Not implemented.

    static QString NAME;    ///< Command name.
    /****************************************************************************/
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CmdRetortStatus
     *
     *  \param timeout = int type parameter
     *  \param retortStatusType =  DataManager::RetortStatusType_t type parameter
     *
     *  \return from CmdRetortStatus
     */
    /****************************************************************************/
    CmdRetortStatus(int timeout, DataManager::RetortStatusType_t retortStatusType);
    ~CmdRetortStatus();
    virtual QString GetName() const;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RetortStatusType
     *
     *  \return from RetortStatusType
     */
    /****************************************************************************/
    inline DataManager::RetortStatusType_t RetortStatusType() const {return m_RetortStatusType;} 
    
private:
    CmdRetortStatus(const CmdRetortStatus &);                     ///< Not implemented.
    const CmdRetortStatus & operator = (const CmdRetortStatus &); ///< Not implemented.
private:
    DataManager::RetortStatusType_t m_RetortStatusType;       ///<  Definition/Declaration of variable m_RetortStatusType
    
}; // end class CmdRetortStatus

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdRetortStatus &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << (int)Cmd.m_RetortStatusType;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdRetortStatus &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    int temp;
    Stream >> temp;
    Cmd.m_RetortStatusType = (DataManager::RetortStatusType_t)temp;
    return Stream;
}
} // end namespace MsgClasses

#endif // MSGCLASSES_CMDRETORTSTATUS_H
