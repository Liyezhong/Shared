# Microsoft Developer Studio Project File - Name="AgentEmbeddedNT" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=AgentEmbeddedNT - Win32 Debug SSL
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AgentEmbeddedNT.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AgentEmbeddedNT.mak" CFG="AgentEmbeddedNT - Win32 Debug SSL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AgentEmbeddedNT - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AgentEmbeddedNT - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AgentEmbeddedNT - Win32 Release SSL" (based on "Win32 (x86) Static Library")
!MESSAGE "AgentEmbeddedNT - Win32 Debug SSL" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AgentEmbeddedNT - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I ".\\" /I "Include" /I "Sysdeps\Win32" /I "Compat" /I "Libsrc\expat\xmlparse" /I "Libsrc\libdes" /I "Libsrc\zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__GATEWAY__" /D "ENABLE_REMOTE_SESSION" /D "ENABLE_FILE_TRANSFER" /D "ENABLE_LARGEFILE64" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\AgentEmbedded.lib"

!ELSEIF  "$(CFG)" == "AgentEmbeddedNT - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I ".\\" /I "Include" /I "Sysdeps\Win32" /I "Compat" /I "Libsrc\expat\xmlparse" /I "Libsrc\libdes" /I "Libsrc\zlib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__GATEWAY__" /D "ENABLE_REMOTE_SESSION" /D "ENABLE_FILE_TRANSFER" /D "ENABLE_LARGEFILE64" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\AgentEmbedded.lib"

!ELSEIF  "$(CFG)" == "AgentEmbeddedNT - Win32 Release SSL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AgentEmbeddedNT___Win32_Release_SSL"
# PROP BASE Intermediate_Dir "AgentEmbeddedNT___Win32_Release_SSL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_SSL"
# PROP Intermediate_Dir "Release_SSL"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I ".\\" /I "Include" /I "Sysdeps\Win32" /I "Compat" /I "Libsrc\expat\xmlparse" /I "Libsrc\libdes" /I "Libsrc\zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "ENABLE_REMOTE_SESSION" /D "ENABLE_FILE_TRANSFER" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I ".\\" /I "Include" /I "Sysdeps\Win32" /I "Compat" /I "Libsrc\expat\xmlparse" /I "Libsrc\libdes" /I "Libsrc\zlib" /I "openssl\include" /D "NDEBUG" /D "__GATEWAY__" /D "HAVE_OPENSSL" /D "ENABLE_SSL" /D "WIN32" /D "_MBCS" /D "_LIB" /D "ENABLE_REMOTE_SESSION" /D "ENABLE_FILE_TRANSFER" /D "ENABLE_LARGEFILE64" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Release\AgentEmbedded.lib"
# ADD LIB32 /nologo /out:"Release_SSL\AgentEmbedded.lib"

!ELSEIF  "$(CFG)" == "AgentEmbeddedNT - Win32 Debug SSL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AgentEmbeddedNT___Win32_Debug_SSL"
# PROP BASE Intermediate_Dir "AgentEmbeddedNT___Win32_Debug_SSL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_SSL"
# PROP Intermediate_Dir "Debug_SSL"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I ".\\" /I "Include" /I "Sysdeps\Win32" /I "Compat" /I "Libsrc\expat\xmlparse" /I "Libsrc\libdes" /I "Libsrc\zlib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "ENABLE_REMOTE_SESSION" /D "ENABLE_FILE_TRANSFER" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I ".\\" /I "Include" /I "Sysdeps\Win32" /I "Compat" /I "Libsrc\expat\xmlparse" /I "Libsrc\libdes" /I "Libsrc\zlib" /I "openssl\include" /D "_DEBUG" /D "__GATEWAY__" /D "HAVE_OPENSSL" /D "ENABLE_SSL" /D "WIN32" /D "_MBCS" /D "_LIB" /D "ENABLE_REMOTE_SESSION" /D "ENABLE_FILE_TRANSFER" /D "ENABLE_LARGEFILE64" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\AgentEmbedded.lib"
# ADD LIB32 /nologo /out:"Debug_SSL\AgentEmbedded.lib"

!ENDIF 

# Begin Target

# Name "AgentEmbeddedNT - Win32 Release"
# Name "AgentEmbeddedNT - Win32 Debug"
# Name "AgentEmbeddedNT - Win32 Release SSL"
# Name "AgentEmbeddedNT - Win32 Debug SSL"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AeAgentRestart.c
# End Source File
# Begin Source File

SOURCE=.\AeContainers.c
# End Source File
# Begin Source File

SOURCE=.\AeDRM.c
# End Source File
# Begin Source File

SOURCE=.\AeDRMEMessage.c
# End Source File
# Begin Source File

SOURCE=.\AeDRMQueue.c
# End Source File
# Begin Source File

SOURCE=.\AeDRMSOAP.c
# End Source File
# Begin Source File

SOURCE=.\AeFileDownload.c
# End Source File
# Begin Source File

