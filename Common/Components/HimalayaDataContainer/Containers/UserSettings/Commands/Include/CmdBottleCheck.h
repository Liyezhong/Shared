/****************************************************************************/
/*! \file CmdBottleCheck.h
 *
 *  \brief Bottle check command.
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

#ifndef MSGCLASSES_CMDBOTTLECHECK_H
#define MSGCLASSES_CMDBOTTLECHECK_H

#include "Global/Include/Commands/Command.h"
#include <HimalayaDataContainer/Helper/Include/Global.h>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class CmdBottleCheck
 */
/****************************************************************************/
class CmdBottleCheck : public Global::Command
{
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of friend operator<<
     */
    /****************************************************************************/
    friend QDataStream & operator << (QDataStream &, const CmdBottleCheck &);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of friend operator>>
     */
    /****************************************************************************/
    friend QDataStream & operator >> (QDataStream &, CmdBottleCheck &);
public:
    /*!
      * \brief constructor
      * \param TimeOut timeout
      *
    */
    CmdBottleCheck(int TimeOut);

    /*!
      * \brief constructor
    */
    CmdBottleCheck();

    /*!
      * \brief destructor
    */
    ~CmdBottleCheck(void);

    /*!
      * \brief function get name
      * \return name
    */
    virtual QString GetName(void) const;
    static QString NAME; ///< Command name.
private:
    /*!
      * \brief copy constructor
    */
    CmdBottleCheck(const CmdBottleCheck &); ///< Not implemented.
    /*!
      * \brief operator =
      * \return from CmdBottleCheck
    */
    const CmdBottleCheck &operator = (const CmdBottleCheck &); ///< Not implemented.

 }; // end class CmdBottleCheck

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function operator<<
 *  \param  Stream stream
 *  \param  Cmd cmd
 *  \return from operator<<
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdBottleCheck &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
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
inline QDataStream & operator >> (QDataStream &Stream, CmdBottleCheck &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    return Stream;
}

} // end namespace MsgClasses

#endif // MSGCLASSES_CMDBOTTLECHECK_H
