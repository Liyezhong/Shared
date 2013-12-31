/****************************************************************************/
/*! \file DataLogging/Source/EventFilter.cpp
 *
 *  \brief Implementation file for class EventFilter.
 *
 *\b Description:
 *      It must be send, if for a specified file name and line number, the
 *      sending is allowed.
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2013-10-16
 *  $Author:    $ Raju
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

#include <DataLogging/Include/EventFilter.h>

#include <QReadLocker>
#include <QWriteLocker>

namespace DataLogging {

EventFilter EventFilter::m_Instance;

/****************************************************************************/
EventFilter::EventFilter() {
}

/****************************************************************************/
EventFilter::~EventFilter() {
}

/****************************************************************************/
void EventFilter::Clear() {
    QWriteLocker WL(&m_SyncObject);

    m_FileInfos.clear();
}

/****************************************************************************/
QString EventFilter::RemovePath(const QString &FileName) {
    QString Result;
    int idx = FileName.lastIndexOf("/");
    if(idx != -1) {
        // extract file name
        Result = FileName.mid(idx+1);
    } else {
        // take complete name
        Result = FileName;
    }
    return Result;
}

/****************************************************************************/
void EventFilter::AddLine(const QString &FileName, int Line) {
    QWriteLocker WL(&m_SyncObject);

    // compute simplified file name
    QString SimpleName = RemovePath(FileName);
    // Get reference to file info. Create one ifit not exist
    FileInfo &FI = m_FileInfos[SimpleName];
    // add line
    FI.AddLine(Line);
}

/****************************************************************************/
void EventFilter::AddLineRange(const QString &FileName, int LineStart, int LineStop) {
    QWriteLocker WL(&m_SyncObject);

    if(LineStart > LineStop) {
        // exit with no error
        return;
    }
    // compute simplified file name
    QString SimpleName = RemovePath(FileName);
    // Get reference to file info. Create one if it not exist
    FileInfo &FI = m_FileInfos[SimpleName];
    // add line range
    FI.AddLineRange(LineStart, LineStop);
}

/****************************************************************************/
bool EventFilter::MustSend(const QString &FileName, int Line) const {
    QReadLocker RL(&m_SyncObject);

    // compute simplified file name
    QString SimpleName = RemovePath(FileName);
    QHash<QString, FileInfo>::const_iterator it = m_FileInfos.find(SimpleName);
    if(it == m_FileInfos.constEnd()) {
        // file not in list
        return false;
    }
    // check
    return it->Contains(Line);
}

} // end namespace DataLogging
