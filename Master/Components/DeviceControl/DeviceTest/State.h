#ifndef STATE_H
#define STATE_H
 
#include <QState>
 
class CState : public QState
{
    Q_OBJECT
public:
    explicit CState( const QString& name, QState* parent = 0 );
    explicit CState( const QString& name, const QString& prefix, QState* parent = 0 );
 
    QString name() const { return m_name; }
    QString prefix() const { return m_prefix; }
 
public slots:
    void setName( const QString& name ) { m_name = name; }
    void setPrefix( const QString& prefix ) { m_prefix = prefix; }
 
protected:
    virtual void onEntry( QEvent* e );
    virtual void onExit( QEvent* e );
    QState * getTopState();
 
protected:
    QString m_name;
    QString m_prefix;
};
 
#endif
