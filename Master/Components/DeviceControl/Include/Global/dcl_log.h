/****************************************************************************/
/*! \file dcl_log.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 01.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains a logging implementation
 *
 *
 */
/****************************************************************************/

#ifndef LOG_H_
#define LOG_H_

//#pragma warning(disable:4530)

#include <cstdio>
#include <sstream>
#include <string>



#define LOG() qDebug()

/*-------------------------------------------------
 This file provides functionality for logging
 Start Logging with the following commands:

   FILE* pFile = fopen("/mydir/myfile.log", "w");
   Output2FILE::Stream() = pFile;

 A log entry will be created with

 FILE_LOG_L(laINIT, llDEBUG) << "MyLogEntry" << nMyInteger << strMyString;

 There are two parameters.
 The first defines the log area, for example 'Initialisation', Communication,...
 The second defines the level, for example debug, information, error
 A log command with a level lower than the predefined will not be written
 Only log commands which match a bitwise OR comparision to g_sLogArea will be written
 See TLogArea and TLogLevel for defines, you can easily expand them.
  Have fun, Norbert Wiedmann

 -----------------------------------------------------------*/


inline std::string NowTime();
inline std::string NowDateTime();

/**
 * Log level definition
 * Each log area can be assigned to a log level which specifies the level of logging details
 * An entry will be logged if the corresponding log instruction has a Level similar or lover then then
 * log level set for the area
 */
typedef enum {  llERROR   = 0x00,  //!< llERROR     error during program execution
                llWARNING = 0x01,  //!< llWARNING     warnings, conditions which should not appear,
                llINFO    = 0x02,  //!< llINFO     info, typically steps during the execution (e.g. start, read config, start diag,..)
                llDEBUG   = 0x03,  //!< llDEBUG     debug info, enter and exit of a class, rude steps during operation
                llDEBUG1  = 0x04,  //!< llDEBUG1
                llDEBUG2  = 0x05,  //!< llDEBUG2
                llDEBUG3  = 0x06,  //!< llDEBUG3
                llDEBUG4  = 0x07   //!< llDEBUG4    debug info, very detailed steps during special operation
              } TLogLevel_t;

/*enum TLogLevel {llERROR   = 0x01,   //!< llERROR     error during program execution
                llWARNING = 0x02, //!< llWARNING   warnings, conditions which should not appear,
                llINFO    = 0x03,    //!< llINFO      info, typically steps during the execution (e.g. start, read config, start diag,..)
                llDEBUG   = 0x04,   //!< llDEBUG     debug info, rude steps during operation
                llDEBUG1  = 0x05,  //!< llDEBUG1
                llDEBUG2  = 0x06,  //!< llDEBUG2
                llDEBUG3  = 0x07,  //!< llDEBUG3
                llDEBUG4  = 0x08, };//!< llDEBUG4    debug info, very detailed steps during special operation*/

/**
 * Log area definition
 * Each log entry must be assigned to a log area which is predefined here
 * The entry will be logged if its log level is equal or lover than the one assigned to.
 */
typedef enum { laUNDEF       = 0x0000, //!< laUNDEF
               laCAN         = 0x0001, //!< laCAN        can bus message logging
               laINIT        = 0x0002, //!< laINIT       all the stuff done without interaction to external system components (e.g. reading configs, open can interface
               laCALIB       = 0x0003, //!< laCALIB
               laCONFIG      = 0x0004, //!< laCONFIG     all the stuff to initialize and configure the external system components (slaves, gui, tcp)
               laCONFIG_FILE = 0x0005, //!< laCONFIG_FILE
               laCOMM        = 0x0006, //!< laCOMM       //laRESERVED    = 0x0007};//!< laRESERVED
               laDEV         = 0x0007, //!< laDEV        all the things told by CDevices and IDevices
               laFCT         = 0x0008, //!< laFCT        all the things told by Function modules and CANNodes
               laDEVPROC     = 0x0009, //!< laDEVPROC    all the stuff from device processing, after start up. heartbeat, state machine, ..
               laSIM         = 0x000a, //!< llSIM        simulation via tcp
               laDIAG        = 0x000b  //!< laDIAG       diagnostic service
              } TLogArea_t;


/****************************************************************************/
/*!
 *  \brief  Log class template
 */
