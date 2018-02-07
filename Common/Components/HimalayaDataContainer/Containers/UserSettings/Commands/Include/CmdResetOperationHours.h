/****************************************************************************/
/*! \file CmdResetOperationHours.h
 *
 *  \brief this command is used to disappeare the warning icon of maintainance reminder.
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

#ifndef MSGCLASSES_CMDRESETOPERATIONHOURS_H
#define MSGCLASSES_CMDRESETOPERATIONHOURS_H

#include "Global/Include/Commands/Command.h"
#include <HimalayaDataContainer/Helper/Include/Global.h>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class CmdResetOperationHours
 */
/****************************************************************************/
class CmdResetOperationHours : public Global::Command
{
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of friend operator<<
     */
    /****************************************************************************/
    friend QDataStream & operator << (QDataStream &, const CmdResetOperationHours &);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of friend operator>>
     */
    /****************************************************************************/
    friend QDataStream & operator >> (QDataStream &, CmdResetOperationHours &);

public:
    CmdResetOperationHours(int timeOut, DataManager::ResetOperationHoursType_t resetOperationHoursType);
    CmdResetOperationHours(void);

    ~CmdResetOperationHours(void);

    virtual QString GetName(void) const;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ResetOperationHoursType
     *
     *  \return from ResetOperationHoursType
     */
    /****************************************************************************/
    inline DataManager::ResetOperationHoursType_t ResetOperationHoursType() const {return m_ResetOperationHoursType;}

    static QString NAME; ///< Command name.
  
private:
    CmdResetOperationHours(const CmdResetOperationHours &); ///< Not implemented.
    const CmdResetOperationHours &operator = (const CmdResetOperationHours &); ///< Not implemented.
    DataManager::ResetOperationHoursType_t m_ResetOperationHoursType;       ///<  Definition/Declaration of variable m_ResetOperationHoursType
 }; // end class CmdResetOperationHours

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function operator<<
 *
 *  \return from operator<<
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdResetOperationHours &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    Stream << (int)Cmd.m_ResetOperationHoursType;
    return Stream;
}

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function operator>>
 *
 *  \return from operator>>
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdResetOperationHours &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    int temp;
    Stream >> temp;
    Cmd.m_ResetOperationHoursType = (DataManager::ResetOperationHoursType_t)temp;
    return Stream;
}

} // end namespace MsgClasses

#endif // MSGCLASSES_CMDRESETOPERATIONHOURS_H
