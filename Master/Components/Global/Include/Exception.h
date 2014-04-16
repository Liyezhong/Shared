/****************************************************************************/
/*! \file Global/Include/Exception.h
 *
 *  \brief Definition file for class Exception.
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

#ifndef GLOBAL_EXCEPTION_H
#define GLOBAL_EXCEPTION_H

#include <Global/Include/TranslatableString.h>
#include <Global/Include/AdjustedTime.h>

namespace Global {

/****************************************************************************/
/**
 * \brief Exception class.
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class Exception {
friend class TestException;
private:
    QString                 m_File;             ///< File in which exception occured.
    QString                 m_Line;             ///< Line in which exception occured.
    QDateTime               m_TimeStamp;        ///< Timestamp at which exception occured.
    quint32                 m_ErrorCode;        ///< Error code.
    tTranslatableStringList m_AdditionalData;   ///< Additional data for exception.
    /****************************************************************************/
    Exception();                        ///< Not implemented.
    /****************************************************************************/
    /**
     * \brief Copy from other instance.
     *
     * \param[in]   r_Other     Instance to copy from.
     */
    /****************************************************************************/
    inline void CopyFrom(const Exception &r_Other) {
        m_File = r_Other.m_File;
        m_Line = r_Other.m_Line;
        m_TimeStamp = r_Other.m_TimeStamp;
        m_ErrorCode = r_Other.m_ErrorCode;
        m_AdditionalData = r_Other.m_AdditionalData;
    }
protected:
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   File        File in which exception occured.
     * \param[in]   Line        Line in which exception occured.
     * \param[in]   TimeStamp   Timestamp at which exception occured.
     * \param[in]   ErrorCode   Error code.
     */
    /****************************************************************************/
    inline Exception(const QString &File, int Line, const QDateTime &TimeStamp, quint32 ErrorCode) :
        m_File(File),
        m_Line(QString::number(Line)),
        m_TimeStamp(TimeStamp),
        m_ErrorCode(ErrorCode)
    {
    }
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   File            File in which exception occured.
     * \param[in]   Line            Line in which exception occured.
     * \param[in]   TimeStamp       Timestamp at which exception occured.
     * \param[in]   ErrorCode       Error code.
     * \param[in]   Argument        One additional datum for exception.
     */
    /****************************************************************************/
    inline Exception(const QString &File, int Line, const QDateTime &TimeStamp, quint32 ErrorCode,
                     const TranslatableString &Argument) :
        m_File(File),
        m_Line(QString::number(Line)),
        m_TimeStamp(TimeStamp),
        m_ErrorCode(ErrorCode)
    {
        m_AdditionalData << Argument;
    }
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   File            File in which exception occured.
     * \param[in]   Line            Line in which exception occured.
     * \param[in]   TimeStamp       Timestamp at which exception occured.
     * \param[in]   ErrorCode       Error code.
     * \param[in]   AdditionalData  Additional data for exception.
     */
    /****************************************************************************/
    inline Exception(const QString &File, int Line, const QDateTime &TimeStamp, quint32 ErrorCode,
                     const tTranslatableStringList &AdditionalData) :
        m_File(File),
        m_Line(QString::number(Line)),
        m_TimeStamp(TimeStamp),
        m_ErrorCode(ErrorCode),
        m_AdditionalData(AdditionalData)
    {
    }
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \param[in]   rOther      Instance to copy from.
     */
    /****************************************************************************/
    inline Exception(const Exception &rOther) :
        m_Line("0"),
        m_ErrorCode(0)
    {
        CopyFrom(rOther);
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    inline ~Exception() {
    }
    /****************************************************************************/
    /**
     * \brief Assignment operator.
     *
     * \param[in]   rOther      Instance to copy from.
     * \return                  Const reference to self.
     */
    /****************************************************************************/
    inline const Exception & operator = (const Exception &rOther) {
        if(&rOther != this) {
            CopyFrom(rOther);
        }
        return *this;
    }
    /****************************************************************************/
    /**
     * \brief Get file name of file in which exception occured.
     *
     * \return  File name.
     */
    /****************************************************************************/
    inline QString GetFile() const {
        return m_File;
    }
    /****************************************************************************/
    /**
     * \brief Get line in which exception occured.
     *
     * \return  Line number.
     */
    /****************************************************************************/
    inline QString GetLine() const {
        return m_Line;
    }

    /****************************************************************************/
    /**
     * \brief Get file and line in which exception occured.
     *
     * \return  string of file name and Line number.
     */
    /****************************************************************************/
    inline QString what() const {
        return "File: " + m_File +
                ", Line: " + m_Line;
//                "\nTime: " + m_TimeStamp.toString();
    }

//    inline std::string what() const {
//        return toString().toStdString();
//    }
    /****************************************************************************/
    /**
     * \brief Get timestamp at which exception occured.
     *
     * \return  Time stamp as string.
     */
    /****************************************************************************/
    inline QDateTime GetTimeStamp() const {
        return m_TimeStamp;
    }
    /****************************************************************************/
    /**
     * \brief Get error code.
     *
     * \return  Error code.
     */
    /****************************************************************************/
    inline quint32 GetErrorCode() const {
        return m_ErrorCode;
    }
    /****************************************************************************/
    /**
     * \brief Get additional data.
     *
     * \return  Const reference to additional data.
     */
    /****************************************************************************/
    inline const tTranslatableStringList & GetAdditionalData() const {
        return m_AdditionalData;
    }
}; // end class Exception

} // end namespace Global

