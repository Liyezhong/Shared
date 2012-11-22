/****************************************************************************/
/** @file AbstractFile.cpp
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
 *  last modified by owner: @(#) Aug 31 2011, 12:35:22
 *
 */
/****************************************************************************/

#include "ImportExport/AbstractFile/Include/AbstractFile.h"

namespace ImportExport {

//AbstractFile::AbstractFile() {}
AbstractFile::~AbstractFile() {}

// PlainFile **************************************************************

/**
 * @brief default constructor for plain file specialization
 */

PlainFile::PlainFile(): m_fd(0) {}


/**
 * destructor: delete FailSafeOpen instance
 */

PlainFile::~PlainFile()
{
    if(m_fd)
    {
        delete m_fd;    //lint !e1551
                        // an exception should be impossible here
    }
}


/**
 * @brief open a plain file for write (via FailSafeOpen)
 */

void PlainFile::open(QString name)
{
    m_fd = new FailSafeOpen(name, 'w');
    m_name = name;
}


/**
 * @brief write data to plain file
 */

void PlainFile::write(QByteArray data)
{
    if(m_fd)
    {
        m_fd->write(data);
    }
}


/**
 * @brief close plain file
 */

void PlainFile::close()
{
    if(m_fd)
    {
        m_fd->close();
    }
}


// RAMFile **************************************************************

/**
 * @brief default constructor: create empty "RAMFile" hashtable
 */

RAMFile::RAMFile():
        m_filedict(QHash<QString, QByteArray>()), m_name()
{
}


/**
 * @brief create new, empty entry in hashtable
 */

void RAMFile::open(QString name)
{
    // @TODO no test on double occurence - needed?
    m_name = name;
    m_filedict[m_name] = QByteArray();
}


/**
 * @brief "write" to file: append data to value in hashtable
 */

void RAMFile::write(QByteArray data)
{
    m_filedict[m_name] += data;
}


/**
 * @brief close "RAMFile": void operation
 */

void RAMFile::close()
{
    // @TODO prevent repeated write - needed?
}


// VoidFile **************************************************************

/**
 * @brief default constructor: create empty set of filenames
 */

VoidFile::VoidFile(): m_filenames(QSet<QString>()), m_name(QString())
{
}


/**
 * @brief "open" file, i.e. add name to filename set
 */

void VoidFile::open(QString name)
{
    // @TODO no test on double occurence - needed?
    m_filenames << name;
    m_name = name;
}


/**
 * @brief write to file - void operation here
 */

void VoidFile::write(QByteArray data)
{
    Q_UNUSED(data);
}


/**
 * @brief close file - void operation here
 */

void VoidFile::close()
{
}

}       // end namespace ImportExport