/****************************************************************************/
template <typename T>
class Log
{
public:
    Log();
    virtual ~Log();
    //! Compose the logging string
    std::ostringstream& Get(TLogArea_t area = laUNDEF, TLogLevel_t level = llINFO, std::string strClass = "", std::string strFunction = "");
public:
    //! Convert area assignments to loging level
    static TLogLevel_t ReportingLevel(TLogArea_t area);
    //! Convert logging level to string
    static std::string ToString(TLogLevel_t level);
    //! Convert logging area to string
    static std::string ToStringArea( TLogArea_t area);
    //! Convert string to logging level
    static TLogLevel_t FromString(const std::string& level);
public:
    //! Output stream
    std::ostringstream os;
private:
    //! Overloaded constructor
    Log(const Log&);
    //! Assignment operator definition
    Log& operator =(const Log&);
};

/****************************************************************************/
/*!
 *  \brief  Constructor of logging class template
 */
/****************************************************************************/
template <typename T>
Log<T>::Log()
{
}

/****************************************************************************/
/*!
 *  \brief  Output the logging string
 *
 *  \iparam area = Logging area
 *  \iparam level = Logging level
 *  \iparam strClass = Name of the calling class
 *  \iparam strFunction = Name of the calling function
 *
 *  \return Output stream
 */
/****************************************************************************/
template <typename T>
std::ostringstream& Log<T>::Get(TLogArea_t area, TLogLevel_t level, std::string strClass, std::string strFunction)
{
    Q_UNUSED(strClass);

    os << "- " << NowTime();
    os << " " << ToStringArea(area) << ":";
    os << " " << ToString(level) << ":";
    //os << std::string(level > llDEBUG ? level - llDEBUG : 0, '\t') << strFunction << std::string("\t") << strClass << std::string("\t");
    os << std::string(((quint16)level) > ((quint16)llDEBUG) ? ((quint16)level) - ((quint16)llDEBUG) : 0, '\t') << /*strClass << "::" <<*/ strFunction;

    return os;
}

/****************************************************************************/
/*!
 *  \brief  Destructor of logging class template
 */
/****************************************************************************/
template <typename T>
Log<T>::~Log()
{
    try
    {
        os << std::endl;
        T::Output(os.str());
    }
    catch (...)
    {
        return;
    }
}

/****************************************************************************/
/*!
 *  \brief  Logging level to area assignments
 *
 *  \iparam area = Logging area
 *
 *  \return Logging level
 */
/****************************************************************************/
template <typename T>
TLogLevel_t Log<T>::ReportingLevel(TLogArea_t area)
{
    switch (area)
    {
        case laUNDEF:
            return llDEBUG;
        case laCAN:         // logarea: laCAN
            return llDEBUG;
        case laINIT:        // logarea: laINIT
            return llDEBUG;
        case laCALIB:       // logarea: laCALIB
            return llDEBUG;
        case laCONFIG:      // logarea: laCONFIG
            return llDEBUG;
        case laCONFIG_FILE: // logarea: laCONFIG_FILE
            return llDEBUG;
        case laCOMM:        // logarea: laCOMM
            return llDEBUG;
        case laDEV:         // logarea: laDEV
            return llDEBUG;
        case laFCT:         // logarea: laFCT
            return llDEBUG1;
        case laDEVPROC:     // logarea: laDEVPROC
            return llDEBUG;
        case laSIM:         // logarea: laSIM
            return llDEBUG;
        case laDIAG:        // logarea: laDIAG
            return llDEBUG;
    }
    return llINFO;
}

/****************************************************************************/
/*!
 *  \brief  Logging level output strings
 *
 *  \iparam level = Logging level
 *
 *  \return Logging level name
 */
/****************************************************************************/
template <typename T>
std::string Log<T>::ToString(TLogLevel_t level)
{
    static const char* const buffer[] = {"ERR ", "WARN", "INFO", "DBG ", "DBG1", "DBG2", "DBG3", "DBG4"};
    return buffer[level];
}

/****************************************************************************/
/*!
 *  \brief  Logging area definitions
 *
 *  \iparam area = Logging area
 *
 *  \return Logging area name
 */
/****************************************************************************/
template <typename T>
std::string Log<T>::ToStringArea(TLogArea_t area)
{
    static const char* const buffer[] = {"undef. ", "CAN    ", "INIT   ", "CALIB  ", "CONFIG  ", "CONF_F ", "COMM   ", "DEV    ", "FCT    ",
                                         "DEVPROC", "SIM    ", "free", "free", "free", "free", "free", "free", };

    return buffer[area];
}

/****************************************************************************/
/*!
 *  \brief  Logging level definitions
 *
 *  \iparam level = Logging level
 *
 *  \return Logging level
 */
