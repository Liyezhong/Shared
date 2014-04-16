/****************************************************************************/
/*! \file ProtocolRxCommand.cpp
 *
 *  \brief ProtocolRxCommand base class implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 25.03.2011
 *   $Author:  $ Y.Novak
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

#include <NetworkComponents/Include/NetworkDevice.h>
#include <NetworkComponents/Include/ProtocolRxCommand.h>
#include <NetworkComponents/Include/ProtocolRxCommands/Ack.h>
#include <NetworkComponents/Include/ProtocolRxCommands/HeartBeatClient.h>
#include <NetworkComponents/Include/ProtocolRxCommands/HeartBeatServer.h>
#include <NetworkComponents/Include/ProtocolRxCommands/SetDateAndTime.h>
#include <NetworkComponents/Include/NetworkComponentEventCodes.h>
#include <Global/Include/EventObject.h>
#include <Global/Include/Utils.h>

namespace NetworkBase {

/// Static registration for command Ack:
DEFINE_REG_CLASS(ProtocolRxCommand, Ack);
/// Static registration for command HeartBeatClient:
DEFINE_REG_CLASS(ProtocolRxCommand, HeartBeatClient);
/// Static registration for command HeartBeatServer:
DEFINE_REG_CLASS(ProtocolRxCommand, HeartBeatServer);
/// Static registration for command SetDateAndTime:
DEFINE_REG_CLASS(ProtocolRxCommand, SetDateAndTime);

/****************************************************************************/
/*!
 *  \brief Default Constructor.
 *
 */
/****************************************************************************/
ProtocolRxCommand::ProtocolRxCommand() :
        ProtocolCommand::ProtocolCommand(),
        m_myRef(""),
        my_NOfDataitems(0)
{
    m_myAttributes.clear();
    m_myDataAttributes.clear();
    m_myMessageDataTree.clear();
    m_myPayloadArray.clear();
}

/****************************************************************************/
/*!
 *  \brief Constructor.
 *
 *  \iparam attrs = list of data attributes of this command
 */
/****************************************************************************/
ProtocolRxCommand::ProtocolRxCommand(const QStringList &attrs) :
        ProtocolCommand::ProtocolCommand(),
        m_myRef(""),
        my_NOfDataitems(0)
{
    m_myAttributes = attrs;
    m_myDataAttributes.clear();
    m_myMessageDataTree.clear();
    m_myPayloadArray.clear();
}

/****************************************************************************/
/*!
 *  \brief Constructor.
 *
 *  \iparam attrs = list of data attributes of this command
 *  \iparam dattrs = list of data attributes of each data element of this command
 */
/****************************************************************************/
ProtocolRxCommand::ProtocolRxCommand(const QStringList &attrs, const QStringList &dattrs) :
        ProtocolCommand::ProtocolCommand(),
        m_myRef(""),
        my_NOfDataitems(0)
{
    m_myAttributes = attrs;
    m_myDataAttributes = dattrs;
    m_myMessageDataTree.clear();
    m_myPayloadArray.clear();
}

/****************************************************************************/
/*!
 *  \brief Destructor.
 *
 */
/****************************************************************************/
ProtocolRxCommand::~ProtocolRxCommand()
{
    m_myDevice = NULL;
}

/****************************************************************************/
/*!
 * \brief This method must be called prior to first use of the class object.
 *
 *        Calling this method ensures that the linker will create all needed
 *        static members. Otherwise it will not. Seem to be a linker bug.
 *
 */
/****************************************************************************/
void ProtocolRxCommand::LinkerTamer()
{
     qDebug() << "ProtocolRxCommand: LinkerTamer called !";
}

/****************************************************************************/
/*!
 *  \brief   Call this function before using the Incoming command
 *
 *           This function will prepare command for execution.
 *
 *  \warning Implement this function in the derived class!
 *
 *  \param   name = command name
 *  \param   wObj = pointer to command's working object
 *
 *  \return  TRUE if initialization succeeded, FALSE otherwise
 *
 ****************************************************************************/
bool ProtocolRxCommand::Initialize(const QString &name, QObject *wObj)
{
    if (wObj == NULL) {
        return false;
    }
    m_myName = name;
    m_myDevice = (NetworkDevice*)wObj;

    // check if this is a combined message
    if (!m_myPayloadArray.isEmpty()) {
        // the header is already extracted --> check reference and return.
        if (m_myRef.isEmpty()) {
            return false;
        }
        return true;
    }

    // Prepare message for parsing:
    bool result = true;
    QDomElement msgroot= m_myMessage.documentElement();
    // extract message header:
    if (!ExtractMessageHeader(&msgroot, &m_myRef, &my_NOfDataitems)) {
        result = false;
    }

    // if no attributes were provided (as in "Ack" case for example), skip data extraction
    if (!m_myAttributes.isEmpty()) {
        // extract data from the incoming message
        if (!ExtractAttributeValues(&msgroot, &m_myAttributes, &m_myDataAttributes, &m_myMessageDataTree)) {
            result = false;
        }
    }

    if (result != true) {
        // something went wrong --> send NAck:
        static_cast<void>(
                // cannot use return value here anyway
                NetworkBase::Ack::SendAcknowledge(m_myDevice, m_myRef, m_myName, NetworkBase::CMH_MSG_SENDING_NACK)
        );
        // command returns "false" and will be destroyed without its Execute function being called.
        return false;
    }

    return result;
}

/****************************************************************************/
/*!
 *  \brief   Call this function before using the Incoming command
 *
 *           This function will prepare command for execution.
 *
 *  \warning Implement this function in the derived class!
 *
 *  \param   name = command name
 *  \param   domD = message to process in XML format
 *  \param   wObj = pointer to command's working object
 *
 *  \return  TRUE if initialization succeeded, FALSE otherwise
 *
 ****************************************************************************/
bool ProtocolRxCommand::Initialize(const QString &name, const QDomDocument &domD, QObject *wObj)
{
    m_myMessage = domD;
    return Initialize(name, wObj);
}

/****************************************************************************/
/*!
 *  \brief   Call this function before using the Incoming command
 *
 *           This function will prepare command for execution.
 *
 *  \warning Implement this function in the derived class!
 *
 *  \param   name = command name
 *  \param   ref = incoming message reference
 *  \param   byteArr = payload of the incoming message
 *  \param   wObj = pointer to command's working object
 *
 *  \return  TRUE if initialization succeeded, FALSE otherwise
 *
 ****************************************************************************/
bool ProtocolRxCommand::Initialize(const QString &name, const QString &ref, const QByteArray &byteArr, QObject *wObj)
{
    m_myRef = ref;
    m_myPayloadArray = byteArr;
    return Initialize(name, wObj);
}


/****************************************************************************/
/*!
 *  \brief   This is a test function. It prints parsed message data tree.
 *
 *  \warning Do not remove this function, it is useful for debugging.
 *
 ****************************************************************************/
