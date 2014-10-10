/****************************************************************************/
/*! \file CmdDisableGUIAction.cpp
 *
 *  \brief Definition of CmdDisableGUIAction
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-12-03
 *  $Author:    $ C.Adaragunchi
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2011 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/
#ifndef CMDDISABLEGUIACTION_H
#define CMDDISABLEGUIACTION_H
#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/// GUI Action Group type
typedef enum {
    ACTION_GROUP1,  //!< Action group used to disable Generate BathLayout and
                    //!< Fill level scan buttons when Hood and Drawers are open
    ACTION_GROUP2,
    ACTION_GROUP3
} ActionGroupType_t;

/****************************************************************************/
/**
 * \brief  This command is sent from Main thread to GUI thread to inform that GUI
 *         action is not possible as of now.
 *
 */
/****************************************************************************/
class CmdDisableGUIAction : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdDisableGUIAction &);
    friend QDataStream & operator >> (QDataStream &, CmdDisableGUIAction &);

private:
    /****************************************************************************/
    CmdDisableGUIAction(const CmdDisableGUIAction &);                     ///< Not implemented.

    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdDisableGUIAction & operator = (const CmdDisableGUIAction &); ///< Not implemented.

public:
    static QString  NAME;                   ///< Command name.
    ActionGroupType_t m_ActionGroupType;    ///< GUI Action group type
    bool m_Enable ;                         ///< true to enable action, false disable action in GUI

    /****************************************************************************/
    /*!
     * \brief   Constructor
     */
    /****************************************************************************/
    CmdDisableGUIAction();
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam   TimeOut               Timeout for command.
     * \iparam   ActionGroupType       ActionGroupType
     * \iparam   Enable                Enable/Disable GUI action
     */
    /****************************************************************************/
    CmdDisableGUIAction(int TimeOut, const ActionGroupType_t ActionGroupType, bool Enable);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdDisableGUIAction();


    ActionGroupType_t GetActionGroupType() const;
    bool GetGUIEnableState() const;

    /****************************************************************************/
    /**
     * \brief Get command name.
     *
     * \return  Command name.
     */
    /****************************************************************************/
    virtual QString GetName() const {
        return NAME;
    }

}; // end class CmdDisableGUIAction

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out] Stream Stream to stream into.
 * \iparam  Cmd The command to stream.
 * \return  Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdDisableGUIAction &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_Enable;
    Stream << Cmd.m_ActionGroupType;
    return Stream;
}

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream from.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdDisableGUIAction &Cmd)
{
    quint32 ActionType;
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_Enable;
    Stream >> ActionType;
    Cmd.m_ActionGroupType = (ActionGroupType_t)ActionType;
    return Stream;
}

}
#endif // CMDDISABLEGUIACTION_H
