/****************************************************************************/
/*! \file DataLogging/Include/BaseLoggerReusable.h
 *
 *  \brief Definition file for class BaseLoggerReusable.
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2013-10-16
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
    QString     m_SWVersion;            ///< SW version string
    /****************************************************************************/
    /****************************************************************************/
    /**
     * \brief Constructor.
     */
    /****************************************************************************/
    BaseLoggerReusable();                                                   ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(BaseLoggerReusable)

    
protected:
    /****************************************************************************/
    /**
     * \brief Set configuration data.
     *
     * Set configuration data. It consists of the operating mode, the serial number
     * and the absolute path in which log files reside.
     *
     * \iparam   OperatingMode       Operating mode.
     * \iparam   SerialNumber        Serial number.
     * \iparam   Path                Absolute path.
     */
    /****************************************************************************/
    void SetConfiguration(const QString &OperatingMode, const QString &SerialNumber,  const QString &Version, const QString &Path);

    /****************************************************************************/
    /**
     * \brief Switch to new output file.
     *
     * We switch to another output file because (for example) the operating mode or
     * the path changed. For that, the old file is closed and the new one is opened.
     * New file is created, which remains open for appending data.
     *
     * \iparam   FileName        File name of file to switch to.
     */
    /****************************************************************************/
    void SwitchToFile(const QString &FileName);

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
     * OK means: Header information can be read and checks the
     * format version coincidence in the opened file.
     *
     * \iparam   FileName    File name.
     * \return   true if header OK.
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
    /****************************************************************************/
    /**
     * \brief Get SW Version.
     *
     * \return      SW version.
     */
    /****************************************************************************/
    inline QString GetSWVersion() const {
        return m_SWVersion;
    }

    /****************************************************************************/
    /**
     * \brief Set SW Version.
     *
     * \param version      SW version.
     */
    /****************************************************************************/
    inline void SetSWVersion(QString version) {
        m_SWVersion = version;
    }
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam   pParent             Parent.
     * \iparam   TheLoggingSource    Source to set in log entry.
     * \iparam   FormatVersion       Format version for output file.
     */
    /****************************************************************************/
    BaseLoggerReusable(QObject *pParent, const QString &TheLoggingSource, int FormatVersion);

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
