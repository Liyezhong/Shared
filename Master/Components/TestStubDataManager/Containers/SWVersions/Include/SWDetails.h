/****************************************************************************/
/*! \file SWDetails.cpp
 *
 *  \brief Implementation file for class CSWDetails.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-09-07
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
#ifndef DATAMANAGER_SWDETAILS_H
#define DATAMANAGER_SWDETAILS_H

#include <QStringList>
#include <QXmlStreamWriter>
#include "TestStubDataManager/Helper/Include/Types.h"
#include "TestStubDataManager/Helper/Include/Helper.h"


namespace DataManager {


/****************************************************************************/
/*!
 *  \brief  This class implements configuration which consists of number
 *  of files and the attributes to the number of files
 */
/****************************************************************************/
class CSWDetails {

    friend class CSWVersionList;

private:
    QString m_SWName;    ///< Store the software file name
    QString m_SWVersion; ///< Store the software version number
    QString m_SWDate;    ///< Store the tag name for the xml
    SWType_t m_SWType;   ///< Store the SW type

    bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);
    bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);


public:
    CSWDetails();
    CSWDetails(const QString SWName);

    CSWDetails(const CSWDetails& SWDetails);

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CSWDetails& SWDetails);
    friend QDataStream& operator >>(QDataStream& InDataStream, CSWDetails& SWDetails);
    CSWDetails& operator=(const CSWDetails&);

    /****************************************************************************/
    /*!
     *  \brief Retrieve the sw file name
     *  \return file name of the sw
     */
    /****************************************************************************/
    QString GetSWName() {return m_SWName;}

    /****************************************************************************/
    /*!
     *  \brief Sets the sw name
     *
     *  \iparam SWName = file name of the sw
     */
    /****************************************************************************/
    void SetSWName(const QString SWName) {m_SWName = SWName;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve the sw version
     *  \return version of the sw
     */
    /****************************************************************************/
    QString GetSWVersion() {return m_SWVersion;}

    /****************************************************************************/
    /*!
     *  \brief Sets the sw version
     *
     *  \iparam SWVersion = sw version of the file
     */
    /****************************************************************************/
    void SetSWVersion(const QString SWVersion) {m_SWVersion = SWVersion;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve the sw date
     *  \return date of the file
     */
    /****************************************************************************/
    QString GetSWDate() {return m_SWDate;}

    /****************************************************************************/
    /*!
     *  \brief Sets the SW date
     *
     *  \iparam SWDate = date of which sw is created
     */
    /****************************************************************************/
    void SetSWDate(const QString SWDate) {m_SWDate = SWDate;}


    /****************************************************************************/
    /*!
     *  \brief Retrieve the software type
     *  \return software type
     */
    /****************************************************************************/
    SWType_t GetSWType() {return m_SWType;}

    /****************************************************************************/
    /*!
     *  \brief Sets the software type
     *
     *  \iparam SWType = enum for the software type
     */
    /****************************************************************************/
    void SetSWType(const SWType_t SWType) {m_SWType = SWType;}

    /****************************************************************************/
    /*!
     *  \brief Get the SW version number from the SW version
     *  \return version number
     */
    /****************************************************************************/
    inline float GetSWVersionNumber()
    {
        // check whether SW consists of the "_" character
        if (m_SWVersion.split("_").count() > 1) {
            // return the second one as version number and convert it into float
            return m_SWVersion.split("_").value(1).toFloat();
        }
        else {
            // conver the version number to float
            return m_SWVersion.toFloat();
        }
    }

};
}


#endif // DATAMANAGER_SWDETAILS_H
