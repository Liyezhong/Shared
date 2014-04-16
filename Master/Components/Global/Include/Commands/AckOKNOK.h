/****************************************************************************/
/*! \file Global/Include/Commands/AckOKNOK.h
 *
 *  \brief Definition file for class AckOKNOK.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-03-23
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

#ifndef GLOBAL_ACKOKNOK_H
#define GLOBAL_ACKOKNOK_H

#include <Global/Include/GlobalDefines.h>
#include <Global/Include/Commands/Acknowledge.h>
#include <Global/Include/Utils.h>

namespace Global {

/****************************************************************************/
/**
 * \brief Class for command acknowledges of type OK / NOK.
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class AckOKNOK : public Global::Acknowledge {
friend QDataStream & operator << (QDataStream &, const AckOKNOK &);
friend QDataStream & operator >> (QDataStream &, AckOKNOK &);
private:
    bool                    m_Status;   ///< Status
    Global::GUIMessageType  m_Type;     ///< Type.
    QString                 m_Text;     ///< text
    /****************************************************************************/
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(AckOKNOK)
protected:
public:
    static QString NAME;            ///< Name of Acknowledge
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    AckOKNOK();
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   Status      True if status = OK.
     * \param[in]   Text        Text of message.
     * \param[in]   Type        Type of message.
     */
    /****************************************************************************/
    AckOKNOK(bool Status, const QString &Text = "", Global::GUIMessageType Type = Global::GUIMSGTYPE_ERROR);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~AckOKNOK();
    /****************************************************************************/
    /**
     * \brief Get status.
     *
     * \return  Status.
     */
    /****************************************************************************/
    inline bool GetStatus() const {
        return m_Status;
    }
    /****************************************************************************/
    /**
     * \brief Get type.
     *
     * \return  Type.
     */
    /****************************************************************************/
    inline Global::GUIMessageType GetType() const {
        return m_Type;
    }
    /****************************************************************************/
    /**
     * \brief Get text.
     *
     * \return  Text.
     */
    /****************************************************************************/
    inline QString GetText() const {
        return m_Text;
    }
    /****************************************************************************/
    /**
     * \brief Get command acknowledge name.
     *
     * \return  Command acknowledge name.
     */
    /****************************************************************************/
    virtual QString GetName() const {
        return NAME;
    }
}; // end class AckOKNOK

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Ack         The acknowledge to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const AckOKNOK &Ack) {
    // copy base class data
    Ack.CopyToStream(Stream);
    // copy internal data
    // status, type and text
    Stream << Ack.m_Status << GUIMessageTypeToString(Ack.m_Type) << Ack.m_Text;
    return Stream;
}

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream from.
 * \param[in,out]   Ack         The acknowledge to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, AckOKNOK &Ack) {
    // copy base class data
    Ack.CopyFromStream(Stream);
    // copy internal data
    // status
    Stream >> Ack.m_Status;
    // gui message type
    QString Tmp;
    Stream >> Tmp;
    Ack.m_Type = StringToGUIMessageType(Tmp, true);
    // text
    Stream >> Ack.m_Text;
    return Stream;
}

} // end namespace Global

#endif // GLOBAL_ACKOKNOK_H
