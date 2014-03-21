/****************************************************************************/
/*! \file XmlConfigFileStrings.h
 *
 *  \brief Definition file for class XmlConfigFileStrings.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-08-17
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

#ifndef DATAMANAGEMENT_XMLCONFIGFILESTRINGS_H
#define DATAMANAGEMENT_XMLCONFIGFILESTRINGS_H

#include <TestStubDataManager/Helper/Include/XmlConfigFile.h>
#include <Global/Include/Translator.h>

#include <QSet>

namespace DataManager {

/****************************************************************************/
/**
 * \brief Class for reading XML based configuration file for strings.
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class XmlConfigFileStrings : public XmlConfigFile {
friend class TestXmlConfigFileStrings;
private:
    Global::tTranslations   m_Data;     ///< Data that was read.
    /****************************************************************************/
    XmlConfigFileStrings(const XmlConfigFileStrings &);                         ///< Not implemented.
    const XmlConfigFileStrings & operator = (const XmlConfigFileStrings &);     ///< Not implemented.
    /****************************************************************************/
    /**
     * \brief Cleanup all memorized data from other read actions.
     */
    /****************************************************************************/
    virtual void ResetData();
    /****************************************************************************/
    /**
     * \brief Read element of type "string" for format version 1.
     *
     * \param[in, out]  rReader         xml stream reader.
     * \param[in, out]  rLanguageData   Language data.
     */
    /****************************************************************************/
    void ReadStringEntry_V1(QXmlStreamReader &rReader, Global::tLanguageData &rLanguageData);
    /****************************************************************************/
    /**
     * \brief Read element of type "language" for format version 1.
     *
     * \param[in, out]  rReader         xml stream reader.
     * \param[in, out]  rLanguageList   List of languages to read.
     */
    /****************************************************************************/
    void ReadLanguage_V1(QXmlStreamReader &rReader, QSet<QLocale::Language> &rLanguageList);
    /****************************************************************************/
    /**
     * \brief Read strings for format version1.
     *
     * \param[in, out]  rReader         xml stream reader.
     * \param[in, out]  rLanguageList   List of languages to read.
     */
    /****************************************************************************/
    void ReadStrings_V1(QXmlStreamReader &rReader, QSet<QLocale::Language> &rLanguageList);
protected:
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     */
    /****************************************************************************/
    XmlConfigFileStrings() {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~XmlConfigFileStrings() {
    }
    /****************************************************************************/
    /**
     * \brief Get const reference to data.
     *
     * \return  Const reference to data.
     */
    /****************************************************************************/
    const Global::tTranslations &Data() const {
        return m_Data;
    }
    /****************************************************************************/
    /**
     * \brief Read configuration from file.
     *
     * Read strings for a list of languages.
     *
     * \param[in]       FileName        File from which configuration is to be read.
     * \param[in, out]  rLanguageList   List of languages to read.
     */
    /****************************************************************************/
    void ReadStrings(const QString &FileName, QSet<QLocale::Language> &rLanguageList);
}; // end class XmlConfigFileStrings

} // end namespace DataManager

#endif // DATAMANAGEMENT_XMLCONFIGFILESTRINGS_H
