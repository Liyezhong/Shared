/****************************************************************************/
/*! \file Global/Include/LoggingSource.h
 *
 *  \brief Definition file for class LoggingSource.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.07.2010
 *   $Author:  $ Y.Novak
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

#ifndef GLOBAL_LOGGINGSOURCE_H
#define GLOBAL_LOGGINGSOURCE_H

#include <Global/Include/GlobalDefines.h>

namespace Global {

/****************************************************************************/
/**
 * \brief  This class implements a logging source. It is used by any object,
 *         which has to do the persistent logging.
 */
/****************************************************************************/
class LoggingSource {
friend class TestLoggingSource;
public:

    /****************************************************************************/
    /*!
     *  \brief   Constructor.
     *
     *  \param   source = logging source, represented by this instance
     *  \param   subComponent = the type of logging source subcomponent
     *
     ****************************************************************************/
    inline LoggingSource(gSourceType source, gSubComponentType subComponent) :
        m_Source(source),
        m_SubComponent(subComponent)
    {
    }
    /****************************************************************************/
    /*!
     *  \brief   Copy Constructor.
     *
     *  \param   rOther = the instrance to copy
     *
     ****************************************************************************/
    inline LoggingSource(const LoggingSource &rOther) :
        m_Source(0),
        m_SubComponent(0)
    {
        CopyFrom(rOther);
    }
    /****************************************************************************/
    /*!
     *  \brief   Default constructor.
     *
     ****************************************************************************/
    inline LoggingSource() :
        m_Source(0),
        m_SubComponent(0)
    {
    }
    /****************************************************************************/
    /*!
     *  \brief   Default Destructor.
     *
     ****************************************************************************/
    inline ~LoggingSource() {
    }
    /****************************************************************************/
    /*!
     *  \brief   Assignment operator.
     *
     *  \param   rOther = the instrance to copy
     *
     ****************************************************************************/
    inline const LoggingSource & operator = (const LoggingSource &rOther)
    {
        if(this != &rOther) {
            CopyFrom(rOther);
        }
        return *this;
    }

private:

    /****************************************************************************/
    /*!
     *  \brief   Copy function.
     *
     *  \param   rOther = the instrance to copy
     *
     ****************************************************************************/
    inline void CopyFrom(const LoggingSource & rOther)
    {
        m_Source = rOther.m_Source;
        m_SubComponent = rOther.m_SubComponent;
    }

public:

    /****************************************************************************/
    /*!
     *  \brief   Equality operator
     *
     *  \param   rOther = the instrance to compare with
     *
     *  \return  True if values of two instances are equal, false otherwise
     *
     ****************************************************************************/
    inline bool operator == (const LoggingSource & rOther) const {
        return (m_Source == rOther.m_Source) && (m_SubComponent == rOther.m_SubComponent);
    }
    /****************************************************************************/
    /*!
     *  \brief   Getter for the source
     *
     *  \return  the logging source
     *
     ****************************************************************************/
    inline gSourceType GetSource() const {
        return m_Source;
    }
    /****************************************************************************/
    /*!
     *  \brief   Getter for the subcomponent
     *
     *  \return  the subcomponent
     *
     ****************************************************************************/
    inline gSubComponentType GetSubComponent() const {
        return m_SubComponent;
    }
    /****************************************************************************/
    /*!
     *  \brief   Setter for the source
     *
     *  \param   src = the logging source to set
     *
     ****************************************************************************/
    inline void SetSource(gSourceType src) {
        m_Source = src;
    }
    /****************************************************************************/
    /*!
     *  \brief   Setter for the subcomponent
     *
     *  \param   subcmp = the subcomponent to set
     *
     ****************************************************************************/
    inline void SetSubComponent(gSubComponentType subcmp) {
        m_SubComponent = subcmp;
    }

private:

    gSourceType        m_Source;           ///< DataLogging Source.
    gSubComponentType  m_SubComponent;     ///< Subcomponent of the DataLogging Source.
}; // end class LoggingSource

} // end namespace Global

#endif // GLOBAL_LOGGINGSOURCE_H
