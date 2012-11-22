/****************************************************************************/
/*! \file DataLogging/Include/ComponentTestLogger.h
 *
 *  \brief Definition file for class ComponentTestLogger.
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

#ifndef DATALOGGING_COMPONENTTESTLOGGER_H
#define DATALOGGING_COMPONENTTESTLOGGER_H

#include <DataLogging/Include/BaseLogger.h>
#include <DataLogging/Include/ComponentTestLoggerConfig.h>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief This class writes the content of a component test run to file.
 *
 * <b>This class should be used only with the signal / slot mechanism.</b>
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class ComponentTestLogger : public BaseLogger {
private:
    ComponentTestLoggerConfig       m_Config;   ///< Configuration.
    /****************************************************************************/
    ComponentTestLogger();      ///< Not implemented.
protected:
    /****************************************************************************/
    /**
     * \brief Write header information.
     *
     * The header information depends of the component test entry.
     *
     * \param[in]   Entry   The component test entry.
     */
    /****************************************************************************/
    void WriteHeader(const ComponentTestEntry &Entry);
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   pParent             Parent.
     * \param[in]   TheLoggingSource    Source to set in log entry.
     */
    /****************************************************************************/
    ComponentTestLogger(LoggingObject *pParent, const Global::LoggingSource &TheLoggingSource);
    /****************************************************************************/
    /**
     * \brief Destructor.
     *
     * All open files will be closed.
     */
    /****************************************************************************/
    virtual ~ComponentTestLogger() {
    }
    /****************************************************************************/
    /**
     * \brief Log a component test.
     *
     * \param[in]   Entry       Event entry to log.
     */
    /****************************************************************************/
    void Log(const DataLogging::ComponentTestEntry &Entry);
    /****************************************************************************/
    /**
     * \brief Change configuration.
     *
     * After the configuration is saved switch to new log file.
     *
     * \param[in]   Config      New configuration for event logger.
     */
    /****************************************************************************/
    void Configure(const DataLogging::ComponentTestLoggerConfig &Config);
}; // end class ComponentTestLogger

} // end namespace DataLogging

#endif // DATALOGGING_COMPONENTTESTLOGGER_H
