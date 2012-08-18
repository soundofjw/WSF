# Microsoft Developer Studio Project File - Name="wsflib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wsflib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wsflib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wsflib.mak" CFG="wsflib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wsflib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wsflib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wsflib - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "wsflib - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "wsflib - Win32 Release"
# Name "wsflib - Win32 Debug"
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\lzoconf.h
# End Source File
# Begin Source File

SOURCE=..\WGMPack.h
# End Source File
# Begin Source File

SOURCE=..\WGMPickDlg.h
# End Source File
# Begin Source File

SOURCE=..\WSFComp.h
# End Source File
# Begin Source File

SOURCE=..\WSFDoc.h
# End Source File
# Begin Source File

SOURCE=..\WSFGlob.h
# End Source File
# Begin Source File

SOURCE=..\WSFMod.h
# End Source File
# Begin Source File

SOURCE=..\WSFModV1.h
# End Source File
# Begin Source File

SOURCE=..\WSFPack.h
# End Source File
# Begin Source File

SOURCE=..\WSFPackV1.h
# End Source File
# Begin Source File

SOURCE=..\WSFSets.h
# End Source File
# Begin Source File

SOURCE=..\WSFSpecialTags.h
# End Source File
# Begin Source File

SOURCE=..\WSFTool.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\WGMPack.cpp
# End Source File
# Begin Source File

SOURCE=..\WGMPickDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\WSFComp.c
# End Source File
# Begin Source File

SOURCE=.\WSFLib.cpp
# End Source File
# Begin Source File

SOURCE=..\WSFLoaders.cpp
# End Source File
# Begin Source File

SOURCE=..\WSFLoadersV1.cpp
# End Source File
# Begin Source File

SOURCE=..\WSFMod.cpp
# End Source File
# Begin Source File

SOURCE=..\WSFModV1.cpp
# End Source File
# Begin Source File

SOURCE=..\WSFOgg.cpp
# End Source File
# Begin Source File

SOURCE=..\WSFPack.cpp
# End Source File
# Begin Source File

SOURCE=..\WSFPackV1.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\wsflib.def
# End Source File
# End Target
# End Project
