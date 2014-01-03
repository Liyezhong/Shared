/****************************************************************************/
/*! \file CmdLanguageFile.cpp
 *
 *  \brief CmdLanguageFile command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 28.08.2012
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

#include <NetCommands/Include/CmdLanguageFile.h>


namespace NetCommands {

QString CmdLanguageFile::NAME = "NetCommands::CmdLanguageFile";

/****************************************************************************/
/*!
 *  \brief   Constructor for sending
 *
 * \iparam   Timeout                 Timeout for command.
 * \iparam   LanguageFileDataStream  Language File Datastream.
 * \iparam   CurrentLanguage         Current Language in Set by user
 */
/****************************************************************************/
CmdLanguageFile::CmdLanguageFile(int Timeout, const QDataStream &LanguageFileDataStream, const QLocale::Language CurrentLanguage) :
    Command(Timeout),
    m_LanguageFileByteArray(LanguageFileDataStream.device()->readAll()),
    m_CurrentLangauge(CurrentLanguage)

{

}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdLanguageFile::CmdLanguageFile() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdLanguageFile::~CmdLanguageFile()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdLanguageFile::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses

