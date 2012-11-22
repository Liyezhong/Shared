/****************************************************************************/
/*! \file DataLogging/Include/EventFilter.h
 *
 *  \brief Definition file for class EventFilter.
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

#ifndef DATALOGGING_EVENTFILTER_H
#define DATALOGGING_EVENTFILTER_H

#include <QSet>
#include <QHash>
#include <QString>
#include <QReadWriteLock>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief This class decides, if an event must be send or not.
 *
 * It must be send, if for a specified file name and line number, the sending is allowed.
 * <b>This class is to be used as a singleton.</b>\n
 * <b>This class is thread safe.</b>
 */
/****************************************************************************/
class EventFilter {
friend class TestEventFilter;
private:
    /****************************************************************************/
    /**
     * \brief This class is used to manage a list of enabled lines.
     *
     * \warning This class is not thread safe!
     */
    /****************************************************************************/
    class FileInfo {
    friend class TestEventFilter;
    friend class EventFilter;
    private:
        QSet<int>       m_Lines;    ///< List of enabled lines.
        /****************************************************************************/
        const FileInfo & operator = (const FileInfo &);     ///< Not implemented.
        /****************************************************************************/
        /**
         * \brief Check if a line is in the list of enabled lines.
         *
         * \param[in]   Line    Line to insert.
         * \return              true if line in list.
         */
        /****************************************************************************/
        inline bool Contains(int Line) const {
            // check if this line is saved in the single line list.
            return m_Lines.contains(Line);
        }
        /****************************************************************************/
        /**
         * \brief Add line to list of enabled lines.
         *
         * \param[in]   Line    Line to insert.
         */
        /****************************************************************************/
        inline void AddLine(int Line) {
            m_Lines.insert(Line);
        }
        /****************************************************************************/
        /**
         * \brief Add line range to list of enabled lines.
         *
         * \param[in]   LineStart   First line to insert.
         * \param[in]   LineStop    Last line to insert.
         */
        /****************************************************************************/
        inline void AddLineRange(int LineStart, int LineStop) {
            // insert all lines including start and end
            Q_ASSERT(LineStart <= LineStop);
            for(int i=LineStart; i<=LineStop; i++) {
                m_Lines.insert(i);
            }
        }
    protected:
    public:
        /****************************************************************************/
        /**
         * \brief Default constructor.
         */
        /****************************************************************************/
        inline FileInfo() {
        }
        /****************************************************************************/
        /**
         * \brief Copy constructor.
         *
         * \param[in]   rOther      Instance to copy from.
         */
        /****************************************************************************/
        inline FileInfo(const FileInfo &rOther) {
            m_Lines = rOther.m_Lines;
        }
        /****************************************************************************/
        /**
         * \brief Destructor.
         */
        /****************************************************************************/
        inline ~FileInfo() {
        }
    };
private:
    QHash<QString, FileInfo>    m_FileInfos;                ///< Information of enabled lines for each file.
    mutable QReadWriteLock      m_SyncObject;               ///< Synchronisation object.
    static EventFilter          m_Instance;                 ///< The one and only instance.
    /****************************************************************************/
    EventFilter(const EventFilter &);                       ///< Not impemented.
    const EventFilter & operator = (const EventFilter &);   ///< Not implemented.
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    EventFilter();
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    ~EventFilter();
    /****************************************************************************/
    /**
     * \brief Remove any path fragments from file name.
     *
     * \param[in]   FileName    File name to be processed.
     * \return                  File name withouth path.
     */
    /****************************************************************************/
    static QString RemovePath(const QString &FileName);
protected:
public:
    /****************************************************************************/
    /**
     * \brief Get reference to instance.
     *
     * \return      Reference to instance.
     */
    /****************************************************************************/
    static inline EventFilter &Instance() {
        return m_Instance;
    }
    /****************************************************************************/
    /**
     * \brief Cleanup saved line range for all files.
     */
    /****************************************************************************/
    void Clear();
    /****************************************************************************/
    /**
     * \brief Add line to list of enabled lines for a specific file.
     *
     * \param[in]   FileName    File name.
     * \param[in]   Line        Line to insert.
     */
    /****************************************************************************/
    void AddLine(const QString &FileName, int Line);
    /****************************************************************************/
    /**
     * \brief Add line range to list of enabled lines for a specific file.
     *
     * \param[in]   FileName    File name.
     * \param[in]   LineStart   First line to insert.
     * \param[in]   LineStop    Last line to insert.
     */
    /****************************************************************************/
    void AddLineRange(const QString &FileName, int LineStart, int LineStop);
    /****************************************************************************/
    /**
     * \brief Check if it is allowed to send event.
     *
     * \param[in]   FileName    File in which event occured.
     * \param[in]   Line        Line in which event occured.
     * \return                  true if allowed.
     */
    /****************************************************************************/
    bool MustSend(const QString &FileName, int Line) const;
}; // end class EventFilter

} // end namespace DataLogging

#endif // DATALOGGING_EVENTFILTER_H
