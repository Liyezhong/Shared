/****************************************************************************/
/*! \file Application.cpp
 *
 *  \brief Application implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-05-17
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

#include "Application/Include/Application.h"
#include <QFile>
#include <QFont>
#include <QTextStream>

namespace Application {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam Argc = Number of command line arguments
 *  \iparam p_Argv = List of strings containing the command line arguments
 */
/****************************************************************************/
CApplication::CApplication(int Argc, char* p_Argv[],
                           Application::ProjectId_t m_ProjId) : QApplication(Argc, p_Argv)
{
    QFont Font;
    Font.setPointSize(11);
    Font.setFamily("FreeSans");
    setFont(Font);
    setStyle(new CLeicaStyle()); //lint !e1524
    Application::CLeicaStyle::SetProjectId(m_ProjId);
}

} // end namespace Application
