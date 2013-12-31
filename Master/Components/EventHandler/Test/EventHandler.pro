!include("EventHandler.pri") {
    error("EventHandler.pri not found")
}

TEMPLATE = subdirs

SUBDIRS =  TestEventCSVInfo.pro\
           TestEventHandlerThreadController.pro\
           TestStateHandler.pro
