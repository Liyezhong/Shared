/****************************************************************************/
/*! \file Global/Source/Commands/CmdByteArray.cpp
 *
 *  \brief CmdByteArray command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 18.01.2012
 *   $Author:  $ M.Scherer
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

#include <Global/Include/Commands/CmdByteArray.h>

#include <QDataStream>
#include <QDebug>

namespace Global {

/****************************************************************************/
/**
 * \brief Default constructor.
 */
/****************************************************************************/
CmdByteArray::CmdByteArray() :
    Command(Command::NOTIMEOUT)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \param[in]  Timeout Timeout for command.
 *  \param[in]  ba      Byte array transported by the command.
 *
 ****************************************************************************/
CmdByteArray::CmdByteArray(int Timeout, const QByteArray &ba) :
    Command(Timeout),
    m_ByteArray(ba)
{
}

/****************************************************************************/
/**
 * \brief Copy constructor.
 *
 * Copy constructor.
 *
 * \param[in]   rOther  Instance to copy from.
 */
/****************************************************************************/
CmdByteArray::CmdByteArray(const CmdByteArray &rOther) :
    Command(rOther)
{
    CopyFrom(rOther);
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 *
 *
 ****************************************************************************/
CmdByteArray::~CmdByteArray()
{
}

/****************************************************************************/
/**
 * \brief Assignment operator.
 *
 * \param[in]   rOther  Instance to copy from.
 * \return              Const reference to self.
 */
/****************************************************************************/
const CmdByteArray & CmdByteArray::operator = (const CmdByteArray &rOther) {
    if(this != &rOther) {
        Command::operator =(rOther);
        CopyFrom(rOther);
    }
    return *this;
}

/****************************************************************************/
/**
 * \brief Copy from other instance.
 *
 * Copy from other instance.
 *
 * \param[in]   rOther  Instance to copy from.
 */
/****************************************************************************/
void CmdByteArray::CopyFrom(const CmdByteArray &rOther) {
    m_ByteArray = rOther.m_ByteArray;
}


/****************************************************************************/
/*!
 *  \brief   This function returns the command data
 *
 *  \return  Data in a QByteArray
 *
 ****************************************************************************/
QByteArray const& CmdByteArray::GetByteArray() const
{
    return m_ByteArray;
}

} // end namespace Global
