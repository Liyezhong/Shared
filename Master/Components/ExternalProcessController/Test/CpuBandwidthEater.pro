!include("ExternalProcessTest.pri") {
    error("ExternalProcessTest.pri not found")
}

TARGET = CpuBandwidthEater

HEADERS += CpuBandwidthEater.h

SOURCES += CpuBandwidthEater.cpp
