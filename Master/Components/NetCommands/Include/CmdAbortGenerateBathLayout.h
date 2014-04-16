/****************************************************************************/
/*! \file CmdAbortGenerateBathLayout.h
 *
 *  \brief CmdAbortGenerateBathLayout command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 03.July.2013
 *   $Author:  $ Ningu
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

#ifndef NETCOMMANDS_CMDABORTGENERATEBATHLAYOUT_H
#define NETCOMMANDS_CMDABORTGENERATEBATHLAYOUT_H

#include <Global/Include/Commands/Command.h>

namespace NetCommands {


//! Coverslip Device flags
typedef enum {
    GENERATE_BLG_PROCESS_CANCELLED,
    GENERATE_BLG_PROCESS_ABORTED
} GenBLGProcessFlags_t;

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdAbortGenerateBathLayout command.
 *          Pressing Abort button while generating Bathlayout process sends
 *          this command to main to park robot arms and keep the system back to
 *          normal user action condition.
 */
/****************************************************************************/
class CmdAbortGenerateBathLayout : public Global::Command {
public:
    static QString NAME;    ///< Command name.

    friend QDataStream & operator << (QDataStream &, const CmdAbortGenerateBathLayout &);
    friend QDataStream & operator >> (QDataStream &, CmdAbortGenerateBathLayout &);
    /****************************************************************************/
    CmdAbortGenerateBathLayout(int Timeout, GenBLGProcessFlags_t GenerateBLGProcessStatus);
    CmdAbortGenerateBathLayout();
    ~CmdAbortGenerateBathLayout();
    virtual QString GetName() const;
    GenBLGProcessFlags_t GetGenerateBLGAbortlType() const;
private:
    CmdAbortGenerateBathLayout(const CmdAbortGenerateBathLayout &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *  \return
     */
    /****************************************************************************/
    const CmdAbortGenerateBathLayout & operator = (const CmdAbortGenerateBathLayout &);   ///< Not implemented.

    GenBLGProcessFlags_t m_GenerateBLGProcessStatus;     ///< Blg process aborted/cancelled differentiator flag
};

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdAbortGenerateBathLayout &Cmd) {

    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream <<Cmd.m_GenerateBLGProcessStatus;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdAbortGenerateBathLayout &Cmd) {

    qint32 MessageID;
    // copy base class data
    Cmd.CopyFromStream(Stream);
    Stream >> MessageID;
    Cmd.m_GenerateBLGProcessStatus = (GenBLGProcessFlags_t)MessageID;
    return Stream;
}


} // end namespace NetCommands

#endif // CMDABORTGENERATEBATHLAYOUT_H