/****************************************************************************/
/**
 * \brief Throw exception without argument list.
 *
 * \param[in]   ErrorCode   The error code.
 */
/****************************************************************************/
#define THROW(ErrorCode) throw Global::Exception(__FILE__, __LINE__, Global::AdjustedTime::Instance().GetCurrentDateTime(), ErrorCode)

/****************************************************************************/
/**
 * \brief Throw exception with one argument.
 *
 * \param[in]   ErrorCode   The error code.
 * \param[in]   Arg         The argument.
 */
/****************************************************************************/
#define THROWARG(ErrorCode, Arg) throw Global::Exception(__FILE__, __LINE__, Global::AdjustedTime::Instance().GetCurrentDateTime(), ErrorCode, Arg)

/****************************************************************************/
/**
 * \brief Throw exception with argument list.
 *
 * \param[in]   ErrorCode   The error code.
 * \param[in]   ArgList     The argument list.
 */
/****************************************************************************/
#define THROWARGS(ErrorCode, ArgList) throw Global::Exception(__FILE__, __LINE__, Global::AdjustedTime::Instance().GetCurrentDateTime(), ErrorCode, ArgList)


/****************************************************************************/
/**
 * \brief Catch all exceptions and log them.
 */
/****************************************************************************/
#define CATCHALL() \
    catch(const Global::Exception & E) {                                                                                                                \
        Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_EXCEPTION,                                                                      \
                                                   Global::FmtArgs() << QString::number(E.GetErrorCode()) << E.what()                                   \
                                                                     << __FILE__ << __LINE__);                                                          \
    }                                                                                                                                                   \
    catch(const std::bad_alloc &) {                                                                                                                     \
        Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_ERROR_MEMORY_ALLOCATION,                                                        \
                                                   Global::tTranslatableStringList() << __FILE__ << __LINE__);                                          \
    }                                                                                                                                                   \
    catch(const std::exception & E) {                                                                                                                   \
        Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_STD_EXCEPTION,                                                                  \
                                                   Global::FmtArgs() << E.what()                                                                        \
                                                                     << __FILE__ << __LINE__);                                                          \
    }                                                                                                                                                   \
    catch(...) {                                                                                                                                        \
        Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION,                                                        \
                                                   Global::tTranslatableStringList() << __FILE__ << __LINE__);                                          \
    }


/****************************************************************************/
/**
 * \brief Catch all exceptions, log and rethrow them.
 */
