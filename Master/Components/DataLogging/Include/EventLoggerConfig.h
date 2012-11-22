/****************************************************************************/
/*! \file DataLogging/Include/EventLoggerConfig.h
 *
 *  \brief Definition file for class EventLoggerConfig.
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

#ifndef DATALOGGING_EVENTLOGGERCONFIG_H
#define DATALOGGING_EVENTLOGGERCONFIG_H

#include <Global/Include/GlobalDefines.h>

#include <QString>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Class used to deliver the configuration for the event logger.
 *
 * This class can be used to deliver the configuration for the event logger
 * using the signal / slot mechanism. It contains the operating mode, serial
 * number, the absolute path in which the file hast to be written and the
 * maximal file size. If latter one is 0, no maximal file size monitoring
 * is done!
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class EventLoggerConfig {
friend class TestEventLoggerConfig;
private:
    QString     m_OperatingMode;            ///< Current operating mode as string.
    QString     m_EventLoggerBaseFileName;  ///< Base for file name.
    QString     m_SerialNumber;             ///< Serial number.
    QString     m_Path;                     ///< Absolute path in which log files must be written.
    qint64      m_MaxFileSize;              ///< Maximal file size. 0 means no maximal file size monitoring!
    /****************************************************************************/
    /**
     * \brief Copy other instance to self.
     *
     * \param[in]   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    void CopyFrom(const EventLoggerConfig &rOther);
protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    EventLoggerConfig();
    /****************************************************************************/
    /**
     * \brief Default constructor.
     *
     * Implemented due to signal / slot mechanism.
     * \param[in]   OperatingMode               Operating mode as string.
     * \param[in]   EventLoggerBaseFileName     Base for file name.
     * \param[in]   SerialNumber                Serial number.
     * \param[in]   Path                        Absolute path in which log files must be written.
     * \param[in]   MaxFileSize                 Maximal File Size. 0 means no maximal file size monitoring! Values < 0 will be set to 0.
     */
    /****************************************************************************/
    EventLoggerConfig(const QString &EventLoggerBaseFileName,
                      const QString &OperatingMode, const QString &SerialNumber,
                      const QString &Path, qint64 MaxFileSize);
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \param[in]   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    EventLoggerConfig(const EventLoggerConfig &rOther);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    ~EventLoggerConfig();
    /****************************************************************************/
    /**
     * \brief Assignment operator.
     *
     * \param[in]   rOther  Other instance
     * \return              Const reference to self.
     */
    /****************************************************************************/
    const EventLoggerConfig & operator = (const EventLoggerConfig &rOther);
    /****************************************************************************/
    /**
     * \brief Get operating mode.
     *
     * \return      Operating mode.
     */
    /****************************************************************************/
    inline QString GetOperatingMode() const {
        return m_OperatingMode;
    }
    /****************************************************************************/
    /**
     * \brief Get base of file name for even logging.
     *
     * \return      Base of file name for even logging.
     */
    /****************************************************************************/
    inline QString GetEventLoggerBaseFileName() const {
        return m_EventLoggerBaseFileName;
    }

    /****************************************************************************/
    /**
     * \brief Get serial number.
     *
     * \return      Serial number.
     */
    /****************************************************************************/
    inline QString GetSerialNumber() const {
        return m_SerialNumber;
    }
    /****************************************************************************/
    /**
     * \brief Get absolute path in which log files must be written.
     *
     * \return      Path.
     */
    /****************************************************************************/
    inline QString GetPath() const {
        return m_Path;
    }
    /****************************************************************************/
    /**
     * \brief Get maximal file size.
     *
     * \return      Maximal file size.
     */
    /****************************************************************************/
    inline qint64 GetMaxFileSize() const {
        return m_MaxFileSize;
    }
}; // end class EventLoggerConfig

} // end namespace DataLogging

#endif // DATALOGGING_EVENTLOGGERCONFIG_H
