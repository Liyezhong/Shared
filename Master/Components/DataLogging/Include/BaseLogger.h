/****************************************************************************/
/*! \file DataLogging/Include/BaseLogger.h
 *
 *  \brief Definition file for class BaseLogger.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
 *  $Author:    $ J.Bugariu
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

#ifndef DATALOGGING_BASELOGGER_H
#define DATALOGGING_BASELOGGER_H

#include <DataLogging/Include/LoggingObject.h>

#include <QFile>
#include <QFileInfo>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Base class for all logger classes.
 *
 * This class encapsulates the base functionality for all logger classes: close
 * and open files for writing, append a line to a file, etc.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class BaseLogger : public LoggingObject {
friend class TestBaseLogger;
private:
    QFile   m_File;                 ///< File in which logging is done.
    int     m_FormatVersion;        ///< Format version of data.
    /****************************************************************************/
    BaseLogger();                                           ///< Not implemented.
    BaseLogger(const BaseLogger &);                         ///< Not implemented.
    const BaseLogger & operator = (const BaseLogger &);     ///< Not implemented.
protected:
    /****************************************************************************/
    /**
     * \brief Get format version.
     *
     * \return  Format version.
     */
    /****************************************************************************/
    inline int GetFormatVersion() const {
        return m_FormatVersion;
    }
    /****************************************************************************/
    /**
     * \brief Check if file is open.
     *
     * \return  true if file is open.
     */
    /****************************************************************************/
    inline bool IsLogFileOpen() const {
        return m_File.isOpen();
    }
    /****************************************************************************/
    /**
     * \brief Get file size.
     *
     * \return  Current file size as reported by Qt.
     */
    /****************************************************************************/
    inline qint64 GetFileSize() const {
        return m_File.size();
    }
    /****************************************************************************/
    /**
     * \brief Get complete base file name.
     *
     * From the Qt documentation:
     * Returns the complete base name of the file without the path.
     * The complete base name consists of all characters in the file up to
     * (but not including) the last '.' character.
     *
     * Example:
     * For a file called "/tmp/archive.tar.gz" only "archive.tar" will be returned.
     *
     * \return  Complete base file name.
     */
    /****************************************************************************/
    inline QString GetCompleteBaseFileName() const {
        return QFileInfo(m_File).completeBaseName();
    }
    /****************************************************************************/
    /**
     * \brief Get complete file name including the path.
     *
     * From the Qt documentation:
     * Returns an absolute path including the file name.
     * The absolute path name consists of the full path and the file name.
     * On Unix this will always begin with the root, '/', directory. On Windows
     * this will always begin 'D:/' where D is a drive letter, except for network
     * shares that are not mapped to a drive letter, in which case the path will
     * begin '//sharename/'. QFileInfo will uppercase drive letters. Note that
     * QDir does not do this. The code snippet below shows this.
     *
     * Example:
     * For a file called "/tmp/archive.tar.gz" "/tmp/archive.tar.gz" will be returned.
     *
     * \return  Complete file name including path.
     */
    /****************************************************************************/
    inline QString GetCompleteFileName() const {
        return QFileInfo(m_File).absoluteFilePath();
    }
    /****************************************************************************/
    /**
     * \brief Create new output file and open it for writing.
     *
     * Create a new file using m_File. m_File remains open for writing.
     *
     * \param[in]   FileName    New file name to use.
     */
    /****************************************************************************/
    void CreateNewFile(const QString &FileName);
    /****************************************************************************/
    /**
     * \brief Remove file.
     *
     * \param[in]   FileName    File name.
     */
    /****************************************************************************/
    void RemoveFile(const QString &FileName) const;
    /****************************************************************************/
    /**
     * \brief Opens an existing file for append.
     *
     * m_File is closed, then openend in append mode.
     * If open fails, m_File remains closed.
     *
     * \param[in]   FileName    File name.
     */
    /****************************************************************************/
    void OpenFileForAppend(const QString &FileName);
    /****************************************************************************/
    /**
     * \brief Convert a timestamp to string for logging.
     *
     * The format used for conversion is "yyyy-MM-dd hh:mm:ss.zzz".
     * \param[in]   TimeStamp   Time stamp to convert into string.
     * \return                  Converted time stamp.
     */
    /****************************************************************************/
    inline QString TimeStampToString(const QDateTime &TimeStamp) const {
        return TimeStamp.toString("yyyy-MM-dd hh:mm:ss.zzz");
    }
    /****************************************************************************/
    /**
     * \brief Get current time stamp as string.
     *
     * The format is suitable for writing the header information. "yyyy-MM-dd hh:mm:ss.zzz"
     *
     * \return  Current time stamp.
     */
    /****************************************************************************/
    inline QString GetTimeStampHeader() const {
        return Global::AdjustedTime::Instance().GetCurrentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    }
    /****************************************************************************/
    /**
     * \brief Get current time stamp as string.
     *
     * The format is suitable for writing the file name. "yy-MM-dd"
     *
     * \return  Current time stamp.
     */
    /****************************************************************************/
    inline QString GetTimeStampFileName() const {
        return Global::AdjustedTime::Instance().GetCurrentDateTime().toString("yyyyMMdd");
    }
    /****************************************************************************/
    /**
     * \brief Append a line to current open log file.
     *
     * Append a line to current open log file. The trailing "\n" is also appended.
     *
     * \param[in]   Line    line to append (without trailing "\n")
     */
    /****************************************************************************/
    void AppendLine(QString Line);
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   pParent             Parent.
     * \param[in]   TheLoggingSource    Source to set in log entry.
     * \param[in]   FormatVersion       Format version for output file.
     */
    /****************************************************************************/
    BaseLogger(LoggingObject *pParent, const Global::LoggingSource &TheLoggingSource, int FormatVersion);
    /****************************************************************************/
    /**
     * \brief Destructor.
     *
     * All open files will be closed.
     */
    /****************************************************************************/
    virtual ~BaseLogger() {
    }
    /****************************************************************************/
    /**
     * \brief Close file.
     *
     * If file is open, it will be flushed and then closed. File name will be cleared.
     */
    /****************************************************************************/
    inline void CloseFile()  {
        if(m_File.isOpen()) {
            static_cast<void>(
                    // we DO NOT NEED the return value of flush
                    m_File.flush()
            );
        }
        m_File.close();
        m_File.setFileName("");
    }
}; // end class BaseLogger

} // end namespace DataLogging

#endif // DATALOGGING_BASELOGGER_H
