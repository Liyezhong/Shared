/****************************************************************************/
/*! \file ReagentGroupColor.h
 *
 *  \brief ReagentGroup defination.
 *
 *   $Date:    $ 2013-03-04
 *   $Author:  $ Swati Tiwari
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
#ifndef DATAMANAGER_REAGENTGROUPCOLOR_H
#define DATAMANAGER_REAGENTGROUPCOLOR_H


#include "DataManager/Helper/Include/Types.h"
#include "DataManager/Helper/Include/Helper.h"

namespace DataManager {
/****************************************************************************/
/*!
 *  \brief  This class implements Reagent Groups
 */
/****************************************************************************/

class CReagentGroupColor{
    friend class CReagentGroupColorList; //!< Made friend so that serialize and deserialize methods are accessible

private:
    bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);
    bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);

    ListOfErrors_t    m_ListOfErrors;        ///<  Definition/Declaration of variable m_ListOfErrors
    ErrorMap_t m_ErrorHash;            //!< Event List for GUI and for logging purpose. This member is not copied when using copy constructor/Assignment operator

protected:
    QString m_Color;                    //!< Reagent Color
    QString m_ColorID;                  //!< Reagent Color ID

public:
    CReagentGroupColor();
    ~CReagentGroupColor();
    CReagentGroupColor(const QString ColorID);


    CReagentGroupColor(const CReagentGroupColor&ReagentGroupColor);
    friend QDataStream& operator << (QDataStream& OutDataStream, const CReagentGroupColor& ReagentGroupColor);
    friend QDataStream& operator >> (QDataStream& InDataStream, CReagentGroupColor& ReagentGroupColor);
    CReagentGroupColor& operator = (const CReagentGroupColor&);

    /******************** INLINE FuNCTIONS **************************************/
    /****************************************************************************/
    /*!
     *  \brief Gets the Color ID of the particular ReagentGroupColor
     *
     *  \return
     */
    /****************************************************************************/

    QString GetColorID() const{return m_ColorID;}

    /****************************************************************************/
    /*!
     *  \brief Sets the Color ID of the particular ReagentGroupColor
     *
     *  \iparam Value = Desired Reagent Color ID
     *
     *  \return
     */
    /****************************************************************************/
    void SetColorID(const QString Value){m_ColorID = Value.trimmed();}
    /****************************************************************************/
    /*!
     *  \brief Gets the Color Value of the particular ReagentGroupColor
     *
     *  \return
     */
    /****************************************************************************/

    QString GetColorValue() const{return m_Color;}
    /****************************************************************************/
    /*!
     *  \brief Sets the Color Value of the particular ReagentGroupColor
     *
     *  \iparam Value = Desired Reagent Color Value
     *
     *  \return
     */
    /****************************************************************************/
    void SetColorValue(const QString Value){m_Color = Value.trimmed();}

    /****************************************************************************/
    /*!
     *  \brief  Appends the error hash in the error list
     *
     *  \iparam p_ErrorHash = pointer to error hash
     *
     */
    /****************************************************************************/
    /*! \warning DON'T implement(override) in derived classes! */
    void SetErrorList(ErrorMap_t *p_ErrorHash)
    {
        m_ListOfErrors.append(p_ErrorHash);
    }

    /****************************************************************************/
};
}
#endif // REAGENTGROUPCOLOR_H
