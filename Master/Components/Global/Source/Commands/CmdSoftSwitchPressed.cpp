#include <Global/Include/Commands/CmdSoftSwitchPressed.h>

namespace Global {

QString CmdSoftSwitchPressed::NAME           = "Global::CmdSoftSwitchPressed";
int     CmdSoftSwitchPressed::m_CmdTimeout   = 5000;

/****************************************************************************/
CmdSoftSwitchPressed::CmdSoftSwitchPressed()
    : Global::Command(m_CmdTimeout)
{
}

/****************************************************************************/
CmdSoftSwitchPressed::~CmdSoftSwitchPressed() {
}

} // end namespace Global
