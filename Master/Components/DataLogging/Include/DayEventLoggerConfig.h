/****************************************************************************/
/*! \file DataLogging/Include/DayEventLoggerConfig.h
 *
 *  \brief Definition file for class DayEventLoggerConfig.
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

#ifndef DATALOGGING_DAYOPERATIONLOGGERCONFIG_H
#define DATALOGGING_DAYOPERATIONLOGGERCONFIG_H

#include <Global/Include/GlobalDefines.h>

#include <QString>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Class used to deliver the configuration for the DayOperation logger.
 *
 * This class can be used to deliver the configuration for the DayOperation logger
 * using the signal / slot mechanism. It contains the operating mode, serial
 * number and the max file age. If latter one is 0, no monitoring of file age
 * is done!
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class DayEventLoggerConfig {
friend class TestDayOperationLoggerConfig;
private:
    QString     m_OperatingMode;        ///< Current operating mode as string.
    QString     m_SerialNumber;         ///< Serial number.
    QString     m_Path;                 ///< Absolute path in which log files must be written.
    int         m_MaxFileCount;         ///< Maximal file count. 0 means no maximal file count monitoring!
    QString     m_BaseFileName;
    /****************************************************************************/
    /**
     * \brief Copy other instance to self.
     *
     * \iparam   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    void CopyFrom(const DayEventLoggerConfig &rOther);
protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    DayEventLoggerConfig();
    /****************************************************************************/
    /**
     * \brief Default constructor.
     *
     * \iparam   OperatingMode       Operating mode as string.
     * \iparam   SerialNumber        Serial number
     * \iparam   Path                Absolute path in which log files must be written.
     * \iparam   MaxFileCount        Maximal file count. 0 means no maximal file count monitoring!
     */
    /****************************************************************************/
    DayEventLoggerConfig(const QString &OperatingMode, const QString &SerialNumber,
                             const QString &Path, int MaxFileCount, const QString &BaseFileName);
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * Implemented due to signal / slot mechanism.
     *
     * \iparam   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    DayEventLoggerConfig(const DayEventLoggerConfig &rOther);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    ~DayEventLoggerConfig();
    /****************************************************************************/
    /**
     * \brief Assignment operator.
     *
     * \iparam   rOther  Other instance
     * \return              Const reference to self.
     */
    /****************************************************************************/
    const DayEventLoggerConfig & operator = (const DayEventLoggerConfig &rOther);
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
     * \brief Get operating mode.
     *
     * \return      Operating mode.
     */
    /****************************************************************************/
    inline QString GetBaseFileName() const {
        return m_BaseFileName;
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
     * \brief Get maximal file count.
     *
     * \return      Maximal file count.
     */
    /****************************************************************************/
    inline int GetMaxFileCount() const {
        return m_MaxFileCount;
    }
}; // end class DayEventLoggerConfig

} // end namespace DataLogging

#endif // DATALOGGING_DAYOPERATIONLOGGERCONFIG_H
