/****************************************************************************/
/*! \file Global/Source/SignalHandler.cpp
 *
 *  \brief Implementation file for class SignalHandler.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-08-08
 *  $Author:    $ Michael Thiel
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include <Global/Include/SignalHandler.h>

#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <cxxabi.h>

#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <Global/Include/EventObject.h>
#include <QCoreApplication>
#include <QProcess>

namespace Global {

/****************************************************************************/
SignalHandler::SignalHandler()
{
}

/****************************************************************************/
void SignalHandler::init()
{
    signal(SIGSEGV, SignalHandler::crashHandler);
    signal(SIGTERM, SignalHandler::crashHandler);
    signal(SIGINT, SignalHandler::crashHandler);
    signal(SIGFPE, SignalHandler::crashHandler);
    signal(SIGABRT, SignalHandler::crashHandler);
    signal(SIGILL, SignalHandler::crashHandler);
}

/****************************************************************************/
QStringList SignalHandler::getBackTrace(int depth)
{
    QRegExp regexp("([^(]+)\\(([^)^+]+)(\\+[^)]+)\\)\\s(\\[[^]]+\\])");

    void *array[150];
    size_t nfuncs;

    nfuncs = backtrace(array, depth);

    char** symbols = backtrace_symbols(array, nfuncs);

    QStringList ret;

    for (unsigned int i = 0; i < nfuncs; i++)
    {
        if (regexp.indexIn(symbols[i]) != -1)
        {
            //get the library or app that contains this symbol
            QString unit = regexp.cap(1);
            //get the symbol
            QString symbol = regexp.cap(2);
            //get the offset
            QString offset = regexp.cap(3);
            //get the address
            QString address = regexp.cap(4);

            //now try and demangle the symbol
            int stat;
            char *demangled =
                    abi::__cxa_demangle(qPrintable(symbol),0,0,&stat);

            if (demangled)
            {
                symbol = demangled;
                free(demangled);
            }

            //put this all together
            ret.append( QString("(%1) %2 (%3 +%4)\n  -- %5\n")
                                .arg(QString::number(i), 3)
                                .arg(unit).arg(address,offset)
                                .arg(symbol) );
        }
        else
        {
            //split line into words
            QStringList words = QString(symbols[i])
                                    .split(" ", QString::SkipEmptyParts);

            if (words.count() == 6 && words[4] == "+")
            {
                //get the library or app that contains this symbol
                QString unit = words[1];
                //get the symbol
                QString symbol = words[3];
                //get the offset
                QString offset = words[5];
                //get the address
                QString address = words[2];

                //now try and demangle the symbol
                int stat;
                char *demangled =
                        abi::__cxa_demangle(qPrintable(symbol),0,0,&stat);

                if (demangled)
                {
                    symbol = demangled;
                    free(demangled);
                }

                //put this all together
                ret.append( QString("(%1) %2 (%3 +%4)\n  -- %5\n")
                                    .arg(QString::number(i), 3)
                                    .arg(unit).arg(address,offset)
                                    .arg(symbol) );
            }
            else
                //I don't recognise this string - just add the raw
                //string to the backtrace
                ret.append(symbols[i]);
        }
    }

    free(symbols);

    return ret;
}

/****************************************************************************/
void SignalHandler::crashHandler(int sig)
{
    qDebug() << "++++++++++++++++++++++++++++\n++++ Signal catched ++++\n++++++++++++++++++++++++++++";

    qDebug() << "Signal" << sig;
    QStringList stringList = getBackTrace();
    qDebug() << stringList.join("\n");

    qDebug() << "+++++++++++++++++++++++++++++++++\n++++ Terminating application ++++\n+++++++++++++++++++++++++++++++++";

    QString fileName = "Stack_";
    fileName += QDateTime::currentDateTime().toString().replace(" ", "_").replace(":", "_");
    fileName += ".txt";
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << "Process received signal " << sig << "\n";
        out << stringList.join("\n");

        file.close();
    }

   	quint64 PID = QCoreApplication::applicationPid();
    //Kill child processes e.g. GUI, RemotecareAgent etc
    (void)QProcess::execute(QString("pkill -P %1").arg(PID).toStdString().c_str());
    exit(sig);
}


/****************************************************************************/
//SignalHandler::~SignalHandler()
//{
//}


} // end namespace Global
