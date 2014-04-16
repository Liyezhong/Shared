/****************************************************************************/
/*! \file CmdConfigurationFile.h
 *
 *  \brief CmdConfigurationFile command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 24.02.2012
 *   $Author:  $ M.Scherer
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

#ifndef NETCOMMANDS_CMDCONFIGURATIONFILE_H
#define NETCOMMANDS_CMDCONFIGURATIONFILE_H

#include <Global/Include/Commands/Command.h>

#include <QStringList>
#include <QDataStream>

namespace NetCommands {

/// Configuration file type
typedef enum {
    PROGRAM,                ///< Program list file
    PROGRAM_SEQUENCE,       ///< Program sequence file
    REAGENT,                ///< Reagent list file
    REAGENTGROUP,           ///< Reagentgroup list file
    STATION,                ///< Station list file
    REAGENTGROUPCOLOR,       ///< ReagentGriupColorfile
    USER_SETTING,           ///< User setting file
    DEVICE_CONFIGURATION,   ///< Device Configuration file
    STATION_BLG,            ///< Station_blg list intermediate file
    PROGRAM_SEQUENCE_BLG,    ///< Program sequence blg intermediate file
    STATION_MATRIX,          ///< Station matrix file
    PARAMETERSET            ///< ParameterSet file
} FileType_t;

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdConfigurationFile command.
 *
 *      This command transmits the different XML configuration files, like
 *      the program or reagent list, from the Master to the GUI.
 */
/****************************************************************************/
class CmdConfigurationFile : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdConfigurationFile &);
    friend QDataStream & operator >> (QDataStream &, CmdConfigurationFile &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdConfigurationFile(int Timeout, const FileType_t FileType, const QDataStream &FileContent);
    CmdConfigurationFile();
    ~CmdConfigurationFile();
    virtual QString GetName() const;
    FileType_t GetFileType() const;
    QByteArray const& GetFileContent() const;
private:
    CmdConfigurationFile(const CmdConfigurationFile &);                     ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdConfigurationFile & operator = (const CmdConfigurationFile &); ///< Not implemented.
private:
    FileType_t m_FileType;      ///< Configuration file type
    QByteArray m_FileContent;   ///< Configuration file content
}; // end class CmdConfigurationFile

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdConfigurationFile &Cmd) {
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_FileType;
    Stream << Cmd.m_FileContent;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdConfigurationFile &Cmd) {
    qint32 FileType;
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> FileType;
    Cmd.m_FileType = (FileType_t) FileType;
    Stream >> Cmd.m_FileContent;
    return Stream;
}

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDCONFIGURATIONFILE_H
