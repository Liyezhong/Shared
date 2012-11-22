/****************************************************************************/
/*! \file DataLogging/Include/BaseLoggerReusable.h
 *
 *  \brief Definition file for class BaseLoggerReusable.
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

#ifndef DATALOGGING_BASELOGGERREUSABLE_H
#define DATALOGGING_BASELOGGERREUSABLE_H

#include <DataLogging/Include/BaseLogger.h>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Base class for logger classes which can reuse a file (append data
 * to an existing file).
 *
 * This class implements the functionality needed to work with log files on
 * which data can be appended: check and write header information, switch to
 * a new file etc.\n
 * <b>This class should be used only with the signal / slot mechanism.</b>
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class BaseLoggerReusable : public BaseLogger {
friend class TestBaseLoggerReusable;
private:
    QString     m_OperatingMode;        ///< Current operating mode as string.
    QString     m_SerialNumber;         ///< Serial number.
    QString     m_Path;                 ///< Absolute path in which log files must be written.
    /****************************************************************************/
    BaseLoggerReusable();                                                   ///< Not implemented.
    BaseLoggerReusable(const BaseLoggerReusable &);                         ///< Not implemented.
    const BaseLoggerReusable & operator = (const BaseLoggerReusable &);     ///< Not implemented.
    /****************************************************************************/
    /**
     * \brief Create a backup file.
     *
     * Has an empty function body. Overwrite as needed in derived classes.
     *
     * \param[in]   FileName    File name of file to backup.
     */
    /****************************************************************************/
    virtual void BackupFile(const QString & FileName);
protected:
    /****************************************************************************/
    /**
     * \brief Set configuration data.
     *
     * Set configuration data. It consists of the operating mode, the serial number
     * and the absolute path in which log files reside.
     *
     * \param[in]   OperatingMode       Operating mode.
     * \param[in]   SerialNumber        Serial number.
     * \param[in]   Path                Absolute path.
     */
    /****************************************************************************/
    void SetConfiguration(const QString &OperatingMode, const QString &SerialNumber, const QString &Path);
    /****************************************************************************/
    /**
     * \brief Switch to new output file.
     *
     * We switch to another output file because (for example) the operating mode or
     * the path changed. For that, the old file is closed and the new one is opened.
     * If reading the header or header information not OK, a backup of the file is done
     * (if BackupOldFile = true) and a new file is created, which remains open for appending data.
     *
     * \param[in]   FileName        File name of file to switch to.
     * \param[in]   BackupOldFile   If true, make a backup of old file
     */
    /****************************************************************************/
    void SwitchToFile(const QString &FileName, bool BackupOldFile);
    /****************************************************************************/
    /**
     * \brief Write header information.
     *
     * Header information consists of: file name, time stamp, type of file,
     * format version and serial number.
     */
    /****************************************************************************/
    void WriteHeader();
    /****************************************************************************/
    /**
     * \brief Check if header information for a specific file is OK.
     *
     * OK means: header information can be read and the expected and read
     * format version and serial number coincide.
     *
     * \param[in]   FileName    File name.
     * \return                  true if header OK.
     */
    /****************************************************************************/
    virtual bool CheckHeaderFormat(const QString &FileName);
    /****************************************************************************/
    /**
     * \brief Get configured operating mode.
     *
     * \return      Operating mode.
     */
    /****************************************************************************/
    inline QString GetOperatingMode() const {
        return m_OperatingMode;
    }
    /****************************************************************************/
    /**
     * \brief Get configured serial number.
     *
     * \return      Serial number.
     */
    /****************************************************************************/
    inline QString GetSerialNumber() const {
        return m_SerialNumber;
    }
    /****************************************************************************/
    /**
     * \brief Get configured path.
     *
     * \return      Path.
     */
    /****************************************************************************/
    inline QString GetPath() const {
        return m_Path;
    }
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
    BaseLoggerReusable(LoggingObject *pParent, const Global::LoggingSource &TheLoggingSource, int FormatVersion);
    /****************************************************************************/
    /**
     * \brief Destructor.
     *
     * All open files will be closed.
     */
    /****************************************************************************/
    virtual ~BaseLoggerReusable() {
    }
}; // end class BaseLoggerReusable

} // end namespace DataLogging

#endif // DATALOGGING_BASELOGGERREUSABLE_H
