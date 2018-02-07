/****************************************************************************/
/*! \file CmdBottleCheckReply.h
 *
 *  \brief Bottle check reply command.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2015-04-30
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

#ifndef MSGCLASSES_CMDBOTTLECHECKREPLY_H
#define MSGCLASSES_CMDBOTTLECHECKREPLY_H

#include "Global/Include/Commands/Command.h"
#include <HimalayaDataContainer/Helper/Include/Global.h>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class CmdBottleCheckReply
 */
/****************************************************************************/
class CmdBottleCheckReply : public Global::Command
{
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of friend operator<<
     */
    /****************************************************************************/
    friend QDataStream & operator << (QDataStream &, const CmdBottleCheckReply &);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of friend operator>>
     */
    /****************************************************************************/
    friend QDataStream & operator >> (QDataStream &, CmdBottleCheckReply &);
public:
    /*!
      * \brief constructor
      * \param TimeOut = time out
      * \param stationID = station ID
      * \param bottleCheckStatusType = bottle check status type
    */
    CmdBottleCheckReply(int TimeOut, const QString& stationID, DataManager::BottleCheckStatusType_t bottleCheckStatusType);

    /*!
      * \brief constructor
    */
    CmdBottleCheckReply();

    /*!
      * \brief destructor
    */
    ~CmdBottleCheckReply(void);

    /*!
      * \brief function get name
      * \return name
    */
    virtual QString GetName(void) const;
    static QString NAME; ///< Command name.
    /*!
      * \brief get station id
      * \return from StationID, get the station ID
    */
    inline QString StationID() const { return m_StationID; }
    /*!
      * \brief get bottle check status type
      * \return from BottleCheckStatusType, get the results of bottle check
    */
    inline DataManager::BottleCheckStatusType_t BottleCheckStatusType() const { return m_BottleCheckStatusType; }
private:
    /*!
      * \brief copy constructor
    */
    CmdBottleCheckReply(const CmdBottleCheckReply &); ///< Not implemented.
    /*!
      * \brief operator =
      * \return from CmdBottleCheckReply
    */
    const CmdBottleCheckReply &operator = (const CmdBottleCheckReply &); ///< Not implemented.
    QString m_StationID; ///< station id
    DataManager::BottleCheckStatusType_t m_BottleCheckStatusType; ///< bottle check status type

 }; // end class CmdBottleCheckReply

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function operator<<
 *  \param  Stream stream
 *  \param  Cmd cmd
 *  \return from operator<<
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdBottleCheckReply &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    Stream << Cmd.StationID();
    Stream << (int)Cmd.BottleCheckStatusType();
    return Stream;
}

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function operator>>
 *  \param  Stream stream
 *  \param  Cmd cmd
 *  \return from operator>>
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdBottleCheckReply &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    Stream >> Cmd.m_StationID;
    int temp;
    Stream >> temp;
    Cmd.m_BottleCheckStatusType = (DataManager::BottleCheckStatusType_t)temp;
    return Stream;
}

} // end namespace MsgClasses

#endif // MSGCLASSES_CMDBOTTLECHECKREPLY_H
