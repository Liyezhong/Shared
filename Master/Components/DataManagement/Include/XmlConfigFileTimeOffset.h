/****************************************************************************/
/*! \file XmlConfigFileTimeOffset.h
 *
 *  \brief Definition file for class XmlConfigFileTimeOffset.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-01-25
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

#ifndef DATAMANAGEMENT_XMLCONFIGFILETIMEOFFSET_H
#define DATAMANAGEMENT_XMLCONFIGFILETIMEOFFSET_H

#include <DataManagement/Include/XmlConfigFile.h>

namespace DataManager {

/****************************************************************************/
/**
 * \brief Class for reading XML based configuration file for TimeOffset.
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class XmlConfigFileTimeOffset : public XmlConfigFile {
friend class TestXmlConfigFileTimeOffset;
private:
    /****************************************************************************/
    XmlConfigFileTimeOffset(const XmlConfigFileTimeOffset &);                       ///< Not implemented.
    const XmlConfigFileTimeOffset & operator = (const XmlConfigFileTimeOffset &);   ///< Not implemented.
    /****************************************************************************/
    /**
     * \brief Read passwords for format version1.
     *
     * \param[in, out]  rReader             xml stream reader.
     * \param[in, out]  rTimeOffset         Reference to time offset.
     */
    /****************************************************************************/
    void ReadTimeOffset_V1(QXmlStreamReader &rReader, int &rTimeOffset);
protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    XmlConfigFileTimeOffset();
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~XmlConfigFileTimeOffset() {
    }
    /****************************************************************************/
    /**
     * \brief Read time offset from file.
     *
     * Read time offset from file.
     *
     * \param[in]       FileName            File from which configuration is to be read.
     * \param[in, out]  rTimeOffset         Reference to time offset.
     */
    /****************************************************************************/
    void ReadTimeOffset(const QString &FileName, int &rTimeOffset);
    /****************************************************************************/
    /**
     * \brief Write time offset to file.
     *
     * Write time offset to file.
     *
     * \param[in]   FileName            File from which configuration is to be read.
     * \param[in]   TimeOffset          Time offset.
     */
    /****************************************************************************/
    void WriteTimeOffset(const QString &FileName, int TimeOffset);
}; // end class XmlConfigFileTimeOffset

} // end namespace DataManager

#endif // DATAMANAGEMENT_XMLCONFIGFILETIMEOFFSET_H
