#ifndef GENERICSTATE_H
#define GENERICSTATE_H

#include <QState>
namespace SoftSwitchManager {

template<typename T>
class GenericState: public QState
{
    typedef void (T::*OnEntryCallBack_t)();
public:
    GenericState(T &CallBackObject, OnEntryCallBack_t CallBackFunc)
      :QState(0)
      ,m_CallBackObject(CallBackObject)
      ,m_CallBack(CallBackFunc)
  {

  }
  ~GenericState(){}

protected:
    void onEntry(QEvent *event)
    {
        //First call the base class call back
        QState::onEntry(event);
        //Now call the registered call back function
        (m_CallBackObject.*m_CallBack)();
    }

private:
    T &m_CallBackObject;//!< Object in which call back is present
    OnEntryCallBack_t m_CallBack;//!< Call back function to be called from OnEntry;
};

} //End of namespace SoftSwitchManager
#endif // GENERICSTATE_H
