/****************************************************************************/
/** @file General.ccp
 *
 *  @brief general constants and utilities
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-08-09
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
 *  last modified by owner: @(#) Aug 31 2011, 14:17:04
 *
 */
/****************************************************************************/

#include "ImportExport/General/Include/General.h"

namespace ImportExport {

// initialization of static members *****************************

const QString Constants::keyfile = QString("keyfile.file");   // XXX to be changed!!
const QString Constants::counter = QString("counter.file");   // XXX to be changed!!

const QByteArray Constants::headerMagic = QByteArray::fromHex("27182818");
const QByteArray Constants::entryMagic = QByteArray::fromHex("31415926");

const QList<QByteArray> Constants::keynames(QList<QByteArray>()
                                            << "Leica" << "Viewer" << "Import");

// functions in container General ******************************

/**
 * @brief write a int32 as big endian and return the QByteArray
 *
 * @param n - integer to be serialized
 * @param size - 2 if short integer, 4 if 32 bit (default)
 * @return QByteArray of converted integer
 */

QByteArray General::int2byte(int n, int size)   //lint !e578
{
    Q_ASSERT(size == 4 || size == 2 || size == 1);
    Q_ASSERT(!(n & 0x8000000)); // bit 31 not set, n>>=8 below

    char bytes[4];

    for(int i=size; i--;)
    {
        bytes[i] = (n & 0xff);
        n >>= 8;
    }

    return QByteArray(bytes, size);
}


/**
 * @brief convert 4 bytes to integer as big endian
 *
 * @param bytes - bytes to convert
 * @param size - how many bytes to convert (4 is default)
 * @return integer
 */

int General::byte2int(char* bytes, int size)    //lint !e578
{
    Q_ASSERT(size == 4 || size == 2 || size == 1);

    int n = 0;

    for(int i=0; i < size; ++i)
    {
        n <<= 8;
        n |= (bytes[i] & 0xff);
    }

    return n;
}


// exception class ************************************************

/**
 * @brief constructor of Imex exception
 *
 * Error weight, line number etc. could be added later
 *
 * @param msg - optional message
 */

ImexException::ImexException(QString msg,
                             const char* filename,
                             int lineno)
{
    m_msg = QString("File ") + QString(filename) + \
            QString(", line ") + QString::number(lineno) + \
            QString(" -- ") + msg;
}


// failsafe file operations ****************************************

/**
 * @brief default constructor
 */

FailSafeOpen::FailSafeOpen(): m_fd(0) {}

/**
 * @brief usual constructor
 *
 * throw ImexException if fails
 *
 * @param name - filename
 * @param mode - open for write ('w') or read ('r')
 */

FailSafeOpen::FailSafeOpen(QString name, char mode): m_fd(0)
{
    open(name, mode);
}


/**
 * @brief open a file and catch all errors
 *
 * throw ImexException if fails
 *
 * @param name - filename
 * @param mode - open for write ('w') or read ('r')
 */

void FailSafeOpen::open(QString name, char mode)
{
    m_name = name;
    Q_ASSERT(mode == 'r' || mode == 'w');

    Q_ASSERT(m_fd == 0);
    m_fd = new QFile(name);

    if(mode == 'r' && !m_fd->open(QIODevice::ReadOnly))
    {
        THROW(QString("can't open %1 for read").arg(name));
    }
    else if(mode == 'w' && !m_fd->open(QIODevice::WriteOnly))
    {
        THROW(QString("can't open %1 for write").arg(name));
    }
}


/**
 * @brief destructor
 */

FailSafeOpen::~FailSafeOpen()
{
    if(m_fd)
    {
        delete m_fd;    //lint !e1551
                        // an exception should be impossible here
    }
}

/**
 * @brief secure read
 *
 * throw ImexException if fails
 * return empty buffer if end was reached
 *
 * @param size - read 'size' bytes (default: read whole file)
 * @return - read data
 */

QByteArray FailSafeOpen::read(int size)
{                                               //lint !e578
    if(!m_fd)
    {
        return QByteArray();    // should never happen, for lint only
    }

    if(!size)
    {
        return m_fd->readAll();
    }
    
    QByteArray buffer;

    while(buffer.size() < size)
    {
        buffer += m_fd->read(size - buffer.size());

        if(m_fd->error() != QFile::NoError)
        {
            THROW(QString("read error on %1").arg(m_name));
        }

        if(m_fd->atEnd())
        {
            break;
        }
    }

    return buffer;
}


/**
 * @brief return size of file
 *
 * @return - size of file
 */

int FailSafeOpen::size()
{
    if(!m_fd)
    {
        return 0;    // should never happen, for lint only
    }
    return m_fd->size();
}


/**
 * @brief secure write
 *
 * throw ImexException if fails
 *
 * @param data - data to write
 */

void FailSafeOpen::write(QByteArray data)
{
    qint64 written;

    if(!m_fd)
    {
        return;    // should never happen, for lint only
    }

    for(;;)
    {
        if((written = m_fd->write(data)) == data.size())
        {
            return;
        }

        if(written == -1)
        {
            THROW(QString("write error on file %1").arg(m_name));
        }

        data = data.mid(written);
    }
}


/**
 * @brief close secure file instance
 */

void FailSafeOpen::close()
{
    if(m_fd)
    {
        m_fd->close();
    }
}

}       // end namespace ImportExport
