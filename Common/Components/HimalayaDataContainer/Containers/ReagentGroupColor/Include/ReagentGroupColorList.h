
/****************************************************************************/
/*! \file ReagentGroupColorList.h
 *
 *  \brief Definition file for class CReagentGroupColorList.
 *
 *   $Version: $ 0.3
 *   $Date:    $ 2013-03-04
 *   $Author:  $ Swati Tiwari
 *
 *  \b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2012 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef DATAMANAGER_REAGENTGROUPCOLORLIST_H
#define DATAMANAGER_REAGENTGROUPCOLORLIST_H


#include "DataManager/Helper/Include/Types.h"
#include "HimalayaDataContainer/Containers/ReagentGroupColor/Include/ReagentGroupColor.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"
#include "HimalayaDataContainer/Helper/Include/HimalayaDataManagerEventCodes.h"
#include "Global/Include/EventObject.h"


namespace DataManager {

typedef QHash<QString, CReagentGroupColor*> ListOfGroupColor_t;     //!< List of Reagents

/****************************************************************************/
/*!
 *  \brief CReagentGroupColorList class Inherited from CDataContainerBase
 */
/****************************************************************************/
class CReagentGroupColorList : public CDataContainerBase
{
    friend class CSpecialVerifierGroupA;

private:
    int m_Version;                              //!< version of the file read
    bool m_DataVerificationMode;                //!< Verification mode flag , verify the Container
    QString m_NextReagentID;                    //!< next Free Reagent ID of USER
    QString m_Filename;                         //!< filename of the file

    ListOfGroupColor_t m_ReagentGroupColorList;             //!< Reagent list

    ListOfIDs_t m_OrderedListOfReagentGroupColorIDs;      //!< Ordered list of Reagent ID

    QReadWriteLock* mp_ReadWriteLock;           //!< Reade Write lock


    ListOfIDs_t m_OrderedListOfReagentColorIDs;       ///<  Definition/Declaration of variable m_OrderedListOfReagentColorIDs

    ErrorMap_t m_ErrorHash;    //!< Event List for GUI and for logging purpose. This member is not copied when using copy constructor/Assignment operator

    bool SerializeContent(QIODevice& IODevice, bool CompleteData);
    bool DeserializeContent(QIODevice& IODevice, bool CompleteData);

    void SetDefaultAttributes();
    bool DeleteAllReagentsGroupColor();
    bool DeleteReagentGroupColor(const unsigned int Index);
    bool DeleteReagentGroupColor(const QString ID);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CheckForUniqueName
     *
     *  \param ID = QString type parameter
     *  \param ReagentName =  QString type parameter
     *
     *  \return from CheckForUniqueName
     */
    /****************************************************************************/
    bool CheckForUniqueName(QString ID, QString ReagentName);
    bool ReadCompleteData(QXmlStreamReader& XmlStreamReader);

    /******************** INLINE FuNCTIONS **************************************/

    /****************************************************************************/
    /*!
     *  \brief Sets the version of The ReagentGroupColor list File
     *
     *  \iparam Value = Version number
     *
     *  \return
     */
    /****************************************************************************/
    void SetVersion(const int Value){m_Version = Value;}

public:
    CReagentGroupColorList();
    ~CReagentGroupColorList();

    CReagentGroupColorList(const CReagentGroupColorList&);
    friend QDataStream& operator <<(QDataStream& OutDataStream, const CReagentGroupColorList& ReagentList);
    friend QDataStream& operator >>(QDataStream& InDataStream, CReagentGroupColorList& ReagentList);
    CReagentGroupColorList& operator=(const CReagentGroupColorList&);
    void Init();

    bool Read(QString Filename);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetCReagentGroupColor
     *
     *  \param ID = const QString type parameter
     *
     *  \return from GetCReagentGroupColor
     */
    /****************************************************************************/
    CReagentGroupColor* GetCReagentGroupColor(const QString ID) {return m_ReagentGroupColorList.value(ID, NULL);}  // uses unique step ID

    bool AddReagentColorGroup (const CReagentGroupColor* p_ReagentGroupColor);

    HashOfHeatingStartMode_t HashOfHeatingStartModes;   //!< heating start modes AtDeviceStart and AtProgramStart

    /******************** INLINE FuNCTIONS **************************************/
    /****************************************************************************/
    /*!
     *  \brief Get's the Data Container Type for reagent
     *
     *  \return Datacontainer Type
     */
    /****************************************************************************/
    DataContainerType_t GetDataContainerType() {return PROGRAMS;}

    /****************************************************************************/
    /*!
     *  \brief Get's the Varification mode
     *
     *  \return Reagents Count
     */
    /****************************************************************************/

    bool GetDataVerificationMode() {return m_DataVerificationMode;}

    /****************************************************************************/
    /*!
     *  \brief Sets the Verification Mode of the ReagentGroupColor list File
     *
     *  \iparam Value = bool type , either true or false
     *
     *  \return
     */
    /****************************************************************************/
    void SetDataVerificationMode(const bool Value){m_DataVerificationMode = Value;}


    /****************************************************************************/
    /*!
     *  \brief Retrieve the number of Color in the ReagentGroupColor list
     *  \return Program count
     */
    /****************************************************************************/
    int  GetNumberOfReagentGroupColor() {return m_ReagentGroupColorList.count();}

    /****************************************************************************/
    /*!
     *  \brief Get's the version of The ReagentGroupColor list File
     *
     *  \return Version Number
     */
    /****************************************************************************/
    int GetVersion() {return m_Version;}

    /****************************************************************************/
    /*!
     *  \brief Get's the fileName of the ReagentGroupColor list
     *
     *  \return File name String
     */
    /****************************************************************************/
    QString GetFilename() {return m_Filename;}

    /****************************************************************************/
    /*!
     *  \brief set's the fileName of the ReagentGroupColor list
     *
     *  \param Value = const QString type parameter
     *
     *  \return File name String
     */
    /****************************************************************************/
  void SetFilename (const QString Value) {m_Filename = Value;}

  const CReagentGroupColor* GetCReagentGroupColor(const unsigned int Index);// uses order index

};

} // namespace DataManager

#endif // DATAMANAGER_DATAREAGENTLIST_H

