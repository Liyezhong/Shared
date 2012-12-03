!include("Global.pri") {
    error("Global.pri not found")
}

Testing = _0_Extra

!include(TestIndexT.pri) {
	error("TestIndexT.pri not found")
}
