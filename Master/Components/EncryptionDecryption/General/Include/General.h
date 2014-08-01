/****************************************************************************/
/*! \file General.h
 *
 *  \brief constants and methods used by several classes of Import/Export
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

#ifndef ENCRYPTIONDECRYPTION_GENERAL_H
#define ENCRYPTIONDECRYPTION_GENERAL_H

#include <QByteArray>
#include <QFile>

namespace EncryptionDecryption {


//// constants for the error numbers
const qint32 ERROR_ENCRYPTIONDECRYPTION_CRYTOSERVICE_RUNNING          = 0x001; //!< trying to run more than one Cryptoservice
const qint32 ERROR_ENCRYPTIONDECRYPTION_EOF_REACHED                   = 0x002; //!< End of the file reached during read file
const qint32 ERROR_ENCRYPTIONDECRYPTION_INCOMPLETE_CHUNK              = 0x003; //!< incomplete chunk on reading a file
const qint32 ERROR_ENCRYPTIONDECRYPTION_UNCOMPRESSION_ERROR           = 0x004; //!< uncompression error on reading a file
const qint32 ERROR_ENCRYPTIONDECRYPTION_CANNOT_OPEN_FILE_FOR_READ     = 0x005; //!< cannot open the file in read mode
const qint32 ERROR_ENCRYPTIONDECRYPTION_CANNOT_OPEN_FILE_FOR_WRITE    = 0x006; //!< cannot open the file in write mode
const qint32 ERROR_ENCRYPTIONDECRYPTION_ERROR_TO_READ                = 0x007; //!< unable to read the file
const qint32 ERROR_ENCRYPTIONDECRYPTION_ERROR_TO_WRITE               = 0x008; //!< unable to write the file
const qint32 ERROR_ENCRYPTIONDECRYPTION_ILLEGAL_MAGIC_HEADER          = 0x009; //!< illegal magic in header of the file
const qint32 ERROR_ENCRYPTIONDECRYPTION_ILLEGAL_HEADER_HMAC           = 0x00a; //!< illegal HMAC header in the key
const qint32 ERROR_ENCRYPTIONDECRYPTION_ILLEGAL_MAGIC_ENTRY           = 0x00b; //!< illegal magic entry number
const qint32 ERROR_ENCRYPTIONDECRYPTION_ILLEGAL_ENTRY_HMAC            = 0x00c; //!< illegal HMAC entry number in the key
const qint32 ERROR_ENCRYPTIONDECRYPTION_ARCHIVEFILE_NAME_NOT_MATCHING = 0x00d; //!< archive file name is different against the
                                                                       //!< archive file name present in the archive file
const qint32 ERROR_ENCRYPTIONDECRYPTION_ENTRIES_NOT_MATCHING          = 0x00e; //!< number of entries are not matching with the archive file entries
const qint32 ERROR_ENCRYPTIONDECRYPTION_EOF_ENTRY                     = 0x00f; //!< unable to read all the entries, because end of the file is reached
const qint32 ERROR_ENCRYPTIONDECRYPTION_ARCHIVEFILE_FORMAT_WRONG      = 0x010; //!< archive file format is wrong
const qint32 ERROR_ENCRYPTIONDECRYPTION_INDEX_IS_MATCHING             = 0x011; //!< hash chain index and USB device indexes are matching
const qint32 ERROR_ENCRYPTIONDECRYPTION_UNABLE_CREATE_FILE_INSTANCE   = 0x012; //!< unable to create the file instance to read or write the file
const qint32 ERROR_ENCRYPTIONDECRYPTION_HMAC_COMPUTATION_STARTED      = 0x013; //!< HMAC computation is already started

const qint32 ERROR_ENCRYPTIONDECRYPTION_KEY_SIZE_LESS                 = 0x014; //!< key size is more by comparing with hash block size
const qint32 ERROR_ENCRYPTIONDECRYPTION_KEYDATA_SIZE_IS_NOT_MATCHING  = 0x015; //!< key file size is not matching with the in-built key file size
const qint32 ERROR_ENCRYPTIONDECRYPTION_HMAC_NOT_INITIALIZED          = 0x016; //!< HMAC is not initialized
const qint32 ERROR_ENCRYPTIONDECRYPTION_AES_NOT_INITIALIZED           = 0x017; //!< AES is not initialized
const qint32 ERROR_ENCRYPTIONDECRYPTION_INTEGER_SIZE_IS_MORE          = 0x018; //!< size of the array shall be either 4 or 2 or 1
const qint32 ERROR_ENCRYPTIONDECRYPTION_MSB_BIT_IS_NOT_SET            = 0x019; //!< Most significat bit not set for calculations
const qint32 ERROR_ENCRYPTIONDECRYPTION_INVALID_FILE_MODE             = 0x01a; //!< file mode is not valid

/****************************************************************************/
/*!
 * \brief constants of general use
 */
/****************************************************************************/
struct Constants
{
    // file where the hash chain is stored
    static const QString keyfile;

    // file where only the hash chain counter is stored
    static const QString counter;

    // size (in bytes) of digest of used hash function (SHA-1)
    static const int HASH_SIZE = 20;        //!< size (in bytes) of digest of used hash function (SHA-1)

    // bytes required to store An, Bn, C and the counter (4 byte)
    static const int KEYFILESIZE = 3*HASH_SIZE + 4;     //!< bytes required to store An, Bn, C and the counter (4 byte)

    // buffer size used in CompressEncrypt class: 1 MB
    static const int COMPR_ENCR_BUFSIZE = 1024*1024;    //!< buffer size used in CompressEncrypt class: 1 MB

    // buffer size used for reading files in WriteArchive: 512 KB
    static const int WRITE_ARCH_BUFSIZE = 512*1024;     //!< buffer size used for reading files in WriteArchive: 512 KB

    // magic strings n archives
    static const QByteArray headerMagic;
    static const QByteArray entryMagic;

    // keynames in order used in archive
    static const QList<QByteArray> keynames;
};

/****************************************************************************/
/*!
 * \brief container for generally used functions
 */
/****************************************************************************/
class General
{
    public:
        static QByteArray int2byte(int value, int size = 4);
        static int byte2int(char* bytes, int size = 4);
};

/****************************************************************************/
/*!
 * \brief class for failsafe reading and writing plain files
 */
/****************************************************************************/
class FailSafeOpen
{
    public:
        FailSafeOpen();
        FailSafeOpen(QString name, char mode);
        ~FailSafeOpen();
        void open(QString name, char mode);
        QByteArray read(int size = 0, int filesize = 0);
        int size();
        void write(QByteArray data);
        void close();

    private:
        QString m_name; //!< name of the file
        QFile* mp_fd;    //!< pointer to the file
};

/****************************************************************************/
/*!
 * \brief Store the exception number
 */
/****************************************************************************/
class ExceptionNumber
{
    public:
        ExceptionNumber(qint32 msg);
        /****************************************************************************/
        /*!
         * \brief get the error number
         *
         * \return error number
         */
        /****************************************************************************/
        inline qint32 getErrorNumber() {return m_ErrorNumber;}

    private:
        quint32 m_ErrorNumber; //!< error number

};

#define THROWEXCEPTIONNUMBER(ErrorNumber)      throw(ExceptionNumber(ErrorNumber)) //!< throws the exception of error number

}       // end namespace EncryptionDecryption

#endif          // ENCRYPTIONDECRYPTION_GENERAL_H