void ProtocolRxCommand::PrintMessageDataTree()
{
    qDebug() << "\n==== ProtocolRxCommand: printing the extracted data tree of the incoming message : ==== \n";
    int dtreesize = m_myMessageDataTree.size();
    for (int i = 0; i < dtreesize; i++) {
        qDebug() << " > Dataitem " << (i + 1) << " of " << dtreesize << " :\n   ";
        int dataitemssize = m_myMessageDataTree[i].DataitemsValues.size();
        for (int j = 0; j < dataitemssize; j++) {
            qDebug() << (" " + m_myMessageDataTree[i].DataitemsValues[j]);
        }
        int subitemssize = m_myMessageDataTree[i].SubDataitemsValues.size();
        for (int j = 0; j < subitemssize; j++) {
            qDebug() << "   >> SubDataitem " << (j + 1) << " of " << subitemssize << " :\n      ";
            for (int k = 0; k < m_myMessageDataTree[i].SubDataitemsValues[j].size(); k++) {
                qDebug() << (" " + m_myMessageDataTree[i].SubDataitemsValues[j][k]);
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief   Extract data attribute values from the XML message
 *
 *  \iparam   Element = XML message container
 *  \iparam   Tags = list of tag names in the CMH_DATA_TAG_NAME element
 *  \iparam   DataTags = list of tag names in the CMH_DATA_TAG_NAME child elements
 *  \param[out]  Values = pointer to message data tree container
 *
 *
 *  \return  true if extraction successful, false otherwise
 *
 ****************************************************************************/
bool ProtocolRxCommand::ExtractAttributeValues(QDomElement *Element, QStringList *Tags, QStringList *DataTags, QList<MessageDataTree_t> *Values)
{
    if ((Element == NULL) || (Tags == NULL) || (DataTags == NULL) || (Values == NULL)) {
        return false;
    }
    bool result = true;

    // fetch number of CMH_DATA_TAG_NAME elements
    QDomNodeList NList = Element->elementsByTagName(CMH_DATA_TAG_NAME);
    QDomElement dataitem = Element->firstChildElement(CMH_DATA_TAG_NAME);
    MessageDataTree_t datatree;
    for (int j = 0; j < NList.size(); j++) {
        datatree.DataitemsValues.clear();
        datatree.SubDataitemsValues.clear();
        if (dataitem.attributes().size() != Tags->size()) {
            return false;
        }
        // iterate through tags
        for (int i = 0; i < Tags->size(); i++) {
            // fetch parameters:
            QString value = (dataitem.attribute((*Tags)[i], "NULL"));
            if ((value.isEmpty()) || (value == "NULL")) {
                qDebug() << "ProtocolRxCommand: attribute " << (QString)((*Tags)[i]) << " is empty !";
                datatree.DataitemsValues << "";
                result = false;
                Global::EventObject::Instance().RaiseEvent(EVENT_NL_ATTR_EMPTY,
                                                           Global::tTranslatableStringList() << (QString)((*Tags)[i])
                                                           << FILE_LINE);
            }
            else {
                datatree.DataitemsValues << value;
            }
        }
        // check if CMH_DATA_TAG_NAME element has kids
        if (dataitem.hasChildNodes()) {
            // yes, it has children. extract them
            if (!ExtractSubAttributeValues(&dataitem, DataTags, &datatree)) {
                result = false;
            }
        }
        Values->append(datatree);
        dataitem = dataitem.nextSiblingElement(CMH_DATA_TAG_NAME);
    }

    // check if number of dataitems correspond to nofitems value from the message:
    if (my_NOfDataitems != Values->size()) {
        // no, it does not
        result = false;
    }

    return result;
}

/****************************************************************************/
/*!
 *  \brief   Extract data attribute values from the XML message
 *
 *  \iparam   Item = CMH_DATA_TAG_NAME element
 *  \iparam   DataTags = list of tag names in the CMH_DATA_TAG_NAME child elements
 *  \param[out]  Values = pointer to message data tree container
 *
 *  \warning This function is only for messages where "dataitems" element
 *           contains children of the same type.
 *
 *  \return  true if extraction successful, false otherwise
 *
 ****************************************************************************/
bool ProtocolRxCommand::ExtractSubAttributeValues(QDomElement *Item, QStringList *DataTags, MessageDataTree_t *Values)
{
    if ((Item == NULL) || (DataTags == NULL) || (Values == NULL)) {
        return false;
    }

    // process attributes
    QDomElement subchild = Item->firstChildElement();
    // get the subelement tag:
    QString tagname = subchild.tagName();
    qint16 NofSubitems = 0;
    // find out how many subelements there should be
    if (!ExtractNofItems(Item, &NofSubitems, tagname, NetworkBase::CMH_DATA_TAG_NAME)) {
        return false;
    }
    QDomNodeList NList = Item->elementsByTagName(tagname);
    QString subvalue = "";
    QStringList subvalues;
    int listsize = NList.size();
    for (int j = 0; j < listsize; j++) {
        subvalues.clear();
        if (subchild.attributes().size() != DataTags->size()) {
            return false;
        }
        // iterate through tags
        for (int k = 0; k < DataTags->size(); k++) {
            // fetch parameters:
            subvalue = (subchild.attribute((*DataTags)[k], "NULL"));
            if ((subvalue.isEmpty()) || (subvalue == "NULL")) {
                qDebug() << "ProtocolRxCommand: attribute " << (QString)((*DataTags)[k]) << " is empty !";
                subvalues << "";                
                Global::EventObject::Instance().RaiseEvent(EVENT_NL_ATTR_EMPTY,
                                                           Global::tTranslatableStringList() << (QString)((*DataTags)[k])
                                                           << FILE_LINE);
                return false;
            }
            else {
                subvalues << subvalue;
            }
        }
        Values->SubDataitemsValues.append(subvalues);
        subchild = subchild.nextSiblingElement();
    }

    if (NofSubitems != Values->SubDataitemsValues.size()) {
        // number of extracted elements does not correspond to nofitems in the message
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief   Extract reference value from the XML message
 *
 *  \iparam   Element = XML message container
 *  \param[out]  Reference = the reference value to return
 *
 *  \return  true if extraction successful, false otherwise
 *
 ****************************************************************************/
bool ProtocolRxCommand::ExtractMessageReference(QDomElement *Element, QString *Reference)
{
    if ((Element == NULL) || (Reference == NULL)) {
        return false;
    }

    bool result = true;

    // fetch reference:
    QString value = (Element->firstChildElement(NetworkBase::CMH_CMD_TAG_NAME)).attribute(NetworkBase::CMH_REFERENCE, "NULL");
    if ((value.isEmpty()) || (value == "NULL")) {
        qDebug() << "ProtocolRxCommand: attribute " << NetworkBase::CMH_REFERENCE << " is empty !";
        *Reference = "";
        result = false;
        Global::EventObject::Instance().RaiseEvent(EVENT_NL_ATTR_EMPTY,
                                                   Global::tTranslatableStringList() << NetworkBase::CMH_REFERENCE
                                                   << FILE_LINE);

    }
    else {
        *Reference = value;
    }

    return result;
}

/****************************************************************************/
/*!
 *  \brief   Extract nofitmes attribute from XML element
 *
 *  \iparam   Element = XML container
 *  \param[out]  NofItems = the value of number of payload items to return
 *  \iparam   targettag = tag of the sub elements
 *  \iparam   sourcetag = tag of the parent element
 *
 *  \return  true if extraction successful, false otherwise
 *
 ****************************************************************************/
bool ProtocolRxCommand::ExtractNofItems(QDomElement *Element, qint16 *NofItems, const QString &targettag, const QString &sourcetag)
{
    if ((Element == NULL) || (NofItems == NULL)) {
        return false;
    }

    if (Element->firstChildElement(targettag).isNull()) {
        *NofItems = 0;
        return true;
    }

    bool result = true;

    // fetch the value
    QString value = "";
    if (targettag == NetworkBase::CMH_DATA_TAG_NAME) {
        value = (Element->firstChildElement(sourcetag)).attribute(NetworkBase::CMH_NOFITEMS, "NULL");
    }
    else {
        value = (Element->attribute(NetworkBase::CMH_NOFITEMS, "NULL"));
    }

    // check the extracted value
    if (value == "NULL") {
        *NofItems = 1;
    }
    else {
        if (value == "") {
            *NofItems = 0;
            result = false;
        }
        else {
            bool f1 = false;
            *NofItems = (quint16)value.toUShort(&f1, 10);
            if (!f1) {
                *NofItems = 0;
                result = false;
            }
        }
    }

    return result;
}

/****************************************************************************/
/*!
 *  \brief   Extract header from the XML message
 *
 *  \iparam   Element = XML message container
 *  \param[out]  Ref = XML message reference
 *  \param[out]  NofItems = number of payload items in the XML message
 *
 *  \return  true if extraction successful, false otherwise
 *
 ****************************************************************************/
bool ProtocolRxCommand::ExtractMessageHeader(QDomElement *Element, QString *Ref, qint16 *NofItems)
{
    if ((Element == NULL) || (Ref == NULL) || (NofItems == NULL)) {
        return false;
    }
    bool result = true;
    result = ExtractMessageReference(Element, Ref);
    result = result && ExtractNofItems(Element, NofItems, NetworkBase::CMH_DATA_TAG_NAME, NetworkBase::CMH_CMD_TAG_NAME);

    return result;
}

/****************************************************************************/
/*!
 *  \brief   Check and act on command execution status
 *
 *  \iparam  status = command execution status
 *
 *  \return  true if execution successful, false otherwise
 *
 ****************************************************************************/
bool ProtocolRxCommand::CheckStatus(const QString &status)
{
    if (status != NetworkBase::CMH_MSG_SENDING_ACK) {
        // something went wrong --> Send NAck:
        return NetworkBase::Ack::SendAcknowledge(m_myDevice, m_myRef, m_myName, status);
    }
    return true;
}

} // end namespace NetworkBase
