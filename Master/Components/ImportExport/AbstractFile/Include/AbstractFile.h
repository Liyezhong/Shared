/****************************************************************************/
/*! \file AbstractFile.h
 *
 *  \brief generalized file concept for import, viewer and checker
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
 *  $Version:   $ 1.0
 *  $Date:      $ 2012-11-26
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
 *
 */
/****************************************************************************/

#ifndef IMPORTEXPORT_ABSTRACTFILE_H
#define IMPORTEXPORT_ABSTRACTFILE_H

#include <QByteArray>
#include <QString>
#include <QFile>
#include <QHash>
#include <QSet>
#include "ImportExport/General/Include/General.h"

namespace ImportExport {

// base class

/****************************************************************************/
/*!
 * \brief abstract class for the file operation
 */
/****************************************************************************/
class AbstractFile
{
    public:
		/****************************************************************************/
		/*!
		 * \brief virtual destructor
		 */
		/****************************************************************************/
        virtual ~AbstractFile();
        /****************************************************************************/
		/*!
		 * \brief virtual function to open the given file
		 *
		 * \iparam   name - name of the file
		 */
		/****************************************************************************/
		virtual void open(QString name) = 0;
		
		/****************************************************************************/
		/*!
		 * \brief virtual function to write data into a file
		 *
		 * \iparam    data - buffer to write into file
		 */
		/****************************************************************************/
        virtual void write(QByteArray data) = 0;
		
		/****************************************************************************/
		/*!
		 * \brief virtual function to close the files
		 */
		/****************************************************************************/
        virtual void close() = 0;
};

/****************************************************************************/
/*!
 * \brief class for writing plain files
 */
/****************************************************************************/
class PlainFile: public AbstractFile
{
    public:
        PlainFile();
        ~PlainFile();
        void open(QString name);
        void write(QByteArray data);
        void close();

    private:
        FailSafeOpen* mp_fd; ///< file pointer
        QString m_name; ///< to store the file name
};


/****************************************************************************/
/*!
 * \brief create QHash with filenames as keys and contents as values
 */
/****************************************************************************/
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
        QHash<QString, QByteArray> m_filedict; ///< to store the file name and file data
        QString m_name; ///< to store the file name
};


/****************************************************************************/
/*!
 * \brief only collect filenames, rest is dummy
 */
/****************************************************************************/
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
        QSet<QString> m_filenames; ///< to store the file names in the list
        QString m_name; ///< to store the file name
};


}       // end namespace ImportExport

#endif                  // IMPORT_EXPORT_ABSTRACT_FILE_H
