/****************************************************************************/
/*! \file Application.h
 *
 *  \brief Application definition.
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

#ifndef APPLICATION_APPLICATION_H
#define APPLICATION_APPLICATION_H

#include "Application/Include/LeicaStyle.h"
#include <QApplication>

namespace Application {

/****************************************************************************/
/**
 * \brief This class can be used as the basic QApplication object in any
 *        graphical user interface application.
 */
/****************************************************************************/
class CApplication : public QApplication
{
    Q_OBJECT

public:
    CApplication(int &Argc, char* p_Argv[], DeviceType_t DeviceType);
protected:
    bool notify(QObject * receiver, QEvent * event );

signals:
    void InteractStart();

private:
    QPoint m_MousePos;
};

} // end namespace Application

#endif // APPLICATION_APPLICATION_H
