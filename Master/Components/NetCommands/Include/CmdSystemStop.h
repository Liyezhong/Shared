#ifndef NETCOMMANDS_CMDSYSTEMSTOP_H
#define NETCOMMANDS_CMDSYSTEMSTOP_H

#include <Global/Include/Commands/Command.h>

namespace NetCommands {

class CmdSystemStop : public Global::Command {

    friend QDataStream & operator << (QDataStream &, const CmdSystemStop &);
    friend QDataStream & operator >> (QDataStream &, CmdSystemStop &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdSystemStop(int Timeout, const QString &StrErrorInfo);
    ~CmdSystemStop();
    virtual QString GetName() const;


    QString GetErrorInfo() const;
    CmdSystemStop();
private:
    CmdSystemStop(const CmdSystemStop &);                       ///< Not implemented.
    const CmdSystemStop & operator = (const CmdSystemStop &);   ///< Not implemented.
     QString m_StrErrorInfo;


}; // end class CmdSystemStop

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdSystemStop &Cmd)
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
inline QDataStream & operator >> (QDataStream &Stream, CmdSystemStop &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_StrErrorInfo;
    return Stream;
}


}// end namespace NetCommands
#endif // NETCOMMANDS_CMDSYSTEMSTOP_H
