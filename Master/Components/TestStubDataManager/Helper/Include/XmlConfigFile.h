/****************************************************************************/
/*! \file XmlConfigFile.h
 *
 *  \brief Definition file for class XmlConfigFile.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-08-02
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

#ifndef DATAMANAGEMENT_XMLCONFIGFILE_H
#define DATAMANAGEMENT_XMLCONFIGFILE_H

#include <Global/Include/GlobalDefines.h>

#include <QXmlStreamReader>
#include <QLocale>
#include <QFile>

namespace DataManager {

/****************************************************************************/
/**
 * \brief Base class for XML based configuration files.
 *
 * It provides some methods for reading / writing xml files.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class XmlConfigFile {
friend class TestXmlConfigFile;
private:
    /****************************************************************************/
    XmlConfigFile(const XmlConfigFile &);                       ///< Not implemented.
    const XmlConfigFile & operator = (const XmlConfigFile &);   ///< Not implemented.
protected:
    /****************************************************************************/
    /**
     * \brief Initialize stream reader.
     *
     * \param[in, out]  rReader     Reference to the stream reader.
     * \param[in, out]  rFile       Reference to IO device.
     * \param[in]       FileName    File name.
     */
    /****************************************************************************/
    void InitStreamReader(QXmlStreamReader &rReader, QFile &rFile, const QString &FileName);
    /****************************************************************************/
    /**
     * \brief Initialize stream reader.
     *
     * \param[in, out]  rReader     Reference to the stream reader.
     * \param[in, out]  rDevice     Reference to IO device.
     */
    /****************************************************************************/
    void InitStreamReader(QXmlStreamReader &rReader, QIODevice &rDevice);
    /****************************************************************************/
    /**
     * \brief Initialize stream writer.
     *
     * \param[in, out]  rWriter     Reference to the stream writer.
     * \param[in, out]  rFile       Reference to IO device.
     * \param[in]       FileName    File name.
     */
    /****************************************************************************/
    void InitStreamWriter(QXmlStreamWriter &rWriter, QFile &rFile, const QString &FileName);
    /****************************************************************************/
    /**
     * \brief Initialize stream writer.
     *
     * \param[in, out]  rWriter     Reference to the stream writer.
     * \param[in, out]  rDevice     Reference to IO device.
     */
    /****************************************************************************/
    void InitStreamWriter(QXmlStreamWriter &rWriter, QIODevice &rDevice);
    /****************************************************************************/
    /**
     * \brief Read attribute as string.
     *
     * \param[in, out]  rReader         Reference to the stream reader.
     * \param[in]       AttributeName   Attribute name.
     * \return                          Attribute value.
     */
    /****************************************************************************/
    QString ReadAttributeString(QXmlStreamReader &rReader, const QString &AttributeName);
    /****************************************************************************/
    /**
     * \brief Read attribute as quint32.
     *
     * \param[in, out]  rReader         Reference to the stream reader.
     * \param[in]       AttributeName   Attribute name.
     * \return                          Attribute value.
     */
    /****************************************************************************/
    quint32 ReadAttributequint32(QXmlStreamReader &rReader, const QString &AttributeName);
    /****************************************************************************/
    /**
     * \brief Read attribute as QLocale::Language.
     *
     * \param[in, out]  rReader         Reference to the stream reader.
     * \param[in]       AttributeName   Attribute name.
     * \return                          Attribute value.
     */
    /****************************************************************************/
    QLocale::Language ReadAttributeLanguage(QXmlStreamReader &rReader, const QString &AttributeName);
    /****************************************************************************/
    /**
     * \brief Read attribute as OnOffState.
     *
     * \param[in, out]  rReader         Reference to the stream reader.
     * \param[in]       AttributeName   Attribute name.
     * \return                          Attribute value.
     */
    /****************************************************************************/
    Global::OnOffState ReadAttributeOnOff(QXmlStreamReader &rReader, const QString &AttributeName);
    /****************************************************************************/
    /**
     * \brief Read next start element.
     *
     * \param[in, out]  rReader         Reference to the stream reader.
     * \param[in]       ElementName     Expected element name.
     */
    /****************************************************************************/
    void ReadStartElement(QXmlStreamReader &rReader, const QString &ElementName);
    /****************************************************************************/
    /**
     * \brief Read format version.
     *
     * Reads the start element with name RootTagName and then tries to read
     * the attribute with name "version" as string.
     *
     * \param[in, out]  rReader         Reference to the stream reader.
     * \param[in]       RootTagName     Root tag name.
     * \return                          Version.
     */
    /****************************************************************************/
    QString ReadFormatVersion(QXmlStreamReader &rReader, const QString &RootTagName);
    /****************************************************************************/
    /**
     * \brief Write format version.
     *
     * Writes the start element with name RootTagName and then writes
     * the attribute with name "version" as string.
     *
     * \param[in, out]  rWriter         Reference to the stream writer.
     * \param[in]       RootTagName     Root tag name.
     * \param[in]       Version         Version.
     */
    /****************************************************************************/
    void WriteFormatVersion(QXmlStreamWriter &rWriter, const QString &RootTagName, const QString &Version);
public:
    /****************************************************************************/
    /**
     * \brief Default Constructor.
     */
    /****************************************************************************/
    XmlConfigFile();
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~XmlConfigFile();
}; // end class XmlConfigFile

} // end namespace DataManager

#endif // DATAMANAGEMENT_XMLCONFIGFILE_H
