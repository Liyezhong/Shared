!include("Global.pri") {
    error("Global.pri not found")
}

TEMPLATE = subdirs

SUBDIRS = TestAbstractFactory.pro \
          TestAdjustedTime.pro \
          TestException.pro \
          TestLoggingSource.pro \
          TestMonotonicTime.pro \
          TestRefManager.pro \
          TestSharedPointer.pro \
          TestSystemPaths.pro \
          TestTranslatableString.pro \
          TestTranslator.pro \
          TestUtils.pro \
          TestCommands.pro

# Tests for IndexT class
#
SUBDIRS += TestIndexT_Default.pro
SUBDIRS += TestIndexT_0_Extra.pro
SUBDIRS += TestIndexT_3_Extra.pro

CONFIG += ordered
