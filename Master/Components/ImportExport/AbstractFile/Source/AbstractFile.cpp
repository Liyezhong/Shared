/****************************************************************************/
/*! \file AbstractFile.cpp
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

#include "ImportExport/AbstractFile/Include/AbstractFile.h"

namespace ImportExport {


/****************************************************************************/
/*!
 * \brief destructor: AbstractFile class
 */
/****************************************************************************/
AbstractFile::~AbstractFile()
{

}

// PlainFile **************************************************************
/****************************************************************************/
/*!
 * \brief default constructor for plain file specialization
 */
/****************************************************************************/
PlainFile::PlainFile(): mp_fd(0)
{

}

/****************************************************************************/
/*!
 * destructor: delete FailSafeOpen instance
 */
/****************************************************************************/
PlainFile::~PlainFile()
{
    if(mp_fd)
    {
        delete mp_fd;    //lint !e1551
                        // an exception should be impossible here
    }
}

/****************************************************************************/
/*!
 * \brief open a plain file for write (via FailSafeOpen)
 *
 * \iparam name - Name of the file
 */
/****************************************************************************/
void PlainFile::open(QString name)
{
    mp_fd = new FailSafeOpen(name, 'w');
    m_name = name;
}

/****************************************************************************/
/*!
 * \brief write data to plain file
 *
 * \iparam data - data to write into the file
 */
/****************************************************************************/
void PlainFile::write(QByteArray data)
{
    if(mp_fd)
    {
        mp_fd->write(data);
    }
}

/****************************************************************************/
/*!
 * \brief close plain file
 */
/****************************************************************************/
void PlainFile::close()
{
    if(mp_fd)
    {
        mp_fd->close();
    }
}


// RAMFile **************************************************************
/****************************************************************************/
/*!
 * \brief default constructor: create empty "RAMFile" hashtable
 */
/****************************************************************************/
RAMFile::RAMFile():
        m_filedict(QHash<QString, QByteArray>()), m_name()
{
}

/****************************************************************************/
/*!
 * \brief create new, empty entry in hashtable
 *
 * \iparam name - Name of the file
 */
/****************************************************************************/
void RAMFile::open(QString name)
{
    m_name = name;
    m_filedict[m_name] = QByteArray();
}

/****************************************************************************/
/*!
 * \brief "write" to file: append data to value in hashtable
 *
 * \iparam data - data to write into the file
 */
/****************************************************************************/
void RAMFile::write(QByteArray data)
{
    m_filedict[m_name] += data;
}

/****************************************************************************/
/*!
 * \brief close "RAMFile": void operation
 */
/****************************************************************************/
void RAMFile::close()
{
}


// VoidFile **************************************************************
/****************************************************************************/
/*!
 * \brief default constructor: create empty set of filenames
 */
/****************************************************************************/
VoidFile::VoidFile(): m_filenames(QSet<QString>()), m_name(QString())
{
}

/****************************************************************************/
/*!
 * \brief "open" file, i.e. add name to filename set
 *
 * \iparam name - Name of the file
 */
/****************************************************************************/
void VoidFile::open(QString name)
{
    m_filenames << name;
    m_name = name;
}

/****************************************************************************/
/*!
 * \brief write to file - void operation here
 *
 * \iparam data - data to write into the file
 */
/****************************************************************************/
void VoidFile::write(QByteArray data)
{
    Q_UNUSED(data);
}

/****************************************************************************/
/*!
 * \brief close file - void operation here
 */
/****************************************************************************/
void VoidFile::close()
{
}

}       // end namespace ImportExport
