/****************************************************************************/
/*! \file DataLogging/Include/ComponentTestLoggerConfig.h
 *
 *  \brief Definition file for class ComponentTestLoggerConfig.
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

#ifndef DATALOGGING_COMPONENTTESTLOGGERCONFIG_H
#define DATALOGGING_COMPONENTTESTLOGGERCONFIG_H

#include <Global/Include/GlobalDefines.h>

#include <QString>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Class used to deliver the configuration for the component test logger.
 *
 * This class can be used to deliver the configuration for the component test logger
 * using the signal / slot mechanism. It contains the path where the file has to be written.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class ComponentTestLoggerConfig {
friend class TestComponentTestLoggerConfig;
private:
    QString     m_OperatingMode;    ///< Current operating mode as string^.
    QString     m_Path;             ///< Absolute path in which the file must be written.
    /****************************************************************************/
    /**
     * \brief Copy other instance to self.
     *
     * \param[in]   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    void CopyFrom(const ComponentTestLoggerConfig &rOther);
protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     *
     */
    /****************************************************************************/
    ComponentTestLoggerConfig();
    /****************************************************************************/
    /**
     * \brief Default constructor.
     *
     * \param[in]   OperatingMode   Operating mode as string.
     * \param[in]   Path            Absolute path in which the file must be written.
     */
    /****************************************************************************/
    ComponentTestLoggerConfig(const QString &OperatingMode, const QString &Path);
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * Implemented due to signal / slot mechanism.
     *
     * \param[in]   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    ComponentTestLoggerConfig(const ComponentTestLoggerConfig &rOther);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    ~ComponentTestLoggerConfig();
    /****************************************************************************/
    /**
     * \brief Assignment operator.
     *
     * \param[in]   rOther  Other instance
     * \return              Const reference to self.
     */
    /****************************************************************************/
    const ComponentTestLoggerConfig & operator = (const ComponentTestLoggerConfig &rOther);
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
     * \brief Get absolute path in which log files must be written.
     *
     * \return      Path.
     */
    /****************************************************************************/
    inline QString GetPath() const {
        return m_Path;
    }
}; // end class ComponentTestLoggerConfig

} // end namespace DataLogging

#endif // DATALOGGING_COMPONENTTESTLOGGERCONFIG_H
