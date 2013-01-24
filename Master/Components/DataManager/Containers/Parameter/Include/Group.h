/****************************************************************************/
/*! \file Group.h
 *
 *  \brief Definition file for class CGroup.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-03-01
 *  $Author:    $ Thirumalesha R
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

#ifndef DATAMANAGER_GROUP_H
#define DATAMANAGER_GROUP_H

#include <QString>
#include <QHash>

#include "ColoradoDataManager/Containers/Parameter/Include/Parameter.h"

namespace DataManager {

typedef QHash<QString, CParameter*> ListOfParameter_t; //!< Parameter Step List
/****************************************************************************/
/*!
 *  \brief   This is the base class for Group
 */
/****************************************************************************/
class CGroup
{

private:
    QString m_Key;                              //!< Group Key
    ListOfParameter_t m_ParameterList;          //!< Parameter List
    ListOfKeys_t m_OrderedListOfParameterKeys;  //!< Ordered list of Group Key's
public:
    CGroup();
    CGroup(const QString Key);
    virtual ~CGroup();
    void Init();

    /****************************************************************************/
    /*!
     *  \brief Copy constructor
     *
     *  \iparam SourceGroup = const reference to SourceGroup object
     */
    /****************************************************************************/
    CGroup(const CGroup& SourceGroup)
    {
        Q_UNUSED(SourceGroup);
        //!< Empty since data is getting copied in the derived class assingment operator func.
    }

    //!< Made protected so that client classes don't call this explicitly.
    /****************************************************************************/
    /*!
     *  \brief Assignment operator
     *
     *  \iparam Group = const reference to Group object
     */
    /****************************************************************************/
    CGroup& operator = (const CGroup& Group)
    {
        Q_UNUSED(Group)
        return *this;
        //!< Empty since data is getting copied in the derived class assingment operator func.
        //!< Defined so that compiler wont provide one !
    }

    /****************************************************************************/
    /*!
     *  \brief  Retrieve the Group Key
     *
     *  \return Group Key
     */
    /****************************************************************************/
     QString GetGroupKey() const {return m_Key;}

    /****************************************************************************/
    /*!
     *  \brief Sets the Group Key
     *
     *  \iparam Value = Group Key
     */
    /****************************************************************************/
     void SetGroupKey(const QString GroupKey){m_Key = GroupKey;}

    /****************************************************************************/
    /*!
     *  \brief Retrieves list of Group steps in a Group
     *
     *  \return Step list
     */
    /****************************************************************************/
     ListOfParameter_t* GetParameterList()
     {
        return &m_ParameterList;
     }

    /****************************************************************************/
    /*!
     *  \brief  Retrieve the Group step count
     *
     *  \return Group step count
     */
    /****************************************************************************/
     int  GetNumberOfParameter() const {return m_ParameterList.count();}

    /****************************************************************************/
    /*!
     *  \brief  Creates a Parameter
     *
     *  \return Parameter
     */
    /****************************************************************************/
     CParameter* CreateParameter() {return new CParameter();}

    /****************************************************************************/
    /*!
     *  \brief  Creates a Group step for the given Parameter Key
     *  \iparam ParameterKey = Key of the Parameter
     *  \return Parameter
     */
    /****************************************************************************/
     CParameter* CreateParameter(const QString ParameterKey) {return new CParameter(ParameterKey);} // given Key will be used

    /****************************************************************************/
    /*!
     *  \brief  Retrieves the Group step associated with the given Key
     *  \iparam Parameter Key = Key of the Parameter
     *  \return Parmater
     */
    /****************************************************************************/
    const CParameter* GetParameter(const QString ParameterKey) const {return m_ParameterList.value(ParameterKey, NULL);}  // uses unique Key

    /****************************************************************************/
    /*!
     *  \brief  Retrieve the Parameter step associated with given index
     *
     *  \return Parameter
     */
    /****************************************************************************/
    const CParameter* GetParameter(const unsigned int Index) const; // uses order index
    bool GetParameter(const unsigned int Index, CParameter& Parameter) const;
    bool AddParameter(CParameter* p_Parameter);   // p_Parameter is added directly => don't delete outside!
    bool AddParameterOffset(CParameter* p_Parameter,int ParameterOffset );  // Add offset to gevine parmater
    bool ResetParameterOffset(CParameter* p_Parameter);       // Set parameter offset value to zero.
    bool UpdateParameter(CParameter* p_Parameter);  // content of p_Parameter will be copied  => delete outside!
    bool DeleteParameter(const QString ParameterKey);   // uses unique Key
    virtual bool DeleteParameter(const unsigned int Index);  // uses order index
    bool DeleteAllParameter();
    bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);
    bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);
    bool MoveParameter(qint32 FromIndex, qint32 ToIndex); // moves Parameter inside a Parameter list

};

} // namespace DataManager

#endif // DATAMANAGER_GROUP_H
