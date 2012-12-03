#ifndef NETCOMMANDS_CMDSYSTEMWARNING_H
#define NETCOMMANDS_CMDSYSTEMWARNING_H

#include <Global/Include/Commands/Command.h>
#include <QStringList>

namespace NetCommands {

class CmdSystemWarning : public Global::Command {

    friend QDataStream & operator << (QDataStream &, const CmdSystemWarning &);
    friend QDataStream & operator >> (QDataStream &, CmdSystemWarning &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdSystemWarning(int Timeout, const QString &StrErrorInfo);
    ~CmdSystemWarning();
    virtual QString GetName() const;


    QString GetWarningInfo() const;
    CmdSystemWarning();

    inline void SetTargetComponentList( QStringList & TargetComponents) {
        m_TargetComponentStringList = TargetComponents;
    }

    inline QStringList GetTargetComponentList() {
        return m_TargetComponentStringList;
    }

private:
    CmdSystemWarning(const CmdSystemWarning &);                       ///< Not implemented.
    const CmdSystemWarning & operator = (const CmdSystemWarning &);   ///< Not implemented.
    QString m_StrWarningInfo;
    QStringList m_TargetComponentStringList;

}; // end class CmdSystemWarning



/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdSystemWarning &Cmd)
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
inline QDataStream & operator >> (QDataStream &Stream, CmdSystemWarning &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_StrWarningInfo;
    return Stream;
}

}// end namespace NetCommands

#endif // NETCOMMANDS_CMDSYSTEMWARNING_H

