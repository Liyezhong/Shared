!include("Global.pri") {
    error("Global.pri not found")
}

Testing = _3_Extra

!include(TestIndexT.pri) {
	error("TestIndexT.pri not found")
}
