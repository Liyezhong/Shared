# -------------------------------------------------
# Project created by QtCreator 2011-05-10T11:49:17
# -------------------------------------------------
QT += network \
    xml \
    xmlpatterns \
    gui

TARGET = AxedaAgent

TEMPLATE = app

SOURCES += ./Source/*.cpp \
           ./Source/*.c
HEADERS += ./Include/*.h

LIBS += -Llibs \
    -L/usr/lib \
    -lAgentEmbedded \
    -lz \
    -lexpat \
    -lssl
