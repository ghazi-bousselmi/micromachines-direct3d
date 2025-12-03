# Microsoft Developer Studio Generated NMAKE File, Based on micro machines 3d.dsp
!IF "$(CFG)" == ""
CFG=micro machines 3d - Win32 Debug
!MESSAGE No configuration specified. Defaulting to micro machines 3d - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "micro machines 3d - Win32 Release" && "$(CFG)" != "micro machines 3d - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "micro machines 3d.mak" CFG="micro machines 3d - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "micro machines 3d - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "micro machines 3d - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "micro machines 3d - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\micro machines 3d.exe"


CLEAN :
	-@erase "$(INTDIR)\d3dapp.obj"
	-@erase "$(INTDIR)\d3dfont.obj"
	-@erase "$(INTDIR)\d3dutil.obj"
	-@erase "$(INTDIR)\dmutil.obj"
	-@erase "$(INTDIR)\dxutil.obj"
	-@erase "$(INTDIR)\gamemenu.obj"
	-@erase "$(INTDIR)\micro machines 3d.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\winmain.res"
	-@erase "$(OUTDIR)\micro machines 3d.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\micro machines 3d.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\winmain.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\micro machines 3d.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\micro machines 3d.pdb" /machine:I386 /out:"$(OUTDIR)\micro machines 3d.exe" 
LINK32_OBJS= \
	"$(INTDIR)\winmain.res" \
	"$(INTDIR)\gamemenu.obj" \
	"$(INTDIR)\d3dfont.obj" \
	"$(INTDIR)\d3dutil.obj" \
	"$(INTDIR)\dmutil.obj" \
	"$(INTDIR)\dxutil.obj" \
	"$(INTDIR)\d3dapp.obj" \
	"$(INTDIR)\micro machines 3d.obj"

"$(OUTDIR)\micro machines 3d.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "micro machines 3d - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\micro machines 3d.exe" "$(OUTDIR)\micro machines 3d.bsc"


CLEAN :
	-@erase "$(INTDIR)\d3dapp.obj"
	-@erase "$(INTDIR)\d3dapp.sbr"
	-@erase "$(INTDIR)\d3dfont.obj"
	-@erase "$(INTDIR)\d3dfont.sbr"
	-@erase "$(INTDIR)\d3dutil.obj"
	-@erase "$(INTDIR)\d3dutil.sbr"
	-@erase "$(INTDIR)\dmutil.obj"
	-@erase "$(INTDIR)\dmutil.sbr"
	-@erase "$(INTDIR)\dxutil.obj"
	-@erase "$(INTDIR)\dxutil.sbr"
	-@erase "$(INTDIR)\gamemenu.obj"
	-@erase "$(INTDIR)\gamemenu.sbr"
	-@erase "$(INTDIR)\micro machines 3d.obj"
	-@erase "$(INTDIR)\micro machines 3d.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\winmain.res"
	-@erase "$(OUTDIR)\micro machines 3d.bsc"
	-@erase "$(OUTDIR)\micro machines 3d.exe"
	-@erase "$(OUTDIR)\micro machines 3d.ilk"
	-@erase "$(OUTDIR)\micro machines 3d.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\micro machines 3d.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\winmain.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\micro machines 3d.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\gamemenu.sbr" \
	"$(INTDIR)\d3dfont.sbr" \
	"$(INTDIR)\d3dutil.sbr" \
	"$(INTDIR)\dmutil.sbr" \
	"$(INTDIR)\dxutil.sbr" \
	"$(INTDIR)\d3dapp.sbr" \
	"$(INTDIR)\micro machines 3d.sbr"

"$(OUTDIR)\micro machines 3d.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=dsound.lib dxguid.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3d8.lib d3dx8.lib winmm.lib dxerr8.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\micro machines 3d.pdb" /debug /machine:I386 /out:"$(OUTDIR)\micro machines 3d.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\winmain.res" \
	"$(INTDIR)\gamemenu.obj" \
	"$(INTDIR)\d3dfont.obj" \
	"$(INTDIR)\d3dutil.obj" \
	"$(INTDIR)\dmutil.obj" \
	"$(INTDIR)\dxutil.obj" \
	"$(INTDIR)\d3dapp.obj" \
	"$(INTDIR)\micro machines 3d.obj"

"$(OUTDIR)\micro machines 3d.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("micro machines 3d.dep")
!INCLUDE "micro machines 3d.dep"
!ELSE 
!MESSAGE Warning: cannot find "micro machines 3d.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "micro machines 3d - Win32 Release" || "$(CFG)" == "micro machines 3d - Win32 Debug"
SOURCE=.\d3dapp.cpp

!IF  "$(CFG)" == "micro machines 3d - Win32 Release"


"$(INTDIR)\d3dapp.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "micro machines 3d - Win32 Debug"


"$(INTDIR)\d3dapp.obj"	"$(INTDIR)\d3dapp.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d3dfont.cpp

!IF  "$(CFG)" == "micro machines 3d - Win32 Release"


"$(INTDIR)\d3dfont.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "micro machines 3d - Win32 Debug"


"$(INTDIR)\d3dfont.obj"	"$(INTDIR)\d3dfont.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d3dutil.cpp

!IF  "$(CFG)" == "micro machines 3d - Win32 Release"


"$(INTDIR)\d3dutil.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "micro machines 3d - Win32 Debug"


"$(INTDIR)\d3dutil.obj"	"$(INTDIR)\d3dutil.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dmutil.cpp

!IF  "$(CFG)" == "micro machines 3d - Win32 Release"


"$(INTDIR)\dmutil.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "micro machines 3d - Win32 Debug"


"$(INTDIR)\dmutil.obj"	"$(INTDIR)\dmutil.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dxutil.cpp

!IF  "$(CFG)" == "micro machines 3d - Win32 Release"


"$(INTDIR)\dxutil.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "micro machines 3d - Win32 Debug"


"$(INTDIR)\dxutil.obj"	"$(INTDIR)\dxutil.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\gamemenu.cpp

!IF  "$(CFG)" == "micro machines 3d - Win32 Release"


"$(INTDIR)\gamemenu.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "micro machines 3d - Win32 Debug"


"$(INTDIR)\gamemenu.obj"	"$(INTDIR)\gamemenu.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=".\micro machines 3d.cpp"

!IF  "$(CFG)" == "micro machines 3d - Win32 Release"


"$(INTDIR)\micro machines 3d.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "micro machines 3d - Win32 Debug"


"$(INTDIR)\micro machines 3d.obj"	"$(INTDIR)\micro machines 3d.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\winmain.rc

"$(INTDIR)\winmain.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

