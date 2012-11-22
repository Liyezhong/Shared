#include "DeviceCommandProcessor/Include/Tray.h"

#include <stdio.h>

namespace DeviceCommandProcessor {

Tray::Tray()
    : m_open(false)
    , m_locked(false)
    , m_enabled(true)
{
}

bool Tray::lockTray()
{
    if (m_open)
        return false;       // cannot lock when open

    m_locked = true;

    return true;
}

bool Tray::executeTransition(bool openRequest, bool lockRequest)
{
    if ((openRequest == m_open) && (lockRequest == m_locked))
        return true;    // we are already in requested state

    if ((openRequest) && (lockRequest))
        return false;   // we cannot both open and lock at same time

    if (m_open != openRequest)
    {
        if (openRequest)
            return openTray();
        else
            return closeTray();
    }

    if (m_locked != lockRequest)
    {
        if (lockRequest)
            return lockTray();
        else
            return unlockTray();
    }

    return false;
}

bool Tray::unlockTray()
{
    if (m_open)
        return false;       // cannot unlock when open

    //if (!locked)
    //    return false;       // cannot unlock when unlocked already

    m_locked = false;

    return true;
}

bool Tray::openTray()
{
    //printf("Tray::openTray\n");
    if ((m_locked) || (!m_enabled))
        return false;

    m_open = true;

    return true;
}

void Tray::setEnabled(bool enable)
{
    m_enabled = enable;
}

bool Tray::closeTray()
{
    //printf("Tray::openTray\n");
    if ((m_locked) || (!m_enabled))
        return false;

    m_open = false;

    return true;
}
}
