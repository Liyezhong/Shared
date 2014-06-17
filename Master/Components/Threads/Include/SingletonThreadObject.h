/****************************************************************************/
/*! \file Threads/Include/SingletonThreadObject.h
 *
 *  \brief Definition file for class SingletonThreadObject.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-12-01
 *  $Author:    $ Nikhil Kamath
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
#ifndef SINGLETONOBJECTTHREAD_H
#define SINGLETONOBJECTTHREAD_H


#include "Global/Include/Utils.h"
#include <QObject>
#include <iostream>
#include <unistd.h>
#include <sys/syscall.h>


namespace Threads {
/****************************************************************************/
/**
 * \brief This class is the singleton thread object class
 */
/****************************************************************************/
class SingletonThreadObject : public QObject
{
    Q_OBJECT
private slots:
    void Run()
    {
        Global::EventObject::Instance().LogThreadId("SingletonObject", syscall(SYS_gettid));
    }
};

} // end namespace Threads

#endif // SINGLETONOBJECTTHREAD_H
