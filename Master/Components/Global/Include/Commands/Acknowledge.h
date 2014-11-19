/****************************************************************************/
/*! \file Global/Include/Commands/Acknowledge.h
 *
 *  \brief Definition file for class Acknowledge.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-03-14
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

#ifndef GLOBAL_ACKNOWLEDGE_H
#define GLOBAL_ACKNOWLEDGE_H

#include <Global/Include/SharedPointer.h>
#include <Global/Include/GlobalDefines.h>

#include <QString>
#include <QDataStream>

namespace Global {

/****************************************************************************/
/**
 * \brief Base class for acknowledges.
 *
 * All command acknowledgess exchanged between thread controllers must be
 * subclasses of this class.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class Acknowledge {
    friend class TestCommands;
private:
    /****************************************************************************/
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(Acknowledge)
protected:
    /****************************************************************************/
    /**
     * \brief Copy to some stream.
     *
     * \param[in,out]   Stream      Stream to stream into.
     */
    /****************************************************************************/
    inline void CopyToStream(QDataStream &Stream) const {
        // nothing to do
        Q_UNUSED(Stream);
    }
    /****************************************************************************/
    /**
     * \brief Get from some stream.
     *
     * \param[in,out]   Stream      Stream to stream from.
     */
    /****************************************************************************/
    inline void CopyFromStream(QDataStream &Stream) {
        // nothing to do
        Q_UNUSED(Stream);
    }
public:
    static const QString    SERIALIZERSTRING;   ///< String used to detect an acknowledge when serializing.
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    Acknowledge();
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~Acknowledge();
    /****************************************************************************/
    /**
     * \brief Get command acknowledge name.
     *
     * \return  Command acknowledge name.
     */
    /****************************************************************************/
    virtual QString GetName() const = 0;

    Global::tRefType m_Ref;     ///< Reference of command for which acknowledgment is sent
}; // end class Acknowledge

typedef Global::SharedPointer<Acknowledge>  AcknowledgeShPtr_t;     ///< Typedef for shared pointer of acknowledge.

} // end namespace Global

#endif // GLOBAL_ACKNOWLEDGE_H
