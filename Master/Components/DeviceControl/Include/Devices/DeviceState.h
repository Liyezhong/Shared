/****************************************************************************/
/*! \file   DeviceState.h
 *
 *  \brief  Definition file for class CState. Base class of all states
 *
 *  \version  0.1
 *  \date     2012-08-26
 *  \author   Torsten Scheck
 *
 *  \b Description:
 *
 *   N/A
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#ifndef DEVICECONTROL_DEVICESTATE_H
#define DEVICECONTROL_DEVICESTATE_H
 
#include <QState>
 
namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Base class of all states
 */
/****************************************************************************/
class CState : public QState
{
    Q_OBJECT
public:
    explicit CState(const QString& strName, QState* p_Parent = 0);
    explicit CState(const QString& strName, const QString& strPrefix, QState* p_Parent = 0);
 
    /****************************************************************************/
    /*!
     *  \brief  Returns the name
     *
     *  \return m_name = name of the state
     */
    /****************************************************************************/
    QString name() const { return m_name; }

    /****************************************************************************/
    /*!
     *  \brief  Returns the prefix
     *
     *  \return m_prefix = prifix name
     */
    /****************************************************************************/
    QString prefix() const { return m_prefix; }
 
public slots:
    /****************************************************************************/
    /*!
     *  \brief  To set/modify name of the state
     *
     *  \iparam statename = name of the state
     */
    /****************************************************************************/
    void setName(const QString& statename) { m_name = statename; }

    /****************************************************************************/
    /*!
     *  \brief  To set/modify prefix
     *
     *  \iparam Stateprefix = parent of the state
     */
    /****************************************************************************/
    void setPrefix(const QString& Stateprefix) { m_prefix = Stateprefix; }
 
protected:
    virtual void onEntry( QEvent* e );
    virtual void onExit( QEvent* e );
    QState * getTopState();
 
protected:
    QString m_name;         //!< Name of the state
    QString m_prefix;       //!< Prefix for printing state name
    bool m_DebugOutput;     //!< To enable or disable debug output

private:
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CState)

};
 
} //namespace

#endif /* DEVICECONTROL_DEVICESTATE_H */
