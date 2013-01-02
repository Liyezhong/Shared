/****************************************************************************/
/*! \file Adjustment.h
 *
 *  \brief Definition file for class CAdjustment
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-12-21
 *  $Author:    $ Stalin
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

#include <QStringList>
#include <QXmlStreamWriter>
#include "DataManager/Helper/Include/Types.h"
#include "DataManager/Helper/Include/Helper.h"
#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"

#ifndef DATAMANAGER_ADJUSTMENT_H
#define DATAMANAGER_ADJUSTMENT_H

namespace DataManager
{

class PositionXYZ
{
public:
    void Clear()
    {
        PositionX = 0;
        PositionY = 0;
        PositionZ = 0;
    }

    quint32 PositionX;
    quint32 PositionY;
    quint32 PositionZ;

    bool Active;
};

typedef enum {
    LEFT_XYZ = 0, RIGHT_XYZ
} Xyz_t;

typedef enum {
    XYZ_ROW1 = 0, XYZ_ROW2, XYZ_ROW3, XYZ_ROW4, XYZ_MAX_ROWS
} XyzRow_t;

typedef enum {
    XYZ_COL1 = 0, XYZ_COL2, XYZ_COL3, XYZ_COL4, XYZ_COL5, XYZ_COL6, XYZ_COL7, XYZ_COL8, XYZ_COL9,
    XYZ_COL10, XYZ_COL11, XYZ_COL12, XYZ_COL13, XYZ_COL14, XYZ_COL15, XYZ_COL16, XYZ_COL17, XYZ_COL18,
    XYZ_MAX_COLS
} XyzCol_t;

class CAdjustment : public CDataContainerBase
{
public:
    CAdjustment();
    CAdjustment(const CAdjustment&);
    ~CAdjustment();

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CAdjustment& Adjustment);
    friend QDataStream& operator >>(QDataStream& InDataStream, CAdjustment& Adjustment);
    CAdjustment& operator=(const CAdjustment&);

    bool Read(QString Filename);

    /****************************************************************************/
    /*!
     *  \brief returns the  Data container type
     *  \return Data container type
     */
    /****************************************************************************/
    DataContainerType_t GetDataContainerType() {return ADJUSTMENT;}

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

    /****************************************************************************/
    /*!
     *  \brief Retrieve the version of the xml file
     *  \return version
     */
    /****************************************************************************/
    int  GetVersion() {return m_Version;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve filename of the XML
     *  \return file name
     */
    /****************************************************************************/
    QString GetFilename () {return m_Filename;}

    bool GetXyzPosition(Xyz_t Xyz, quint8 Row, quint8 Column, PositionXYZ &Position);

    bool SetXyzPosition(Xyz_t Xyz, quint8 Row, quint8 Column, PositionXYZ Position);

private:
    bool SerializeContent(QIODevice& p_Device, bool CompleteData);
    bool DeserializeContent(QIODevice& p_Device, bool CompleteData);

    bool SerializeXyzContent(QXmlStreamWriter &XmlStreamWriter, Xyz_t Xyz);
    bool DeserializeXyzContent(QXmlStreamReader &XmlStreamReader, Xyz_t Xyz);

    void ClearPositions();

    /****************************************************************************/
    /*!
     *  \brief Sets the XML version
     *  \iparam Value = Version to set
     */
    /****************************************************************************/
    void SetVersion(const int Value)        {m_Version = Value;}

    /****************************************************************************/
    /*!
     *  \brief Sets the XML file name
     *  \iparam Value = Filename to set
     */
    /****************************************************************************/
    void SetFilename(const QString Value)   {m_Filename = Value;}

    void Init();

    //
    int m_Version;                  //!< version of the file read
    bool m_DataVerificationMode;    //!< Verification mode flag , verify the Container
    QString m_Filename;             //!< filename of the XML file to read

    //
    PositionXYZ m_StaionPosLeft[XYZ_MAX_COLS][XYZ_MAX_ROWS];
    PositionXYZ m_StaionPosRight[XYZ_MAX_COLS][XYZ_MAX_ROWS];
};

}

#endif // DATAMANAGER_ADJUSTMENT_H
