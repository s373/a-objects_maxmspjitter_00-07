# Microsoft Developer Studio Project File - Name="a-lorenz" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=a-lorenz - WIN32 RELEASE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "a-lorenz.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "a-lorenz.mak" CFG="a-lorenz - WIN32 RELEASE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "a-lorenz - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "obj\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "a-lorenz_EXPORTS" /YX /FD /c
# ADD CPP /nologo /Zp2 /MT /W3 /GX /O2 /I "..\..\..\c74support\max-includes" /I "..\..\..\c74support\msp-includes" /D "WIN_VERSION" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "WIN_EXT_VERSION" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0xc07 /d "NDEBUG"
# ADD RSC /l 0xc07
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ..\..\..\c74support\max-includes\MaxAPI.lib ..\..\..\c74support\msp-includes\MaxAudio.lib /nologo /dll /machine:I386 /out:"../../A-Chaos bin XP/a-lorenz.mxe" /libpath:"../../bin" /export:main
# SUBTRACT LINK32 /pdb:none /incremental:yes /nodefaultlib
# Begin Target

# Name "a-lorenz - Win32 Release"
# Begin Group "Source C"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="../a-lorenz.c"
# End Source File
# End Group
# Begin Group "Header H"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE="../..\maxmspjit\max-includes\ext.h"
# End Source File
# End Group
# End Target
# End Project
