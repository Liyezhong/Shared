!include("StateMachines.pri") {
    error("StateMachines.pri not found")
}

TEMPLATE = subdirs

SUBDIRS += TestState.pro \
           TestStateEngine.pro


CONFIG += ordered