SOURCE=.\AeFileTransfer.c
# End Source File
# Begin Source File

SOURCE=.\AeFileUpload.c
# End Source File
# Begin Source File

SOURCE=.\AeGzip.c
# End Source File
# Begin Source File

SOURCE=.\AeInitLocal.c
# End Source File
# Begin Source File

SOURCE=.\AeInterface.c
# End Source File
# Begin Source File

SOURCE=.\AeNullFileFilter.c
# End Source File
# Begin Source File

SOURCE=.\Sysdeps\Win32\AeOS.c
# End Source File
# Begin Source File

SOURCE=.\AeRemoteChannel.c
# End Source File
# Begin Source File

SOURCE=.\AeRemoteDesktopProbe.c
# End Source File
# Begin Source File

SOURCE=.\AeRemoteDirectConnection.c
# End Source File
# Begin Source File

SOURCE=.\AeRemoteDirectTransport.c
# End Source File
# Begin Source File

SOURCE=.\AeRemoteHTTPTransport.c
# End Source File
# Begin Source File

SOURCE=.\AeRemoteSession.c
# End Source File
# Begin Source File

SOURCE=.\AeTar.c
# End Source File
# Begin Source File

SOURCE=.\AeUtil.c
# End Source File
# Begin Source File

SOURCE=.\AeWebAuth.c
# End Source File
# Begin Source File

SOURCE=.\AeWebCommon.c
# End Source File
# Begin Source File

SOURCE=.\AeWebConnection.c
# End Source File
# Begin Source File

SOURCE=.\AeWebConnectionSOCKS.c
# End Source File
# Begin Source File

SOURCE=.\AeWebRequest.c
# End Source File
# Begin Source File

SOURCE=.\AeWebTransaction.c
# End Source File
# Begin Source File

SOURCE=.\AeWebUserAgent.c
# End Source File
# Begin Source File

SOURCE=.\AeXML.c
# End Source File
# Begin Source File

SOURCE=.\Compat\md4c.c
# End Source File
# Begin Source File

SOURCE=.\Compat\md5c.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Ae.h
# End Source File
# Begin Source File

SOURCE=.\AeContainers.h
# End Source File
# Begin Source File

SOURCE=.\AeDRM.h
# End Source File
# Begin Source File

SOURCE=.\AeDRMEMessage.h
# End Source File
# Begin Source File

SOURCE=.\AeDRMQueue.h
# End Source File
# Begin Source File

SOURCE=.\Include\AeDRMSOAP.h
# End Source File
# Begin Source File

SOURCE=.\Include\AeError.h
# End Source File
# Begin Source File

SOURCE=.\AeFileDownload.h
# End Source File
# Begin Source File

SOURCE=.\AeFileTransfer.h
# End Source File
# Begin Source File

SOURCE=.\AeFileUpload.h
# End Source File
# Begin Source File

SOURCE=.\AeGlobals.h
# End Source File
# Begin Source File

SOURCE=.\AeGzip.h
# End Source File
# Begin Source File

SOURCE=.\Include\AeInterface.h
# End Source File
# Begin Source File

SOURCE=.\AeNullFileFilter.h
# End Source File
# Begin Source File

SOURCE=.\Include\AeOS.h
# End Source File
# Begin Source File

SOURCE=.\Sysdeps\Win32\AeOSLocal.h
# End Source File
# Begin Source File

SOURCE=.\AeRemoteChannel.h
# End Source File
# Begin Source File

SOURCE=.\AeRemoteDesktopProbe.h
# End Source File
# Begin Source File

SOURCE=.\AeRemoteDirectConnection.h
# End Source File
# Begin Source File

SOURCE=.\AeRemoteDirectTransport.h
# End Source File
# Begin Source File

SOURCE=.\AeRemoteHTTPTransport.h
# End Source File
# Begin Source File

SOURCE=.\AeRemoteSession.h
# End Source File
# Begin Source File

SOURCE=.\AeTar.h
# End Source File
# Begin Source File

SOURCE=.\Include\AeTypes.h
# End Source File
# Begin Source File

SOURCE=.\AeUtil.h
# End Source File
# Begin Source File

SOURCE=.\Include\AeVersion.h
# End Source File
# Begin Source File

SOURCE=.\AeWebAuth.h
# End Source File
# Begin Source File

SOURCE=.\AeWebCommon.h
# End Source File
# Begin Source File

SOURCE=.\AeWebConnection.h
# End Source File
# Begin Source File

SOURCE=.\AeWebConnectionSOCKS.h
# End Source File
# Begin Source File

SOURCE=.\Include\AeWebRequest.h
# End Source File
# Begin Source File

SOURCE=.\AeWebTransaction.h
# End Source File
# Begin Source File

SOURCE=.\AeWebUserAgent.h
# End Source File
# Begin Source File

SOURCE=.\AeXML.h
# End Source File
# End Group
# End Target
# End Project
