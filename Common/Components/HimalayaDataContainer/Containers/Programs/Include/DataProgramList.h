/****************************************************************************/
/*! \file DataProgramList.h
 *
 *  \brief Definition file for class CDataProgramList.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-08-01
 *  $Author:    $ F. Toth, N.Kamath
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

#ifndef DATAMANAGER_DATAPROGRAMLIST_H
#define DATAMANAGER_DATAPROGRAMLIST_H


#include "Global/Include/Translator.h"
#include "DataManager/Helper/Include/Types.h"
#include "HimalayaDataContainer/Containers/Programs/Include/Program.h"
#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "HimalayaDataContainer/Helper/Include/HimalayaDataManagerEventCodes.h"

//lint -sem(DataManager::CDataProgramList::AddProgram, custodial(1))
//lint -sem(DataManager::CDataProgramList::UpdateProgram, custodial(2))


namespace DataManager {

//!< Hash table for list of programs
typedef QHash<QString, CProgram*> ListOfPrograms_t;

/****************************************************************************/
/*!
 *  \brief  This class implements Program List
 */
/****************************************************************************/
class CDataProgramList : public CDataContainerBase
{

public:
    CDataProgramList();
    CDataProgramList(const CDataProgramList&);
    ~CDataProgramList();

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CDataProgramList&  ProgramList);
    friend QDataStream& operator >>(QDataStream& InDataStream, CDataProgramList& ProgramList);
    CDataProgramList& operator=(const CDataProgramList&);

    void Init();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function UpdateOnLanguageChanged
     *
     *  \return from UpdateOnLanguageChanged
     */
    /****************************************************************************/
    void UpdateOnLanguageChanged();

    /****************************************************************************/
    /*!
     *  \brief returns the  Data container type
     *  \return Data container type
     */
    /****************************************************************************/
    DataContainerType_t GetDataContainerType() {return PROGRAMS;}

    /****************************************************************************/
    /*!
     *  \brief retrieves the status of Verification
     *  \return true-verification on , false-verification off
     */
    /****************************************************************************/
    bool GetDataVerificationMode() {return m_DataVerificationMode;}
    /****************************************************************************/
    /*!
     *  \brief sets verification mode
     *  \iparam Value = true-verification on, false - verification off
     */
    /****************************************************************************/
    void SetDataVerificationMode(const bool Value){m_DataVerificationMode = Value;}

    bool Read(QString Filename);

    /****************************************************************************/
    /*!
     *  \brief returns the version of the xml file read
     *  \return file version
     */
    /****************************************************************************/
    int  GetVersion() {return m_Version;}

    /****************************************************************************/
    /*!
     *  \brief returns the Next available program Id
     *  \iparam AutoCount = true to increase program ID , false - doesn't increase
     *                       program ID.
     *  \return Next program ID
     */
    /****************************************************************************/
    QString GetNextFreeProgID(bool AutoCount)
    {
        QString NextProgID = m_NextProgramID;
        if (AutoCount) {
            unsigned int  NextProgramID = m_NextProgramID.mid(1, -1).toInt();
            NextProgID = m_NextProgramID;
            NextProgramID++;
            m_NextProgramID.clear();
            m_NextProgramID = QString::number(NextProgramID);
            //Ignore return value
            (void)m_NextProgramID.prepend("U");
        }
        return NextProgID;
    }
    /****************************************************************************/
    /*!
     *  \brief Function to set next available program ID
     *  \iparam Value = Program ID to set
     */
    /****************************************************************************/
    void SetNextFreeProgID(const QString Value){m_NextProgramID = Value;}

    /****************************************************************************/
    /*!
     *  \brief returns the filename of the xml file read
     *  \return file name
     */
    /****************************************************************************/
    QString GetFilename() {return m_Filename;}


    //SECTION PROGRAMS//
    /****************************************************************************/
    /*!
     *  \brief Returns number of programs in the list
     *  \return Number of programs
     */
    /****************************************************************************/
    int  GetNumberOfPrograms() const {return m_ProgramList.count();}

    // the CreateProgram functions won't add the created program to the internal program list! This is the job of AddProgramm.
    /****************************************************************************/
    /*!
     *  \brief Creates a new program with an unique ID
     *  \return Pointer to Program
     */
    /****************************************************************************/
    CProgram* CreateProgram() {return new CProgram(GetNextFreeProgID(true));}  // correct new ID will be generated

    /****************************************************************************/
    /*!
     *  \brief Creates a new program with program ID sent
     *  \iparam ProgramID = ID of the program
     *  \return Pointer to Program
     */
    /****************************************************************************/
    CProgram* CreateProgram(const QString ProgramID) {return new CProgram(ProgramID);} // given ID will be used

    /****************************************************************************/
    /*!
     *  \brief Retrieve a program with the given program ID
     *  \iparam ProgramID
     *  \return Pointer to Program
     */
    /****************************************************************************/
    CProgram const * GetProgram(const QString ProgramID) {return m_ProgramList.value(ProgramID, NULL);}  // uses unique program ID
    /****************************************************************************/
    /*!
     *  \brief Retrieve a program with the given index
     *  \iparam Index
     *  \return Pointer to Program
     */
    /****************************************************************************/
    /*!
     *  \brief  GetProgram
     *  \iparam Index Index
     *  \return return CProgram
     */
    CProgram* GetProgram(const unsigned int Index) const; // uses order index

    /*!
     *  \brief  GetProgram
     *  \iparam ProgramID  Program id
     *  \return function status
     */
    bool GetProgram(const QString ProgramID, CProgram& Program);

    /*!
     *  \brief  GetProgram
     *  \iparam Index Index
     *  \iparam Program program
     *  \return function status
     */
    bool GetProgram(const unsigned int Index, CProgram& Program);

    /*!
     *  \brief  GetProgram
     *  \iparam p_Program program
     *  \return function status
     */
    bool AddProgram(const CProgram* p_Program);   // p_Program is added directly => don't delete outside!    

    /*!
     *  \brief  UpdateProgram
     *  \iparam p_Program program
     *  \return function status
     */
    bool UpdateProgram(const CProgram* p_Program);  // content of p_Program will be copied  => delete outside!

    /*!
     *  \brief  DeleteProgram
     *  \iparam ProgramID program id
     *  \return function status
     */
    bool DeleteProgram(const QString ProgramID);   // uses unique program ID

    /*!
     *  \brief  DeleteProgram
     *  \iparam Index Index
     *  \return function status
     */
    bool DeleteProgram(const unsigned int Index);  // uses order index

    /*!
     *  \brief  DeleteAllPrograms
     *  \return function status
     */
    bool DeleteAllPrograms();

    /*!
     *  \brief  GetFavoriteProgramIDs
     *  \return function status
     */
    QStringList GetFavoriteProgramIDs(); // get five favorite Programs' ID


    //END OF SECTION PROGRAMS//

    /*!
     *  \brief  ProgramExists
     *  \iparam ProgramID program id
     *  \return function status
     */
    bool ProgramExists(const QString ProgramID);

    /****************************************************************************/
    /*!
     *  \brief  Retreive the reagent ID's used by all the program.
     *  \return List of Reagent ID's used by all the programs in the Program List.
     */
    /****************************************************************************/
    QStringList const & GetReagentIDList() const { return m_ReagentIDList; }

    /*!
     *  \brief  CheckForUniquePropeties
     *  \iparam p_Program p_Program
     *  \iparam excludeSeft excludeSeft
     *  \return List of Reagent ID's used by all the programs in the Program List.
     */
    bool CheckForUniquePropeties(const CProgram* p_Program, bool excludeSeft = false);

