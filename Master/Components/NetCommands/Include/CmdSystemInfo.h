#ifndef NETCOMMANDS_CMDSYSTEMINFO_H
#define NETCOMMANDS_CMDSYSTEMINFO_H


#include <Global/Include/Commands/Command.h>

namespace NetCommands {

class CmdSystemInfo : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdSystemInfo &);
    friend QDataStream & operator >> (QDataStream &, CmdSystemInfo &);

public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdSystemInfo(int Timeout, const QString &StrErrorInfo);
    ~CmdSystemInfo();
    virtual QString GetName() const;


    QString GetErrorInfo() const;
    CmdSystemInfo();
private:
    CmdSystemInfo(const CmdSystemInfo &);                       ///< Not implemented.
    const CmdSystemInfo & operator = (const CmdSystemInfo &);   ///< Not implemented.
     QString m_StrErrorInfo;

}; // end class CmdSystemInfo
/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdSystemInfo &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_StrErrorInfo;
    return Stream;
}

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream from.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdSystemInfo &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_StrErrorInfo;
    return Stream;
}
}// end namespace NetCommands



#endif // NETCOMMANDS_CMDSYSTEMINFO_H
