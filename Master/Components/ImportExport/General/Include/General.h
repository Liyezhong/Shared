/****************************************************************************/
/** @file General.h
 *
 *  @brief constants and methods used by several classes of Import/Export
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
 *  last modified by owner: @(#) Aug 17 2011, 12:54:31
 *
 */
/****************************************************************************/

#ifndef IMPORT_EXPORT_CONSTANTS_H
#define IMPORT_EXPORT_CONSTANTS_H

#include <QByteArray>
#include <QFile>

namespace ImportExport {

/**
 * @brief constants of general use
 */

struct Constants
{
    // file where the hash chain is stored
    static const QString keyfile;

    // file where only the hash chain counter is stored
    static const QString counter;

    // size (in bytes) of digest of used hash function (SHA-1)
    static const int HASH_SIZE = 20;

    // bytes required to store An, Bn, C and the counter (4 byte)
    static const int KEYFILESIZE = 3*HASH_SIZE + 4;

    // buffer size used in CompressEncrypt class: 1 MB
    static const int COMPR_ENCR_BUFSIZE = 1024*1024;

    // buffer size used for reading files in WriteArchive: 512 KB
    static const int WRITE_ARCH_BUFSIZE = 512*1024;

    // magic strings n archives
    static const QByteArray headerMagic;
    static const QByteArray entryMagic;

    // keynames in order used in archive
    static const QList<QByteArray> keynames;
};

/**
 * @brief container for generally used functions
 */

class General
{
    public:
        static QByteArray int2byte(int n, int size = 4);
        static int byte2int(char* bytes, int size = 4);
};


/**
 * @brief class for failsafe reading and writing plain files
 */

class FailSafeOpen
{
    public:
        FailSafeOpen();
        FailSafeOpen(QString name, char mode);
        ~FailSafeOpen();
        void open(QString name, char mode);
        QByteArray read(int size = 0);
        int size();
        void write(QByteArray data);
        void close();

    private:
        QString m_name;
        QFile* m_fd;
};


/**
 * @brief exception used for error messages
 */

class ImexException
{
    public:
        ImexException(QString msg,
                      const char* filename = 0,
                      int lineno = 0);
        inline QString getMsg() {return m_msg;}

    private:
        QString m_msg;
};

#define THROW(msg)      throw(ImexException(msg, __FILE__, __LINE__))

}       // end namespace ImportExport

#endif          // IMPORT_EXPORT_CONSTANTS_H