private:
    int m_Version;  //!<  version of the file read
    bool m_DataVerificationMode;//!< Verification mode flag , verify the Container
    QString m_NextProgramID;//!< next Free Program ID to use for a new user program
    QString m_Filename;//!< filename of the XML file to read

    ListOfPrograms_t m_ProgramList; //!< Program list

    ListOfIDs_t m_OrderedListOfProgramIDs; //!< Ordered list of program ID's

    QReadWriteLock* mp_ReadWriteLock; //!< Lock for thread safety

    QStringList m_ProgramListNames;         //!< List of Program names

    ErrorMap_t m_ErrorHash;    //!< Event List for GUI and for logging purpose. This member is not copied when using copy constructor/Assignment operator
    QStringList m_ReagentIDList;//!< Holds the reagent ID's present in Program. This member is not copied when using copy constructor/Assignment operator

    /*!
     *  \brief  SerializeContent
     *  \iparam IODevice IO Device
     *  \iparam CompleteData  CompleteData
     *  \return serialize status
     */
    bool SerializeContent(QIODevice& IODevice, bool CompleteData);

    /*!
     *  \brief  DeserializeContent
     *  \iparam IODevice IO Device
     *  \iparam CompleteData  CompleteData
     *  \return de-serialize status
     */
    bool DeserializeContent(QIODevice& IODevice, bool CompleteData);

    /*!
     *  \brief  ReadAllPrograms
     *  \iparam XmlStreamReader XmlStreamReader
     *  \iparam CompleteData  CompleteData
     *  \return function status
     */
    bool ReadAllPrograms(QXmlStreamReader& XmlStreamReader, bool CompleteData);

    /*!
     *  \brief  UpdateReagentIDList
     *  \return function status
     */
    bool UpdateReagentIDList();

    /****************************************************************************/
    /*!
     *  \brief sets the xml version
     *  \iparam Value = Version to set
     */
    /****************************************************************************/
    void SetVersion(const int Value){m_Version = Value;}

    /****************************************************************************/
    /*!
     *  \brief Sets the XML file name
     *  \iparam Value = Filename to set
     */
    /****************************************************************************/
    void SetFilename(const QString Value) {m_Filename = Value;}
};

} // namespace DataManager

#endif // DATAMANAGER_DATAPROGRAM_H
