!include("TestGPIOThreadController.pri") {
    error("TestGPIOThreadController.pri not found")
}

TARGET = utTestGPIOThreadController

SOURCES +=  TestGPIOThreadController.cpp
INCLUDEPATH += ../../
DEPENDPATH += ../../

UseLibs(Threads Global EventHandler DataLogging GPIOManager NetCommands)
