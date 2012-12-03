# include pri file from Master/Test

DEFINES *= UNITTESTS

!include(../../../Test/Platform.pri) {
    error("../../../Test/Platform.pri not found")
}

# currently not used because of
# problems with target platform:
#
# LIBS += -lboost_date_time-mt
