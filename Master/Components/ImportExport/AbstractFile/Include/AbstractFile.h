/****************************************************************************/
/** @file AbstractFile.h
 *
 *  @brief generalized file concept for import, viewer and checker
 *
 *  AbstractFile is the base class; derived are:
 *  - PlainFile is used for Import
 *  - RAMFile is used by Viewer (generating a QHash with file contents
 *    as values)
 *  - VoidFile generates only a QSet of entry names.
 *
 *  Each derived class must define the methods open(), write() and close().
 *  No more methods will be used.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-08-12
 *  $Author:    $ R.Wobst
 *
 *  @b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 *  last modified by owner: @(#) Aug 12 2011, 17:51:29
 *
 */
/****************************************************************************/

#ifndef IMPORT_EXPORT_ABSTRACT_FILE_H
#define IMPORT_EXPORT_ABSTRACT_FILE_H

#include <QByteArray>
#include <QString>
#include <QFile>
#include <QHash>
#include <QSet>
#include "ImportExport/General/Include/General.h"

namespace ImportExport {

// base class

class AbstractFile
{
    public:
        virtual ~AbstractFile();
        virtual void open(QString name) = 0;
        virtual void write(QByteArray data) = 0;
        virtual void close() = 0;
};


// write to plain files

class PlainFile: public AbstractFile
{
    public:
        PlainFile();
        ~PlainFile();
        void open(QString name);
        void write(QByteArray data);
        void close();

    private:
        FailSafeOpen* m_fd;
        QString m_name;
};


// create QHash with filenames as keys and contents as values

class RAMFile: public AbstractFile
{
    friend class TestAbstractFile;

    public:
        RAMFile();
        void open(QString name);
        void write(QByteArray data);
        void close();
        inline QHash<QString, QByteArray> getFiles() {return m_filedict;}

    private:
        QHash<QString, QByteArray> m_filedict;
        QString m_name;
};


// only collect filenames, rest is dummy

class VoidFile: public AbstractFile
{
    friend class TestAbstractFile;

    public:
        VoidFile();
        void open(QString name);
        void write(QByteArray data);
        void close();
        inline QSet<QString> getFilenames() {return m_filenames;}

    private:
        QSet<QString> m_filenames;
        QString m_name;
};


}       // end namespace ImportExport

#endif                  // IMPORT_EXPORT_ABSTRACT_FILE_H
