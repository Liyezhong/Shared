/****************************************************************************/
/*! \file Program.h
 *
 *  \brief Definition file for class CProgram.
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

#ifndef DATAMANAGER_PROGRAM_H
#define DATAMANAGER_PROGRAM_H

#include <DataManager/Helper/Include/Types.h>
#include "HimalayaDataContainer/Containers/Programs/Include/ProgramBase.h"


namespace DataManager {

//!< Forward declration
class CDataProgramList;


/****************************************************************************/
/*!
 *  \brief  This class implements  programs
 */
/****************************************************************************/
class CProgram : public CProgramBase
{
    friend class CDataProgramList;
private:
    bool m_LeicaProgram;                        //!< leica program or not
    QString m_Icon;                             //!< program icon
    bool m_Favorite;                            //!< program favorite
    bool m_BottleCheck;                         //!< flag for bottle checking or not

    QStringList m_ReagentIDList;                //!< Holds the reagent ID's present in Program.

    bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);
    bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);
    void Init();
public:
    CProgram();
    CProgram(const QString ID);
    CProgram(const QString ID, const QString Name, const QString LongName, bool IsLeicaProgram, QString Icon, bool Favorite);
    CProgram(const CProgram&);
    ~CProgram();

    quint32 GetProgramDurationInSeconds() const; // get program duration in second

    friend QDataStream& operator << (QDataStream& OutDataStream, const CProgram& Program);
    friend QDataStream& operator >> (QDataStream& InDataStream, CProgram& Program);
    CProgram& operator = (const CProgram&);


    /****************************************************************************/
    /*!
     *  \brief Checks if program is a Favorite program
     *
     *  \return True - Favorite , else no
     */
    /****************************************************************************/
    bool GetBottleCheck() const {return m_BottleCheck;}

    /****************************************************************************/
    /*!
     *  \brief set bottle check flag
     *
     *  \iparam value = true enable,
     */
    /****************************************************************************/
    void SetBottleCheck(bool value) {m_BottleCheck = value;}

    /****************************************************************************/
    /*!
     *  \brief Checks if program need to do bottle check
     *
     *  \return True - yes , else no
     */
    /****************************************************************************/
    bool IsFavorite() const {return m_Favorite;}
    /****************************************************************************/
    /*!
     *  \brief Checks if program is a Cleaning program
     *
     *  \return True - yes , else no
     */
    /****************************************************************************/
    bool IsCleaningProgram() const { return m_ID.at(0) == QChar('C');}

    /****************************************************************************/
    /*!
     *  \brief  enable or disable a program to be Favorite
     *  \iparam value = true enable,
     */
    /****************************************************************************/
    void SetFavorite(bool value) {m_Favorite = value;}   // unlock not possible


    /****************************************************************************/
    /*!
     *  \brief Sets the status of Leica Program
     *
     *  \iparam value = true - Leica Program , false - not Lecia Program
     */
    /****************************************************************************/
    void SetLeicaProgram(const bool value){m_LeicaProgram = value;}
    /****************************************************************************/
    /*!
     *  \brief Checks if Leica Program
     *
     *  \return true - Leica Program , false - not Lecia Program
     */
    /****************************************************************************/
    bool IsLeicaProgram() const {return m_LeicaProgram;}


    /****************************************************************************/
    /*!
     *  \brief Sets Program icon
     *
     *  \iparam value icon ID
     */
    /****************************************************************************/
    void SetIcon(const QString value){m_Icon = value;}
    /****************************************************************************/
    /*!
     *  \brief Get Program Icon
     *
     *  \return Program Icon ID
     */
    /****************************************************************************/
    QString GetIcon() const {return m_Icon;}


    /****************************************************************************/
    /*!
     *  \brief Retrieve Reagent ID list
     *
     *  \return Reagent ID list
     */
    /****************************************************************************/
    QStringList GetReagentIDList() const {return m_ReagentIDList;}
};


} // namespace DataManager

#endif // DATAMANAGER_PROGRAM_H
