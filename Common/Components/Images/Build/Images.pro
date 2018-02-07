!include("Images.pri"):error("Images.pri not found")
TARGET = Images
RESOURCES *= ../Resource/*.qrc \
    ../Resource/BathButton/*.qrc \
    ../Resource/BathRack/*.qrc \ # ../Resource/CheckBox/*.qrc \
# ../Resource/Digits/*.qrc \
# l    ../Resource/Grid/*.qrc \
# ../Resource/IconPushButton/*.qrc \
# ../Resource/Icons/Configuration/*.qrc \
# ../Resource/Icons/GridButton-Riffle/*.qrc \
# ../Resource/Icons/Lock/*.qrc \
# ../Resource/Icons/Main_Tabs/*.qrc \
# ../Resource/Icons/MISC/*.qrc \
# ../Resource/Icons/Move/*.qrc \
# ../Resource/Icons/Scroll_Indicator/*.qrc \
# ../Resource/Icons/User_Status/*.qrc \
# ../Resource/Icons/User_Status_TextButton/*.qrc \
# ../Resource/Icons/Status_Bar/*.qrc \
# ../Resource/LAS-MessageBox-Icons/Icons_50x50/*.qrc \
# ../Resource/Popup/*.qrc \
    ../Resource/Rack/*.qrc \
    ../Resource/Rack/Progress/*.qrc \
    ../Resource/Rack/RackColor/*.qrc \ # ../Resource/RadioButton/*.qrc \
# ../Resource/SlideSwitch/*.qrc \
# ../Resource/Tab_Control/*.qrc \
# ../Resource/TextButton/*.qrc \
# ../Resource/TextButton/TextButtonsColored/*.qrc \
    ../Resource/Rack/RackStatus_Load_Unload/RackStatus.qrc \
    ../Resource/Popup/Popup.qrc
