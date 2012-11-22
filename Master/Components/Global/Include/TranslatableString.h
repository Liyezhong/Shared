/****************************************************************************/
/*! \file Global/Include/TranslatableString.h
 *
 *  \brief Definition file for class TranslatableString.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
 *  $Author:    $ J.Bugariu
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

#ifndef GLOBAL_TRANSLATABLESTRING_H
#define GLOBAL_TRANSLATABLESTRING_H

#include <Global/Include/GlobalEventCodes.h>

#include <QList>
#include <QString>

namespace Global {

class TranslatableString;

typedef QList<TranslatableString> tTranslatableStringList; ///< typedef for list of translatable strings

/****************************************************************************/
/**
 * \brief This line encapsulates a translatable string.
 *
 * It contains an string ID and an list of arguments. These arguments are
 * translatable strings themselves. It may also be of type plain string
 * in which case this string is saved in \ref m_String.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class TranslatableString {
friend class TestTranslatableString;
private:
    bool                        m_oString;          ///< Flag indicating that it is a plain string.
    quint32                     m_StringID;         ///< String ID.
    QString                     m_String;           ///< Plain string.
    tTranslatableStringList     m_ArgumentList;     ///< Argument list. Also translatable strings.
    /****************************************************************************/
    /**
     * \brief Copy from other instance.
     *
     * \param[in]   rOther  Instance to copy from.
     */
    /****************************************************************************/
    inline void CopyFrom(const TranslatableString &rOther) {
        m_oString = rOther.m_oString;
        m_StringID = rOther.m_StringID;
        m_String = rOther.m_String;
        m_ArgumentList = rOther.m_ArgumentList;
    }
protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    inline TranslatableString() :
        m_oString(false),
        m_StringID(EVENT_GLOBAL_UNKNOWN_STRING_ID)
    {
    }
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   StringID        StringID
     */
    /****************************************************************************/
    inline TranslatableString(quint32 StringID) :
        m_oString(false),
        m_StringID(StringID)
    {
    }
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   String          The plain string.
     */
    /****************************************************************************/
    inline TranslatableString(const QString &String) :
        m_oString(true),
        m_StringID(EVENT_GLOBAL_UNKNOWN_STRING_ID),
        m_String(String)
    {
    }
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   pcString        The plain string.
     */
    /****************************************************************************/
    inline TranslatableString(const char *pcString) :
        m_oString(true),
        m_StringID(EVENT_GLOBAL_UNKNOWN_STRING_ID),
        m_String(pcString)
    {
    }
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   StringID        StringID
     * \param[in]   ArgumentList    Argument list.
     */
    /****************************************************************************/
    inline TranslatableString(quint32 StringID, const tTranslatableStringList &ArgumentList) :
        m_oString(false),
        m_StringID(StringID),
        m_ArgumentList(ArgumentList)
    {
    }
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \param[in]   rOther  Instance to copy from.
     */
    /****************************************************************************/
    inline TranslatableString(const TranslatableString &rOther) :
        m_oString(false),
        m_StringID(EVENT_GLOBAL_UNKNOWN_STRING_ID)
    {
        CopyFrom(rOther);
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    inline ~TranslatableString() {
    }
    /****************************************************************************/
    /**
     * \brief Assignment operator.
     *
     * \param[in]   rOther  Instance to copy from.
     */
    /****************************************************************************/
    inline const TranslatableString & operator = (const TranslatableString &rOther) {
        if(this != &rOther) {
            CopyFrom(rOther);
        }
        return *this;
    }
    /****************************************************************************/
    /**
     * \brief Check if it is of type plain string.
     *
     * \return  true if of type plain string.
     */
    /****************************************************************************/
    inline bool IsString() const {
        return m_oString;
    }
    /****************************************************************************/
    /**
     * \brief Get string ID.
     *
     * \return  String ID.
     */
    /****************************************************************************/
    inline quint32 GetStringID() const {
        return m_StringID;
    }
    /****************************************************************************/
    /**
     * \brief Get plain string.
     *
     * \return  Plain string.
     */
    /****************************************************************************/
    inline QString GetString() const {
        return m_String;
    }
    /****************************************************************************/
    /**
     * \brief Get argument list.
     *
     * \return  Argument list.
     */
    /****************************************************************************/
    inline const tTranslatableStringList & GetArgumentList() const {
        return m_ArgumentList;
    }
}; // end class TranslatableString

} // end namespace DataLogging

# endif // GLOBAL_TRANSLATABLESTRING_H
