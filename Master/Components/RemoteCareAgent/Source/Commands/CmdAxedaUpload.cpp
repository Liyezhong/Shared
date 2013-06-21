/****************************************************************************/
/*! \file CmdAxedaUpload.cpp
 *
 *  \brief CmdAxedaUpload command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.09.2011
 *   $Author:  $ Y.Novak
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

#include <RemoteCareAgent/Include/Commands/CmdAxedaUpload.h>

#include <QDataStream>
#include <QDebug>

namespace RCAgentNamespace {

QString CmdAxedaUpload::NAME = "RCAgentNamespace::CmdAxedaUpload";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 ****************************************************************************/
CmdAxedaUpload::CmdAxedaUpload() :
        Command(Command::NOTIMEOUT),
        m_FileName("")
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \param[in] timeout = Timeout of this command.
 *
 ****************************************************************************/
CmdAxedaUpload::CmdAxedaUpload(int timeout) :
        Command(timeout),
        m_FileName("")
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \param[in] timeout = Timeout of this command.
 *  \param[in] filename = the name of the file to upload to the RemoteCare Server
 *
 ****************************************************************************/
CmdAxedaUpload::CmdAxedaUpload(int timeout, const QString &filename) :
        Command(timeout),
        m_FileName(filename)
{
}

/****************************************************************************/
/*!
 *  \brief   Copy Constructor
 *
 *  \param[in] rOther = instance to copy
 *
 ****************************************************************************/
CmdAxedaUpload::CmdAxedaUpload(const CmdAxedaUpload &rOther) :
        Command(rOther)
{
    CopyFrom(rOther);
}

/****************************************************************************/
/*!
 *  \brief   Assignment operator
 *
 *  \param[in] rOther = instance to assign
 *
 ****************************************************************************/
const CmdAxedaUpload & CmdAxedaUpload::operator = (const CmdAxedaUpload &rOther)
{
    if (this != &rOther) {
        Command::operator = (rOther);
        CopyFrom(rOther);
    }
    return *this;
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 *
 *
 ****************************************************************************/
CmdAxedaUpload::~CmdAxedaUpload()
{
}

/****************************************************************************/
/*!
 *  \brief   Copy data from another class instance
 *
 *  \param[in] rOther = instance to copy from
 *
 ****************************************************************************/
void CmdAxedaUpload::CopyFrom(const CmdAxedaUpload &rOther)
{
    m_FileName = rOther.m_FileName;
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 *
 ****************************************************************************/
QString CmdAxedaUpload::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   Get the filename of the upload file
 *
 *  \return  filename
 *
 ****************************************************************************/
QString CmdAxedaUpload::GetUploadFileName() const
{
    return m_FileName;
}

} // end namespace
