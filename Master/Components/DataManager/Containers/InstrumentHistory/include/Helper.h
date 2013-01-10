/****************************************************************************/
/*! \file Helper.h
 *
 *  \brief Definition file for general purpose functions.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-12-20
 *  $Author:    $ Soumya. D
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

#ifndef DATAMANAGER_HELPER_H
#define DATAMANAGER_HELPER_H

#include <QXmlStreamReader>

namespace DataManager
{

class Helper
{
public:
    Helper();

    /****************************************************************************/
    /*!
     *  \brief  to read node from XML File
     *  \return true on success, false on failure
     */
    /****************************************************************************/
    static bool ReadNode(QXmlStreamReader& XmlStreamReader, QString NodeName);
};

} // namespace DataManager

#endif // DATAMANAGER_HELPER_H
