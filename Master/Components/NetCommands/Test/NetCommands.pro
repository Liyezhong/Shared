!include("DummyComponent.pri") {
    error("DummyComponent.pri not found")
}

TEMPLATE = subdirs

SUBDIRS =   TestDummyComponent.pro
