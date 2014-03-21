/****************************************************************************/
/*! \file General.ccp
 *
 *  \brief general constants and utilities
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

#include "ImportExport/General/Include/General.h"

namespace ImportExport {

// initialization of static members
const QString Constants::keyfile = QString("keyfile.file"); ///< constant for key file name
const QString Constants::counter = QString("counter.file"); ///< constant for counter file name

const QByteArray Constants::headerMagic = QByteArray::fromHex("27182818"); ///< constant for headermagic for the archive
const QByteArray Constants::entryMagic = QByteArray::fromHex("31415926"); ///< constant for entry magic for the archive

const QList<QByteArray> Constants::keynames(QList<QByteArray>()
                                            << "Leica" << "Viewer" << "Import"); ///< store the key names

/****************************************************************************/
/*!
 * \brief write a int32 as big endian and return the QByteArray
 *
 * \iparam value - integer to be serialized
 * \iparam size - 2 if short integer, 4 if 32 bit (default)
 *
 * \return QByteArray of converted integer
 */
/****************************************************************************/
QByteArray General::int2byte(int value, int size)   //lint !e578
{    
    char bytes[4];

    if (!(size == 4 || size == 2 || size == 1))
    {
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_INTEGER_SIZE_IS_MORE);
    }

    if ((value & 0x8000000)) // bit 31 not set, n>>=8 below
    {
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_MSB_BIT_IS_NOT_SET);
    }

    for(int counter = (size - 1); counter >= 0; counter--)
    {
        bytes[counter] = (value & 0xff);
        value >>= 8;
    }

    return QByteArray(bytes, size);
}

/****************************************************************************/
/*!
 * \brief convert 4 bytes to integer as big endian
 *
 * \iparam bytes - bytes to convert
 * \iparam size - how many bytes to convert (4 is default)
 *
 * \return integer
 */
/****************************************************************************/
int General::byte2int(char* bytes, int size)    //lint !e578
{
    int value = 0;

    if (!(size == 4 || size == 2 || size == 1))
    {
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_INTEGER_SIZE_IS_MORE);
    }

    for(int counter = 0; counter < size; ++counter)
    {
        value <<= 8;
        value |= (bytes[counter] & 0xff);
    }

    return value;
}


/****************************************************************************/
/*!
 * \brief constructor of Imex exception
 *
 * \iparam ErrorNumber - Number of the error
 */
/****************************************************************************/
ExceptionNumber::ExceptionNumber(qint32 ErrorNumber) : m_ErrorNumber(ErrorNumber)
{

}


/****************************************************************************/
/*!
 * \brief default constructor
 */
/****************************************************************************/
FailSafeOpen::FailSafeOpen(): mp_fd(0)
{

}

/****************************************************************************/
/*!
 * \brief usual constructor
 *
 * throw ImexException if fails
 *
 * iparam name - filename
 * iparam mode - open for write ('w') or read ('r')
 */
/****************************************************************************/
FailSafeOpen::FailSafeOpen(QString name, char mode): mp_fd(0)
{
    open(name, mode);
}

/****************************************************************************/
/*!
 * \brief open a file and catch all errors
 *
 * throw ImexException if fails
 *
 * \iparam name - filename
 * \iparam mode - open for write ('w') or read ('r')
 */
/****************************************************************************/
void FailSafeOpen::open(QString name, char mode)
{
    m_name = name;
    mp_fd = new QFile(name);

    if(!(mode == 'r' || mode == 'w'))
    {
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_INVALID_FILE_MODE);
    }

    if (mp_fd == 0)
    {
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_UNABLE_CREATE_FILE_INSTANCE);
    }

    if(mode == 'r' && !mp_fd->open(QIODevice::ReadOnly))
    {
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_CANNOT_OPEN_FILE_FOR_READ);
    }
    else if(mode == 'w' && !mp_fd->open(QIODevice::WriteOnly))
    {
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_CANNOT_OPEN_FILE_FOR_WRITE);
    }
}

/****************************************************************************/
/*!
 * \brief destructor
 */
/****************************************************************************/
FailSafeOpen::~FailSafeOpen()
{
    if(mp_fd)
    {
        delete mp_fd;    //lint !e1551
                        // an exception should be impossible here
    }
}

/****************************************************************************/
/*!
 * \brief secure read
 *
 * throw ImexException if fails
 * return empty buffer if end was reached
 *
 * \iparam size - read 'size' bytes (default: read whole file)
 *
 * \return - read data
 */
/****************************************************************************/
QByteArray FailSafeOpen::read(int size)
{                                               //lint !e578
    if(!mp_fd)
    {
        return QByteArray();    // should never happen, for lint only
    }

    if(!size)
    {
        return mp_fd->readAll();
    }
    
    QByteArray buffer;

    while(buffer.size() < size)
    {
        buffer += mp_fd->read(size - buffer.size());

        if(mp_fd->error() != QFile::NoError)
        {
            THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_ERROR_TO_READ);
        }

        if(mp_fd->atEnd())
        {
            break;
        }
    }

    return buffer;
}

/****************************************************************************/
/*!
 * \brief return size of file
 *
 * \return - size of file
 */
/****************************************************************************/
int FailSafeOpen::size()
{
    if(!mp_fd)
    {
        return 0;    // should never happen, for lint only
    }
    return mp_fd->size();
}

/****************************************************************************/
/*!
 * \brief secure write
 *
 * throw ImexException if fails
 *
 * \iparam data - data to write
 */
/****************************************************************************/
void FailSafeOpen::write(QByteArray data)
{
    qint64 written = -1;

    if(!mp_fd)
    {
        return;    // should never happen, for lint only
    }

    while(written != data.size())
    {
        if((written = mp_fd->write(data)) == data.size())
        {
            return;
        }

        if(written == -1)
        {
            THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_ERROR_TO_WRITE);
        }

        data = data.mid(written);
    }
}

/****************************************************************************/
/*!
 * \brief close secure file instance
 */
/****************************************************************************/
void FailSafeOpen::close()
{
    if(mp_fd)
    {
        mp_fd->close();
    }
}

}       // end namespace ImportExport
