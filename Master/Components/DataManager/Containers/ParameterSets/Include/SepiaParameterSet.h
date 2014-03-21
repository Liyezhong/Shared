/****************************************************************************/
/*! \file Program.h
 *
 *  \brief Definition file for class CSepiaParameterSet.
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

#ifndef SEPIADATAMANAGER_SEPIAPARAMETERSET_H
#define SEPIADATAMANAGER_SEPIAPARAMETERSET_H

#include <QString>
#include <QStringList>
#include <QHash>
#include <QDebug>
#include <DataManager/Helper/Include/Types.h>


namespace DataManager {


/****************************************************************************/
/*!
 *  \brief  This class implements parameter sets for coverslipper
 */
/****************************************************************************/
class CSepiaParameterSet
{
    friend class CSepiaParameterSetList;
private:
    QString m_MountantName;                            //!< program color
    quint16 m_CoverglassLength;
    quint16 m_MountantVolume;
    QString m_ID;

//    bool m_Locked;                              //!< True - Leica program, else user defined program
//    ListOfExpandedSteps_t m_ExpandedStepList;   //!< Normal steps + Macro steps
//    bool m_ExpandedListUpToDate;                //!< Flag: true- list upto date
//    ProgramSIDHash_t    m_ProgramSIDHash;       //!< This hash holds the previously created SID step.
//    QStringList m_ReagentIDList;                //!< Holds the reagent ID's present in Program.
//    QStringList m_StationList;                  //!< List of Stations used by the program
//    CSepiaParameterSetStep* mp_DryStationStep;            //!< Dry Station step created when two steps are in different zones of BathLayout
//    CSepiaParameterSetStep* mp_SIDStationStep;            //!< SID step . Both Dry step and SID step are present only in Expanded Steplist
//    bool m_Modified;
//    ListOfIDs_t m_OrderedListOfExpandedStepIDs; //!< Ordered list of step id's
    bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);
    bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);
//    bool RefreshExpandedStepList();
//    void Init();
//    void UpdateExpandedSteps(const CSepiaParameterSet &Other);
public:
    CSepiaParameterSet();
    CSepiaParameterSet(const QString ID);
//    CSepiaParameterSet(const QString ID);
    CSepiaParameterSet(const QString ID, const QString MountantName, const quint16 CoverglassLength, const quint16 MountantVolume);
    CSepiaParameterSet(const CSepiaParameterSet&);
    ~CSepiaParameterSet();

    friend QDataStream& operator << (QDataStream& OutDataStream, const CSepiaParameterSet& Program);
    friend QDataStream& operator >> (QDataStream& InDataStream, CSepiaParameterSet& Program);
    CSepiaParameterSet& operator = (const CSepiaParameterSet&);

    /****************************************************************************/
    /*!
     *  \brief Retrieves the MountantName associated with the program
     *
     *  \return MountantName
     */
    /****************************************************************************/
    QString GetMountantName()const {return m_MountantName;}


    /****************************************************************************/
    /*!
     *  \brief Sets the color associated with the program
     *
     *  \iparam Color
     */
    /****************************************************************************/
    void SetMountantName(const QString MountantName) {m_MountantName = MountantName;}

    /****************************************************************************/
    /*!
     *  \brief Retrieves the ID associated with the program
     *
     *  \return ID
     */
    /****************************************************************************/
    QString GetID()const {return m_ID;}


    /****************************************************************************/
    /*!
     *  \brief Sets the color associated with the program
     *
     *  \iparam Color
     */
    /****************************************************************************/
    void SetID(QString ID){m_ID = ID;}

    /****************************************************************************/
    /*!
     *  \brief Retrieves the CoverglassLength associated with the program
     *
     *  \return CoverglassLength
     */
    /****************************************************************************/
    quint16 GetCoverglassLength()const {return m_CoverglassLength;}


    /****************************************************************************/
    /*!
     *  \brief Sets the color associated with the program
     *
     *  \iparam Color
     */
    /****************************************************************************/
    void SetCoverglassLength(const quint16 CoverglassLength){m_CoverglassLength = CoverglassLength;}

    /****************************************************************************/
    /*!
     *  \brief Retrieves the MountantVolume associated with the program
     *
     *  \return MountantVolume
     */
    /****************************************************************************/
    quint16 GetMountantVolume()const {return m_MountantVolume;}


    /****************************************************************************/
    /*!
     *  \brief Sets the color associated with the program
     *
     *  \iparam Color
     */
    /****************************************************************************/
    void SetMountantVolume(const quint16 MountantVolume){m_MountantVolume = MountantVolume;}

};


} // namespace DataManager

#endif // SEPIADATAMANAGER_SEPIAPARAMETERSET_H