/****************************************************************************/
template <typename T>
TLogLevel_t Log<T>::FromString(const std::string& level)
{
    if (level == "DEBUG4")
        return llDEBUG4;
    if (level == "DEBUG3")
        return llDEBUG3;
    if (level == "DEBUG2")
        return llDEBUG2;
    if (level == "DEBUG1")
        return llDEBUG1;
    if (level == "DEBUG")
        return llDEBUG;
    if (level == "INFO")
        return llINFO;
    if (level == "WARNING")
        return llWARNING;
    if (level == "ERROR")
        return llERROR;
    Log<T>().Get(llWARNING, __FUNCTION__, "class") << "Unknown logging level '" << level << "'. Using INFO level as default.";
    return llINFO;
}

/****************************************************************************/
/*! \brief This class implements the logging functionality
*/
/****************************************************************************/
class Output2FILE
{
public:
    static FILE*& Stream();
    static void Output(const std::string& msg);
};

/****************************************************************************/
/*!
 *  \brief   Returns the output stream
 *
 *  \return  Output stream
 *
 ****************************************************************************/
inline FILE*& Output2FILE::Stream()
{
    static FILE* pStream = stderr;
    return pStream;
}

/****************************************************************************/
/*!
 *  \brief   Outputs the string to the logging stream
 *
 *  \param    msg = Logging message string
 *
 ****************************************************************************/
inline void Output2FILE::Output(const std::string& msg)
{
    return;
#ifdef PTS
    FILE* pStream = Stream();
    if (!pStream)
        return;
    fprintf(pStream, "%s", msg.c_str());     
/*lint -save -e534 */
//ignoring return value of fflush
    fflush(pStream);
/*lint -restore */
#else
   QString s =  QString(msg.c_str());
   //LOG() << s; //disbale the log now, or will get too much information on debug output
#endif
}


#define FILELOG_DECLSPEC  //!< obsolete?

//! File logging class instance
class FILELOG_DECLSPEC FILELog : public Log<Output2FILE> {};

//#define FILELOG_MAX_LEVEL llERROR

#ifndef FILELOG_MAX_LEVEL
#define FILELOG_MAX_LEVEL llDEBUG4  //!< maximum logging leve (max. depth)
#endif

/****************************************************************************/
/*!
 *  \brief  Logging function
 *
 *  \iparam area = Logging area
 *  \iparam level = Logging level
 */
/****************************************************************************/
#define FILE_LOG_L(area, level) \
    if (level > FILELog::ReportingLevel(area) || !Output2FILE::Stream()) ; \
    else FILELog().Get(area, level, "", __FUNCTION__)
/****************************************************************************/
/*!
 *  \brief  Logging function
 *
 *  \iparam area = Logging area
 *  \iparam level = Logging level
 *  \iparam classname = Calling class
 */
/****************************************************************************/
#define FILE_LOG_L1(area, level, classname) \
    if (level > FILELOG_MAX_LEVEL) ;\
    else if (level > FILELog::ReportingLevel(area) || !Output2FILE::Stream()) ; \
    else FILELog().Get(area, level, classname, __FUNCTION__)

#include <sys/time.h>

/****************************************************************************/
/*!
 *  \brief  Return current time as string
 *
 *  \return Current time formatted string
 */
/****************************************************************************/
inline std::string NowTime()
{
    struct timeval tv;
    char result[20] = {0};
/*lint -save -e534 */
//ignoring return value of time, strftime and gettimeofday
    gettimeofday(&tv, 0);
/*lint -restore */
    sprintf(result, "%03ld.%03ld", (long)tv.tv_sec, (long)tv.tv_usec / 1000);

    return result;
}

/****************************************************************************/
/*!
 *  \brief  Return current data and time as string
 *
 *  \return Current date and time formatted string
 */
/****************************************************************************/
inline std::string NowDateTime()
{
    char bufDate[10];
    char bufTime[11];
    time_t t;
    tm r;
    struct timeval tv;

/*lint -save -e534 */
//ignoring return value of time, strftime and gettimeofday
    time(&t);

    strftime(bufDate, sizeof(bufDate), "%x", localtime_r(&t, &r));
    strftime(bufTime, sizeof(bufTime), "%X", localtime_r(&t, &r));

    gettimeofday(&tv, 0);
/*lint -restore */
    char result[100] = {0};
    sprintf(result, "%s-%s.%03ld", bufDate, bufTime, (long)tv.tv_usec / 1000);
    return result;
}

#endif //__LOG_H__
