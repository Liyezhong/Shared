!include("KeyBoard.pri"):error("KeyBoard.pri not found")
TARGET = KeyBoard
HEADERS *= ../Include/*.h
SOURCES *= ../Source/*.cpp
RESOURCES *= ../Resource/Large/*.qrc \
    ../Resource/Large/SpellerButtons/*.qrc \
    ../Resource/Large/Icons/*.qrc \
    ../Resource/Small/Icons/*.qrc \
    ../Resource/Small/SpellerButtons/SpellerSepia.qrc

