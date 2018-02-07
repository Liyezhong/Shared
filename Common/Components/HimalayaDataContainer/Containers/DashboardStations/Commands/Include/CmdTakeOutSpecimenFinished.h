/****************************************************************************/
/*! \file CmdTakeOutSpecimenFinished.h
 *
 *  \brief CmdTakeOutSpecimenFinished command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 17.04.2013
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

#ifndef MSGCLASSES_CMDTAKEOUTSPECIMENFINISHED_H
#define MSGCLASSES_CMDTAKEOUTSPECIMENFINISHED_H

#include <Global/Include/Commands/Command.h>
#include <HimalayaDataContainer/Helper/Include/Global.h>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdTakeOutSpecimenFinished command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdTakeOutSpecimenFinished : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdTakeOutSpecimenFinished &);
    friend QDataStream & operator >> (QDataStream &, CmdTakeOutSpecimenFinished &);
public:
    CmdTakeOutSpecimenFinished();                                             ///< Not implemented.

    static QString NAME;    ///< Command name.
    /****************************************************************************/
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CmdTakeOutSpecimenFinished
     *
     *  \param Timeout = int type parameter
     *  
     *
     *  \return from CmdTakeOutSpecimenFinished
     */
    /****************************************************************************/
    CmdTakeOutSpecimenFinished(int Timeout);
    ~CmdTakeOutSpecimenFinished();
    virtual QString GetName() const;


private:
    CmdTakeOutSpecimenFinished(const CmdTakeOutSpecimenFinished &);                     ///< Not implemented.
    const CmdTakeOutSpecimenFinished & operator = (const CmdTakeOutSpecimenFinished &); ///< Not implemented.
private:

}; // end class CmdTakeOutSpecimenFinished

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdTakeOutSpecimenFinished &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
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
inline QDataStream & operator >> (QDataStream &Stream, CmdTakeOutSpecimenFinished &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    return Stream;
}
} // end namespace MsgClasses

#endif // MSGCLASSES_CMDTAKEOUTSPECIMENFINISHED_H
