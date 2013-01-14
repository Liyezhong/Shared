/****************************************************************************/
/*! \file Helper.cpp
 *
 *  \brief Implementation file for general purpose functions.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-12-21
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

#include "DataManager/Containers/InstrumentHistory/Include/Helper.h"
#include <QDebug>

namespace DataManager
{

/****************************************************************************/
/*!
 *  \brief Default Constructor
 */
/****************************************************************************/
Helper::Helper()
{

}

/****************************************************************************/
/*!
 *  \brief Searches for the node and returns true if found
 *
 *  \iparam XmlStreamReader = Instance of the QXmlStreamReader
 *  \iparam NodeName = Name of the node in the Xml file
 *
 *  \return True or False
 */
/****************************************************************************/
bool Helper::ReadNode(QXmlStreamReader& XmlStreamReader, QString NodeName)
{
    // look for node <NodeName>
    if (XmlStreamReader.atEnd()) {
        qDebug() << "Helper::ReadNode, nothing to read since we are at the end already" << NodeName;
    }

    bool Result = false;
    while (!XmlStreamReader.atEnd())
    {
        if (XmlStreamReader.readNext() == QXmlStreamReader::Invalid) {
            qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
        }

        if (XmlStreamReader.isStartElement()) {
            if (XmlStreamReader.name() == NodeName) {
                Result = true;
                break;
            }
            else {
                // Assert has to be called here
                Q_ASSERT(Result == false);
            }
        }
    }
    if (!Result) {
        qDebug() << "Helper::ReadNode failed. Node not found: " << NodeName;
    }
    return Result;
}

} // namespace DataManager
