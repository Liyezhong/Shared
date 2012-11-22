#ifndef TRAY_H
#define TRAY_H

#include <QObject>

namespace DeviceCommandProcessor {

class Tray: public QObject
{
    Q_OBJECT

public:
    Tray();

    bool lockTray();
    bool unlockTray();
    bool openTray();
    bool closeTray();
    bool isOpen() { return m_open; }
    bool isClosed() { return !m_open; }
    bool isLocked() { return m_locked; }
    void setEnabled(bool enable);
    bool executeTransition(bool openRequest, bool lockRequest);

private:
    bool m_open;
    bool m_locked;
    bool m_enabled;

};
}

#endif // TRAY_H
