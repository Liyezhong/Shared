!include("Global.pri") {
    error("Global.pri not found")
}

Testing = _Default

!include(TestIndexT.pri) {
	error("TestIndexT.pri not found")
}
