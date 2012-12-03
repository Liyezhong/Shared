/****************************************************************************
**

**
****************************************************************************/

#include <QApplication>
#include <QtCore>

#include <stdlib.h>

#include <NetworkServer.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QString str = "local";
    NetworkServer *lserver = new NetworkServer(str, &app);
    if (!lserver->startServer()) {
        qDebug() << "Cannot start local Server, exiting";
        delete lserver;
        return -1;
    }
    qDebug() << "Started local Server";

    str = "global";
    NetworkServer *gserver = new NetworkServer(str, &app);
    if (!gserver->startServer()) {
        qDebug() << "Cannot start global Server, exiting";
        delete lserver;
        delete gserver;
        return -1;
    }
    qDebug() << "Started global Server";

    return app.exec();
}
