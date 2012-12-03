#ifndef NETCOMMANDS_CMDSYSTEMERROR_H
#define NETCOMMANDS_CMDSYSTEMERROR_H


#include <Global/Include/Commands/Command.h>

namespace NetCommands {

class CmdSystemError : public Global::Command {

    friend QDataStream & operator << (QDataStream &, const CmdSystemError &);
    friend QDataStream & operator >> (QDataStream &, CmdSystemError &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdSystemError(int Timeout, const QString &StrErrorInfo, bool  Ack = false);
    ~CmdSystemError();
    virtual QString GetName() const;
    QString GetErrorInfo() const;
    bool GetAckFlag() const {
        return m_Ack;
    }
    CmdSystemError();
    inline void SetEvent(quint32 EventID) {

        m_EventID = EventID;
    }

private:
    quint32 m_EventID;
    CmdSystemError(const CmdSystemError &);                       ///< Not implemented.
    const CmdSystemError & operator = (const CmdSystemError &);   ///< Not implemented.
    QString m_StrErrorInfo;
    bool m_Ack;

}; // end class CmdSystemError



/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdSystemError &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    //Stream << Cmd.m_StrErrorInfo;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdSystemError &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_StrErrorInfo;
    return Stream;
}

}// end namespace NetCommands

#endif // NETCOMMANDS_CMDSYSTEMERROR_H
