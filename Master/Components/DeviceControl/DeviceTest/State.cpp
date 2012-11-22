// from: http://qt-project.org/doc/qt-4.7/statemachine-api.html#comments
// added getTopState()
#include "State.h"
#include <QStateMachine>
#include <QDebug>

CState::CState( const QString& name, QState* parent )
    : QState( parent ),
      m_name( name ),
      m_prefix()
{
}
 
CState::CState( const QString& name, const QString& prefix, QState* parent )
    : QState( parent ),
      m_name( name ),
      m_prefix( prefix )
{
}
 
void CState::onEntry( QEvent* e )
{
    Q_UNUSED( e );
 
    // Print out the state we are entering and its parents
    QString state = m_name;
    CState* parent = dynamic_cast<CState*>( parentState() );
    while ( parent != 0 )
    {
        state = parent->name() + "->" + state;
        parent = dynamic_cast<CState*>( parent->parentState() );
    }
    qDebug() << m_prefix << "Entering state:" << state;
    //qDebug() << m_prefix << "Entering state:" << state << this;
}
 
void CState::onExit( QEvent* e )
{
    Q_UNUSED( e );
 
    // Print out the state we are exiting and its parents
    QString state = m_name;
    CState* parent = dynamic_cast<CState*>( parentState() );
    while ( parent != 0 )
    {
        state = parent->name() + "->" + state;
        parent = dynamic_cast<CState*>( parent->parentState() );
    }
    qDebug() << m_prefix << "Exiting state:" << state;
}

QState * CState::getTopState()
{
    QState *s = this;
    while ( s->parentState() != s->machine() ) {
         s = s->parentState();
    }
    return s;
}
