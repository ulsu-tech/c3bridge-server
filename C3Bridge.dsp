# Microsoft Developer Studio Project File - Name="C3Bridge" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=C3Bridge - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "C3Bridge.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "C3Bridge.mak" CFG="C3Bridge - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "C3Bridge - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "C3Bridge - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "C3Bridge - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".vs\Release"
# PROP Intermediate_Dir ".vs\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "include" /I "dependencies\win32xx" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "VS_RC_60"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib comctl32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "C3Bridge - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".vs\Debug"
# PROP Intermediate_Dir ".vs\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "include" /I "dependencies\win32xx" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "VS_RC_60"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib comctl32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "C3Bridge - Win32 Release"
# Name "C3Bridge - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\source\aboutdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\source\application.cpp
# End Source File
# Begin Source File

SOURCE=.\source\bstring.cpp
# End Source File
# Begin Source File

SOURCE=.\source\bufferstream.cpp
# End Source File
# Begin Source File

SOURCE=.\source\callbackerrorimplementation.cpp
# End Source File
# Begin Source File

SOURCE=.\source\client.cpp
# End Source File
# Begin Source File

SOURCE=.\source\client_cross.cpp
# End Source File
# Begin Source File

SOURCE=.\source\client_file.cpp
# End Source File
# Begin Source File

SOURCE=.\source\client_variables.cpp
# End Source File
# Begin Source File

SOURCE=.\source\cross3.cpp
# End Source File
# Begin Source File

SOURCE=.\source\cross3krc.cpp
# End Source File
# Begin Source File

SOURCE=.\source\main.cpp
# End Source File
# Begin Source File

SOURCE=.\source\mainwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\source\proxy.cpp
# End Source File
# Begin Source File

SOURCE=.\source\ringbuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\source\ringstream.cpp
# End Source File
# Begin Source File

SOURCE=.\source\settings.cpp
# End Source File
# Begin Source File

SOURCE=.\source\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\source\utilities.cpp
# End Source File
# Begin Source File

SOURCE=.\source\view.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\source\aboutdialog.h
# End Source File
# Begin Source File

SOURCE=.\source\application.h
# End Source File
# Begin Source File

SOURCE=.\source\bstring.h
# End Source File
# Begin Source File

SOURCE=.\source\bufferstream.h
# End Source File
# Begin Source File

SOURCE=.\include\c3bi.h
# End Source File
# Begin Source File

SOURCE=.\source\callbackerrorimplementation.h
# End Source File
# Begin Source File

SOURCE=.\source\client.h
# End Source File
# Begin Source File

SOURCE=.\source\cross3.h
# End Source File
# Begin Source File

SOURCE=.\source\cross3krc.h
# End Source File
# Begin Source File

SOURCE=.\source\mainwindow.h
# End Source File
# Begin Source File

SOURCE=.\source\proxy.h
# End Source File
# Begin Source File

SOURCE=.\source\resource.h
# End Source File
# Begin Source File

SOURCE=.\source\ringbuffer.h
# End Source File
# Begin Source File

SOURCE=.\source\ringstream.h
# End Source File
# Begin Source File

SOURCE=.\source\settings.h
# End Source File
# Begin Source File

SOURCE=.\source\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\source\targetver.h
# End Source File
# Begin Source File

SOURCE=.\source\utilities.h
# End Source File
# Begin Source File

SOURCE=.\source\view.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\source\resources.rc
# End Source File
# End Group
# End Target
# End Project
