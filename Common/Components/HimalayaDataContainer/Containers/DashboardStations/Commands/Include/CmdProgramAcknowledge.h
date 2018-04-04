/****************************************************************************/
/*! \file CmdProgramAcknowledge.h
 *
 *  \brief CmdProgramAcknowledge command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 11.04.2013
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

#ifndef MSGCLASSES_CMDPROGRAMACKNOWLEDGE_H
#define MSGCLASSES_CMDPROGRAMACKNOWLEDGE_H

#include <Global/Include/Commands/Command.h>
#include <HimalayaDataContainer/Helper/Include/Global.h>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdProgramAcknowledge command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdProgramAcknowledge : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdProgramAcknowledge &);
    friend QDataStream & operator >> (QDataStream &, CmdProgramAcknowledge &);
public:
    CmdProgramAcknowledge();                                             ///< Not implemented.

    static QString NAME;    ///< Command name.
    /****************************************************************************/
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CmdProgramAcknowledge
     *
     *  \param Timeout = int type parameter
     *  \param acknownedgeType =  DataManager::ProgramAcknownedgeType_t type parameter
     *
     *  \return from CmdProgramAcknowledge
     */
    /****************************************************************************/
    CmdProgramAcknowledge(int Timeout, DataManager::ProgramAcknownedgeType_t acknownedgeType, QString retortName = "Common");
    ~CmdProgramAcknowledge();
    virtual QString GetName() const;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function AcknownedgeType
     *
     *  \return from AcknownedgeType
     */
    /****************************************************************************/
    inline DataManager::ProgramAcknownedgeType_t AcknownedgeType() const {return m_AcknownedgeType;}
 private:
    CmdProgramAcknowledge(const CmdProgramAcknowledge &);                     ///< Not implemented.
    const CmdProgramAcknowledge & operator = (const CmdProgramAcknowledge &); ///< Not implemented.
private:
    DataManager::ProgramAcknownedgeType_t m_AcknownedgeType;       ///<  Definition/Declaration of variable m_AcknownedgeType
    QString m_RetortName;
    
}; // end class CmdProgramAcknowledge

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdProgramAcknowledge &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << (int)(Cmd.m_AcknownedgeType);
    Stream << Cmd.m_RetortName;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdProgramAcknowledge &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    int temp;
    Stream >> temp;
    Cmd.m_AcknownedgeType = (DataManager::ProgramAcknownedgeType_t)temp;
    Stream >> Cmd.m_RetortName;
    return Stream;
}
} // end namespace MsgClasses

#endif // MSGCLASSES_CMDPROGRAMACKNOWLEDGE_H
