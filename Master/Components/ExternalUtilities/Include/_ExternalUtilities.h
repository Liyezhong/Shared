// ================================================================================================
/*! @file Platform/Master/Components/ExternalUtilities/Include/_ExternalUtilities.h
 *  $Revision: 0.2 $
 *  $Date: 2010-10-01 19:59:08+02 $
 *  $Author: Martin Weitzel (www.tbfe.de)
 *
 * @brief
 * Decouple typedef-s used in external libraries.
 *
 * Main purpose:
 *  - ease transition if stuff from external libraries is moved (e.g. Boost->TR1 and TR1->C++0x)
 *  - make type names look more according to local style guide rules
 *
 * @par Hint
 * To further abbreviate, locally something like
 * @code
 *  namespace XU = ExternalUtilities;
 * @endcode
 * may be used, so that e.g. \c ExternalUtilities::TimeSpan and \c XU::TimeSpan are synonyms.
 *
 * @todo MW:
 *  Add a short discussion how the classes defined here relate to
 *  Leica::Simulation::DeviceSpec::PhysicalUnits::Duration.
 *
 * ================================================================================================
*/
    //lint -esym(14, PtsBase)
    // --------------------------------------------------------------------------------------------
    // FlexeLint considers multiple defintions of this symbol to be an error.
    //
    // The FlexeLint message is correct but it is unavoidable due to the FlexeLint setup used on
    // build server, which checks several independant main programs as if they were linked into
    // one single executable. The easiest way to avoid the message (other as changing the setup of
    // the build server) is this central suppression by symbol name.
    //
    // -- Martin Weitzel[MW] / 2011-08-31

#ifndef EXTERNALUTILITIES_H
#define EXTERNALUTILITIES_H

#include "boost/date_time/posix_time/posix_time.hpp"

namespace ExternalUtilities {

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    //                                                  imports all the names from boost namespaces
    //
    using namespace boost::gregorian;
    using namespace boost::posix_time;
    using namespace boost::date_time;

    using boost::date_time::neg_infin;
    using boost::date_time::pos_infin;

    /*!
     * A "point" in time.
    */
    typedef boost::posix_time::ptime Time;

    /*!
     * A "duration", i.e. the difference between to points in time.
    */
    typedef boost::posix_time::time_duration TimeSpan;

    /*!
     * A "period", i.e. a starting and an ending time point.
    */
    typedef boost::posix_time::time_period TimeRange;

    /*!
     * A "date" (discarding the time of day from Time).
    */
    typedef boost::gregorian::date GregorianDate;

    /*!
     * Helper class to define a duration from an (untyped) number of seconds.
    */
    typedef boost::posix_time::seconds TimeSpan_SECS;

    /*!
     * Helper class to define a duration from an (untyped) number of milliseconds.
    */
    typedef boost::posix_time::milliseconds TimeSpan_MSECS;

    /*!
     * Helper class to define a duration from an (untyped) number of microseconds.
    */
    typedef boost::posix_time::microseconds TimeSpan_USECS;

}

#endif
