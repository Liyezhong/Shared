!include("NetworkComponentsTest.pri") {
    error("NetworkComponentsTest.pri not found")
}

TEMPLATE = subdirs

SUBDIRS += TestNetworkServer.pro \
           TestNetworkClient.pro \
           TestNetworkServerDevice.pro \
           TestNetworkClientDevice.pro \
           TestConnectionManager.pro \
           TestProtocolRxCommand.pro \
           TestProtocolTxCommand.pro \
           TestCreatorFunctor.pro

CONFIG += ordered
