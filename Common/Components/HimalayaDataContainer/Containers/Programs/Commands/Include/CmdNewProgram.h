/****************************************************************************/
/*! \file CmdNewProgram.h
 *
 *  \brief CmdNewProgram command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 12.04.2012
 *   $Author:  $ N.Kamath
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
#ifndef CMDNEWPROGRAM_H
#define CMDNEWPROGRAM_H


#include <Global/Include/Commands/Command.h>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdNewProgram command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdNewProgram : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdNewProgram &);
    friend QDataStream & operator >> (QDataStream &, CmdNewProgram &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdNewProgram(int Timeout, const QDataStream &ProgramDataStream, const QDataStream &ProgramSequenceDataStream);
    CmdNewProgram(int Timeout, const QDataStream &ProgramDataStream);
    CmdNewProgram();
    ~CmdNewProgram();
    virtual QString GetName() const;
    /****************************************************************************/
    /**
     * \brief Retrieve Program data
     * \return Reagent Data
     **/
    /****************************************************************************/
    QByteArray const & GetProgramData() const { return m_ProgramByteArray;}
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetProgramSequenceData
     *
     *  \return from GetProgramSequenceData
     */
    /****************************************************************************/
    QByteArray const & GetProgramSequenceData() const { return m_ProgramSequenceByteArray; }


private:
    CmdNewProgram(const CmdNewProgram &);                     ///< Not implemented.
    const CmdNewProgram & operator = (const CmdNewProgram &); ///< Not implemented.
    QByteArray m_ProgramByteArray;          //!< Byte Array filled with Program data
    QByteArray m_ProgramSequenceByteArray;  //!< Byte Array filled with Program data
}; // end class CmdNewProgram

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdNewProgram &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_ProgramByteArray;// << Cmd.m_ProgramSequenceByteArray;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdNewProgram &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_ProgramByteArray;// >> Cmd.m_ProgramSequenceByteArray ;
    return Stream;
}
} // end namespace MsgClasses

#endif // CMDNEWPROGRAM_H
