/****************************************************************************/
/*! \file DeviceLifeCycleRecord.cpp
 *
 *  \brief Implementation file for class DeviceLifeCycleRecord.
 *
 *  $Version:   $ 0.1
 *   $Date:    $ 07.07.2014
 *   $Author:  $ A. Yang
 *
 *  @b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2013 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "DeviceControl/Include/DeviceProcessing/DeviceLifeCycleRecord.h"
#include <QTextStream>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <Global/Include/SystemPaths.h>
#include <unistd.h>

using namespace DeviceControl;

DeviceLifeCycleRecord::DeviceLifeCycleRecord()
{
     m_DeviceLifeCycleRecordFileName = Global::SystemPaths::Instance().GetSettingsPath() + "/" + "DeviceLifeCycleRecord.xml";
}

DeviceLifeCycleRecord::~DeviceLifeCycleRecord()
{
    FreeObjects();
}

void DeviceLifeCycleRecord::ReadRecord()
{
    QDomDocument domDocument;
    QDomElement root;
    QDomElement child;
    QFile file(m_DeviceLifeCycleRecordFileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        file.close();
        return;
    }

    if (!domDocument.setContent(&file)) {
         file.close();
         return;
     }

    root = domDocument.documentElement();

    if (root.tagName() != "DeviceLifeCycles")
    {
        file.close();
        return;
    }
    else if (root.hasAttribute("Version")
               && root.attribute("Version") != "0")
    {
        file.close();
        return;
    }

    child = root.firstChildElement("DeviceList");
    if(child.isNull())
    {
          file.close();
          return;
    }

    QDomElement childDevice = child.firstChildElement("Device");
    while (!childDevice.isNull())
    {
       QString keyDevice = childDevice.attribute("Key");
       ModuleLifeCycleRecord* pModuleLifeCycleRecord = new ModuleLifeCycleRecord();

       QDomElement childFuncModule = childDevice.firstChildElement("Functionmodules").firstChildElement("Functionmodule");
       while (!childFuncModule.isNull())
       {
            QString KeyFuncModule = childFuncModule.attribute("Key");
            PartLifeCycleRecord* pPartRecord = new PartLifeCycleRecord();
            QDomElement childParameter = childFuncModule.firstChildElement("Parameter");
            while (!childParameter.isNull())
            {
                QString KeyParam = childParameter.attribute("Key");
                QString ValueParam = childParameter.attribute("Value");

                pPartRecord->m_ParamMap.insert(KeyParam, ValueParam);
                childParameter = childParameter.nextSiblingElement("Parameter");
            }

          pModuleLifeCycleRecord->m_PartLifeCycleMap.insert(KeyFuncModule, pPartRecord);
          childFuncModule = childFuncModule.nextSiblingElement("Functionmodule");
       }

       m_ModuleLifeCycleMap.insert(keyDevice, pModuleLifeCycleRecord);
       childDevice = childDevice.nextSiblingElement("Device");
    }
    file.close();
}

void DeviceLifeCycleRecord::WriteRecord()
{
    QDomDocument domDocument;
    QDomElement root;
    QFile file(m_DeviceLifeCycleRecordFileName);
    if (!file.open(QFile::WriteOnly))
    {
        file.close();
        return;
    }

    QTextStream out(&file);
    root = domDocument.createElement("DeviceLifeCycles");
    root.setAttribute("Version", 0);
    QDomElement deviceListElement  = domDocument.createElement("DeviceList");
    root.appendChild(deviceListElement);

    QMapIterator<QString, ModuleLifeCycleRecord*> iter(m_ModuleLifeCycleMap);
    while (iter.hasNext()) {
        iter.next();
        QDomElement deviceElement = domDocument.createElement("Device");
        deviceElement.setAttribute("Key", iter.key());
        deviceListElement.appendChild(deviceElement);

        QDomElement functionmodulesElement = domDocument.createElement("Functionmodules");


        ModuleLifeCycleRecord* pModuleLifeCycleRecord = iter.value();
        QMapIterator<QString, PartLifeCycleRecord*> iterPart(pModuleLifeCycleRecord->m_PartLifeCycleMap);
        while (iterPart.hasNext()) {
            iterPart.next();
            QDomElement functionmoduleElement = domDocument.createElement("Functionmodule");
            functionmoduleElement.setAttribute("Key", iterPart.key());
            PartLifeCycleRecord* partLifeCycleRecord = iterPart.value();


                    QMapIterator<QString, QString> iterParam(partLifeCycleRecord->m_ParamMap);
                    while (iterParam.hasNext()) {
                        iterParam.next();
                        QDomElement parameterElement = domDocument.createElement("Parameter");
                        parameterElement.setAttribute("Key", iterParam.key());
                        parameterElement.setAttribute("Value", iterParam.value());
                        functionmoduleElement.appendChild(parameterElement);
                    }

            functionmodulesElement.appendChild(functionmoduleElement);
        }

        deviceElement.appendChild(functionmodulesElement);

    }
    domDocument.appendChild(root);
    const int IndentSize = 4;
    domDocument.save(out, IndentSize);
    (void)file.flush();
    (void)fsync(file.handle());
    file.close();
}

void DeviceLifeCycleRecord::FreeObjects()
{
    QMapIterator<QString, ModuleLifeCycleRecord*> iter(m_ModuleLifeCycleMap);
    while (iter.hasNext()) {
        iter.next();
        ModuleLifeCycleRecord* pModuleLifeCycleRecord = iter.value();
        QMapIterator<QString, PartLifeCycleRecord*> iterPart(pModuleLifeCycleRecord->m_PartLifeCycleMap);
        while (iterPart.hasNext()) {
            iterPart.next();
            PartLifeCycleRecord* partLifeCycleRecord = iterPart.value();
            delete partLifeCycleRecord;
        }
        pModuleLifeCycleRecord->m_PartLifeCycleMap.clear();
        delete pModuleLifeCycleRecord;
    }
    m_ModuleLifeCycleMap.clear();
}


