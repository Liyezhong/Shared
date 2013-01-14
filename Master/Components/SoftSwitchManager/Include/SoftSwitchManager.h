#ifndef SOFTSWITCHMANAGER_H
#define SOFTSWITCHMANAGER_H

#include <QObject>

namespace SoftSwitchManager {

class SoftSwitchManager:public QObject
{
public:
    SoftSwitchManager();
    ~SoftSwitchManager();
    Q_DISABLE_COPY(SoftSwitchManager) //Disable copy and assignment
private:

};

}//End of namespace SoftSwitchManager
#endif // SOFTSWITCHMANAGER_H
