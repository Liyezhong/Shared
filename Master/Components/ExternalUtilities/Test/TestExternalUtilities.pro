!include("ExternalUtilities.pri") {
    error("ExternalUtilities.pri not found")
}

TARGET = utTestExternalUtilities
SOURCES = TestExternalUtilities.cpp