/****************************************************************************/
#define CATCHALL_RETHROW() \
    catch(const Global::Exception &E) {                                                                                                                 \
        Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_EXCEPTION,                                                                      \
                                                   Global::FmtArgs() << QString::number(E.GetErrorCode()) << E.what()                                   \
                                                                     << __FILE__ << __LINE__);                                                          \
        throw;                                                                                                                                          \
    }                                                                                                                                                   \
    catch(const std::bad_alloc &) {                                                                                                                     \
        Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_ERROR_MEMORY_ALLOCATION,                                                        \
                                                   Global::tTranslatableStringList() << __FILE__ << __LINE__);                                          \
        throw;                                                                                                                                          \
    }                                                                                                                                                   \
    catch(const std::exception & E) {                                                                                                                   \
        Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_STD_EXCEPTION,                                                                  \
                                                   Global::FmtArgs() << E.what()                                                                        \
                                                                     << __FILE__ << __LINE__);                                                          \
        throw;                                                                                                                                          \
    }                                                                                                                                                   \
    catch(...) {                                                                                                                                        \
        Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION,                                                        \
                                                   Global::tTranslatableStringList() << __FILE__ << __LINE__);                                          \
        throw;                                                                                                                                          \
    }


/****************************************************************************/
/**
 * \brief Catch all exceptions and log them. Exit catch block with return value.
 *
 * \param[in]   i   The return value.
 */
/****************************************************************************/
#define CATCHALL_RETURN(i) \
    catch(const Global::Exception &E) {                                                                                                                 \
        Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_EXCEPTION,                                                                      \
                                                   Global::FmtArgs() << QString::number(E.GetErrorCode()) << E.what()                                   \
                                                                     << __FILE__ << __LINE__);                                                          \
        return i;                                                                                                                                       \
    }                                                                                                                                                   \
    catch(const std::bad_alloc &) {                                                                                                                     \
        Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_ERROR_MEMORY_ALLOCATION,                                                        \
                                                   Global::tTranslatableStringList() << __FILE__ << __LINE__);                                          \
        return i;                                                                                                                                       \
    }                                                                                                                                                   \
    catch(const std::exception & E) {                                                                                                                   \
        Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_STD_EXCEPTION,                                                                  \
                                                   Global::FmtArgs() << E.what()                                                                        \
                                                                     << __FILE__ << __LINE__);                                                          \
        return i;                                                                                                                                       \
    }                                                                                                                                                   \
    catch(...) {                                                                                                                                        \
        Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION,                                                        \
                                                   Global::tTranslatableStringList() << __FILE__ << __LINE__);                                          \
        return i;                                                                                                                                       \
    }

/****************************************************************************/
/**
 * \brief Catch all exceptions in destructor and log them.
 */
/****************************************************************************/
#define CATCHALL_DTOR() \
    catch(const Global::Exception & E) {                                                                                                                \
        try {                                                                                                                                           \
            Global::EventObject::Instance().RaiseEventDtor(Global::EVENT_GLOBAL_EXCEPTION, QString::number(E.GetErrorCode()), E.what(),                 \
                                                           __FILE__, __LINE__);                                                                         \
        }                                                                                                                                               \
        catch (...) {                                                                                                                                   \
        }                                                                                                                                               \
    }                                                                                                                                                   \
    catch(const std::bad_alloc &) {                                                                                                                     \
        try {                                                                                                                                           \
            Global::EventObject::Instance().RaiseEventDtor(Global::EVENT_GLOBAL_ERROR_MEMORY_ALLOCATION, __FILE__, __LINE__);                           \
        }                                                                                                                                               \
        catch (...) {                                                                                                                                   \
        }                                                                                                                                               \
    }                                                                                                                                                   \
    catch(const std::exception & E) {                                                                                                                   \
        try {                                                                                                                                           \
            Global::EventObject::Instance().RaiseEventDtor(Global::EVENT_GLOBAL_STD_EXCEPTION, E.what(), __FILE__, __LINE__);                           \
        }                                                                                                                                               \
        catch (...) {                                                                                                                                   \
        }                                                                                                                                               \
    }                                                                                                                                                   \
    catch(...) {                                                                                                                                        \
        try {                                                                                                                                           \
            Global::EventObject::Instance().RaiseEventDtor(Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, __FILE__, __LINE__);                           \
        }                                                                                                                                               \
        catch (...) {                                                                                                                                   \
        }                                                                                                                                               \
    }

#endif // GLOBAL_EXCEPTION_H
