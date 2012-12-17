/****************************************************************************/
/*! \file DataProgramList.h
 *
 *  \brief Definition file for class CSepiaParameterSetList.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-12-06
 *  $Author:    $ Michael Thiel
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

#ifndef SEPIADATAMANAGER_SEPIAPARAMETERSETLIST_H
#define SEPIADATAMANAGER_SEPIAPARAMETERSETLIST_H

#include <QHash>
#include <QList>
#include <QReadWriteLock>
#include <QBuffer>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "Global/Include/Translator.h"
#include "DataManager/Helper/Include/Types.h"
#include "DataManager/Containers/ParameterSets/Include/SepiaParameterSet.h"


namespace DataManager {


/****************************************************************************/
/*!
 *  \brief  This class implements Program List
 */
/****************************************************************************/
class CSepiaParameterSetList
{

public:
    CSepiaParameterSetList();
    CSepiaParameterSetList(const CSepiaParameterSetList&);
    ~CSepiaParameterSetList();

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CSepiaParameterSetList&  ProgramList);
    friend QDataStream& operator >>(QDataStream& InDataStream, CSepiaParameterSetList& ProgramList);
    CSepiaParameterSetList& operator=(const CSepiaParameterSetList&);

    void Init();

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
    int  GetNumberOfPrograms() {return m_ProgramList.count();}

    /****************************************************************************/
    /*!
     *  \brief Retrieve a program with the given program ID
     *  \iparam ProgramID
     *  \return Pointer to Program
     */
    /****************************************************************************/
    CSepiaParameterSet* GetParameterSet(QString ParameterId);

    bool AddParameterSet(CSepiaParameterSet* p_Program);   // p_Program is added directly => don't delete outside!
    bool DeleteProgram(QString ParameterID);   // uses unique program ID
    bool DeleteAllPrograms();
    //END OF SECTION PROGRAMS//


    bool ParameterSetExists(QString ParameterId);

private:
    int m_Version;  //!<  version of the file read
//    bool m_DataVerificationMode;//!< Verification mode flag , verify the Container
//    QString m_NextProgramID;//!< next Free Program ID to use for a new user program
    QString m_Filename;//!< filename of the XML file to read

    QHash<QString, CSepiaParameterSet*> m_ProgramList; //!< Program list
    QString m_ParameterId;

//    QReadWriteLock* mp_ReadWriteLock; //!< Lock for thread safety

    bool SerializeContent(QIODevice& IODevice, bool CompleteData);
    bool DeserializeContent(QIODevice& IODevice, bool CompleteData);

    bool ReadAllPrograms(QXmlStreamReader& XmlStreamReader, bool CompleteData);

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

//    bool UpdateReagentIDList();
};

} // namespace DataManager

#endif // SEPIADATAMANAGER_SEPIAPARAMETERSETLIST_H
