/****************************************************************************/
/*! \file DataLogging/Include/ComponentTestEntry.h
 *
 *  \brief Definition file for class ComponentTestEntry.
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

#ifndef DATALOGGING_COMPONENTTESTENTRY_H
#define DATALOGGING_COMPONENTTESTENTRY_H

#include <Global/Include/TranslatableString.h>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Class used to deliver the content of a component test result.
 *
 * This class can be used to deliver the content for a component test result using the
 * signal / slot mechanism. It contains the tester name, version numbers, etc.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class ComponentTestEntry {
friend class TestComponentTestEntry;
private:
    QString                             m_ComponentName;                ///< Name of the component.
    QString                             m_BoardName;                    ///< Name of the board.
    QString                             m_BoardSerialNumber;            ///< Serial number of the board.
    QString                             m_TesterName;                   ///< Name of the tester.
    QString                             m_DeviceSerialNumber;           ///< Serial number of whole device.
    QString                             m_ComponentSerialNumber;        ///< Serial number of the tested component.
    QString                             m_ComponentFirmwareVersion;     ///< Firmware version number of the tested component.
    QString                             m_ComponentHardwareVersion;     ///< Hardware version number of the tested component.
    QString                             m_MainboardSoftwareVersion;     ///< Software version number of the mainboard.
    Global::tTranslatableStringList     m_TestResults;                  ///< Content of component test result.
    /****************************************************************************/
    /**
     * \brief Copy other instance to self.
     *
     * \param[in]   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    void CopyFrom(const ComponentTestEntry &rOther);
protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     *
     * Implemented due to signal / slot mechanism.
     */
    /****************************************************************************/
    ComponentTestEntry();
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * Implemented due to signal / slot mechanism.
     *
     * \param[in]   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    ComponentTestEntry(const ComponentTestEntry &rOther);
    /****************************************************************************/
    /**
     * \brief Constructor with an argument list.
     *
     * \param[in]   ComponentName               Name of the component.
     * \param[in]   BoardName                   Name of the board.
     * \param[in]   BoardSerialNumber           Serial number of the board.
     * \param[in]   TesterName                  Name of the tester.
     * \param[in]   DeviceSerialNumber          Serial number of whole device.
     * \param[in]   ComponentSerialNumber       Serial number of the tested component.
     * \param[in]   ComponentFirmwareVersion    Firmware version number of the tested component.
     * \param[in]   ComponentHardwareVersion    Hardware version number of the tested component.
     * \param[in]   MainboardSoftwareVersion    Software version number of the mainboard.
     * \param[in]   TestResults                 Content of component test.
     */
    /****************************************************************************/
    ComponentTestEntry( const QString &ComponentName, const QString &BoardName,
                        const QString &BoardSerialNumber,
                        const QString &TesterName, const QString &DeviceSerialNumber,
                        const QString &ComponentSerialNumber, const QString &ComponentFirmwareVersion,
                        const QString &ComponentHardwareVersion, const QString &MainboardSoftwareVersion,
                        const Global::tTranslatableStringList &TestResults);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    ~ComponentTestEntry();
    /****************************************************************************/
    /**
     * \brief Assignment operator.
     *
     * \param[in]   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    const ComponentTestEntry & operator = (const ComponentTestEntry &rOther);
    /****************************************************************************/
    /**
     * \brief Get the name of the component.
     *
     * \return      Name of the component.
     */
    /****************************************************************************/
    inline QString GetComponentName() const {
        return m_ComponentName;
    }
    /****************************************************************************/
    /**
     * \brief Get the name of the board.
     *
     * \return      Name of the board.
     */
    /****************************************************************************/
    inline QString GetBoardName() const {
        return m_BoardName;
    }
    /****************************************************************************/
    /**
     * \brief Get the serial number of the board..
     *
     * \return      Serial number of the board..
     */
    /****************************************************************************/
    inline QString GetBoardSerialNumber() const {
        return m_BoardSerialNumber;
    }
    /****************************************************************************/
    /**
     * \brief Get the name of the tester.
     *
     * \return      Name of the tester.
     */
    /****************************************************************************/
    inline QString GetTesterName() const {
        return m_TesterName;
    }
    /****************************************************************************/
    /**
     * \brief Get the serial number of whole device.
     *
     * \return      Serial number of whole device.
     */
    /****************************************************************************/
    inline QString GetDeviceSerialNumber() const {
        return m_DeviceSerialNumber;
    }
    /****************************************************************************/
    /**
     * \brief Get the serial number of the tested component.
     *
     * \return      Serial number of the tested component.
     */
    /****************************************************************************/
    inline QString GetComponentSerialNumber() const {
        return m_ComponentSerialNumber;
    }
    /****************************************************************************/
    /**
     * \brief Get the firmware version number of the tested component.
     *
     * \return      Firmware version number of the tested component.
     */
    /****************************************************************************/
    inline QString GetComponentFirmwareVersion() const {
        return m_ComponentFirmwareVersion;
    }
    /****************************************************************************/
    /**
     * \brief Get the hardware version number of the tested component.
     *
     * \return      Hardware version number of the tested component.
     */
    /****************************************************************************/
    inline QString GetComponentHardwareVersion() const {
        return m_ComponentHardwareVersion;
    }
    /****************************************************************************/
    /**
     * \brief Get the software version number of the mainboard.
     *
     * \return      Software version number of the mainboard.
     */
    /****************************************************************************/
    inline QString GetMainboardSoftwareVersion() const {
        return m_MainboardSoftwareVersion;
    }
    /****************************************************************************/
    /**
     * \brief Get the test results as const reference.
     *
     * \return      Test results as const reference.
     */
    /****************************************************************************/
    inline const Global::tTranslatableStringList &GetTestResults() const {
        return m_TestResults;
    }
    /****************************************************************************/
    /**
     * \brief Append new line to test results.
     *
     * \param[in]   Line    Line to append.
     */
    /****************************************************************************/
    inline void AppendLineToTestResults(Global::tTranslatableStringList Line) {
        m_TestResults.append(Line);
    }
}; // end class ComponentTestEntry

} // end namespace DataLogging


#endif // DATALOGGING_COMPONENTTESTENTRY_H
