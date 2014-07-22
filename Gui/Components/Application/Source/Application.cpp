/****************************************************************************/
/*! \file Application.cpp
 *
 *  \brief Application implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-05-17
 *   $Author:  $ M.Scherer,Shuvasmita.S
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
 #include <QMouseEvent>

namespace Application {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam Argc = Number of command line arguments
 *  \iparam p_Argv = List of strings containing the command line arguments
 *  \iparam DeviceType = Device type
 */
/****************************************************************************/
CApplication::CApplication(int &Argc, char* p_Argv[], Application::DeviceType_t DeviceType) : QApplication(Argc, p_Argv)
{
    QFont Font;
    if (DeviceType == Application::DEVICE_SEPIA) {
        Font.setPointSize(9);
    }
    else {
        Font.setPointSize(11);
    }
    Font.setFamily("FreeSans");
    setFont(Font);
    setStyle(new CLeicaStyle()); //lint !e1524
    Application::CLeicaStyle::SetCurrentDeviceType(DeviceType);
}




bool CApplication::notify ( QObject * receiver, QEvent * event )
{
    bool isTouch = false;
    if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if (mouseEvent)
        {
            QPoint p = mouseEvent->globalPos();
            if (p != m_MousePos)
               isTouch = true;
            m_MousePos = mouseEvent->globalPos();
        }
    }
    else
    {
        if (event->type() == QEvent::KeyPress)
            isTouch = true;
    }

    /*if (isTouch)
        emit this->InteractStart();
*/
    return QApplication::notify(receiver, event);
}

} // end namespace Application
