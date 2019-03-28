# $Id: planner.mak,v 1.35 1998/01/22 20:05:50 darseq Exp $
!if !$d(BCB)
BCB = $(MAKEDIR)\..
!endif

# ---------------------------------------------------------------------------
# IDE SECTION
# ---------------------------------------------------------------------------
# The following section of the project makefile is managed by the BCB IDE.
# It is recommended to use the IDE to change any of the values in this
# section.
# ---------------------------------------------------------------------------

VERSION = BCB.06.00
# ---------------------------------------------------------------------------
PROJECT = planner.exe
OBJFILES = planner.obj Clients.obj Tracks.obj Codes.obj Fbase.obj Fmain.obj \
    Fpending.obj Fcomplete.obj Foptions.obj Fmdetail.obj Farchive.obj \
    Fdetails.obj Fcodes.obj Frange.obj Fprint.obj Ffind.obj Fentries.obj \
    Fcreated.obj Fschedule.obj Femployee.obj Fimport.obj Fmail.obj ThreadModify.obj \
    Odbc.obj Query.obj Exceptions.obj TRowGrid\RowGrid.obj TDataRows\DataRows.obj \
    TDataRows\RowControl.obj FastMM4BCB\FastMM4.obj \
    FastMM4BCB\FastMM4Messages.obj FastMM4BCB\FastMM4BCB.obj
RESFILES = planner.res ENU\PLAN_ENU.res
MAINSOURCE = planner.cpp
RESDEPEN = $(RESFILES) Fbase.dfm Fmain.dfm Fpending.dfm Fcomplete.dfm Foptions.dfm \
    Fmdetail.dfm Farchive.dfm Fdetails.dfm Fcodes.dfm Frange.dfm Fprint.dfm \
    Ffind.dfm Fentries.dfm Fcreated.dfm Fschedule.dfm Femployee.dfm Fimport.dfm \
    Fmail.dfm
LIBFILES = Odbc32.lib versit.lib gdiplus.lib
IDLFILES = 
IDLGENFILES = 
LIBRARIES = ScrollBox.lib TRowControl.lib TDataRows.lib CheckBox.lib vclx.lib \
    TRowGrid.lib rtl.lib vcl.lib
PACKAGES = vcl.bpi rtl.bpi vclx.bpi designide.bpi
SPARELIBS = vcl.lib rtl.lib TRowGrid.lib vclx.lib CheckBox.lib TDataRows.lib \
    TRowControl.lib ScrollBox.lib
DEFFILE = 
OTHERFILES = 
# ---------------------------------------------------------------------------
DEBUGLIBPATH = $(BCB)\lib\debug
RELEASELIBPATH = $(BCB)\lib\release
#USERDEFINES = LICENSENR="3c049bc78e3f1258fc85d3ce10432475";MS_SQL;_USE_OLD_RW_STL
SYSDEFINES = NO_STRICT
INCLUDEPATH = FastMM4BCB;TDataRows;TRowGrid;$(BCB)\include;$(BCB)\include\vcl
LIBPATH = FastMM4BCB;TDataRows;TRowGrid;versit;$(BCB)\Projects\Lib;$(BCB)\lib\obj;$(BCB)\lib
WARNINGS= -w-par
PATHCPP = .;TRowGrid;TDataRows;FastMM4BCB
PATHASM = .;
PATHPAS = .;FastMM4BCB
PATHRC = .;ENU
PATHOBJ = .;$(LIBPATH)
# ---------------------------------------------------------------------------
CFLAG1 = -O2 -H=$(BCB)\lib\vcl60.csm -Hc -Vx -Ve -X- -a8 -b- -k- -vi -c -tW -tWM -w-8004 -w-8026 -w-8027 -w-msg
IDLCFLAGS = 
PFLAGS = -$Y- -$L- -$D- -$A8 -v -JPHNE -M
RFLAGS = 
AFLAGS = /mx /w2 /zn
LFLAGS = -D"" -aa -Tpe -GD -x -Gn /r
# ---------------------------------------------------------------------------
ALLOBJ = c0w32.obj sysinit.obj $(OBJFILES)
ALLRES = $(RESFILES)
ALLLIB = $(LIBFILES) $(LIBRARIES) import32.lib cp32mt.lib
# ---------------------------------------------------------------------------
!ifdef IDEOPTIONS

[Version Info]
IncludeVerInfo=0
AutoIncBuild=0
MajorVer=1
MinorVer=0
Release=0
Build=0
Debug=0
PreRelease=0
Special=0
Private=0
DLL=0

[Version Info Keys]
CompanyName=
FileDescription=
FileVersion=1.0.0.0
InternalName=
LegalCopyright=
LegalTrademarks=
OriginalFilename=
ProductName=
ProductVersion=1.0.0.0
Comments=

[Debugging]
DebugSourceDirs=$(BCB)\source\vcl

!endif





# ---------------------------------------------------------------------------
# MAKE SECTION
# ---------------------------------------------------------------------------
# This section of the project file is not used by the BCB IDE.  It is for
# the benefit of building from the command-line using the MAKE utility.
# ---------------------------------------------------------------------------

.autodepend
# ---------------------------------------------------------------------------
#!if "$(USERDEFINES)" != ""
#AUSERDEFINES = -d$(USERDEFINES:;= -d)
#!else
#AUSERDEFINES =
#!endif

!if !$d(BCC32)
BCC32 = bcc32
!endif

!if !$d(CPP32)
CPP32 = cpp32
!endif

!if !$d(DCC32)
DCC32 = dcc32
!endif

!if !$d(TASM32)
TASM32 = tasm32
!endif

!if !$d(LINKER)
LINKER = ilink32
!endif

!if !$d(BRCC32)
BRCC32 = brcc32
!endif


# ---------------------------------------------------------------------------
!if $d(PATHCPP)
.PATH.CPP = $(PATHCPP)
.PATH.C   = $(PATHCPP)
!endif

!if $d(PATHPAS)
.PATH.PAS = $(PATHPAS)
!endif

!if $d(PATHASM)
.PATH.ASM = $(PATHASM)
!endif

!if $d(PATHRC)
.PATH.RC  = $(PATHRC)
!endif

!if $d(PATHOBJ)
.PATH.OBJ  = $(PATHOBJ)
!endif
# ---------------------------------------------------------------------------
$(PROJECT): $(OTHERFILES) $(IDLGENFILES) $(OBJFILES) $(RESDEPEN) $(DEFFILE)
    $(BCB)\BIN\$(LINKER) @&&!
    $(LFLAGS) -L$(LIBPATH) +
    $(ALLOBJ), +
    $(PROJECT),, +
    $(ALLLIB), +
    $(DEFFILE), +
    $(ALLRES)
!
# ---------------------------------------------------------------------------
.pas.hpp:
    $(BCB)\BIN\$(DCC32) $(PFLAGS) -U$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -O$(INCLUDEPATH) --BCB {$< }

.pas.obj:
    $(BCB)\BIN\$(DCC32) $(PFLAGS) -U$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -O$(INCLUDEPATH) --BCB {$< }

.cpp.obj:
    $(BCB)\BIN\$(BCC32) $(CFLAG1) $(WARNINGS) -I$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -n$(@D) {$< }

.c.obj:
    $(BCB)\BIN\$(BCC32) $(CFLAG1) $(WARNINGS) -I$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -n$(@D) {$< }

.c.i:
    $(BCB)\BIN\$(CPP32) $(CFLAG1) $(WARNINGS) -I$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -n. {$< }

.cpp.i:
    $(BCB)\BIN\$(CPP32) $(CFLAG1) $(WARNINGS) -I$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -n. {$< }

.asm.obj:
    $(BCB)\BIN\$(TASM32) $(AFLAGS) -i$(INCLUDEPATH:;= -i) $(AUSERDEFINES) -d$(SYSDEFINES:;= -d) $<, $@

.rc.res:
    $(BCB)\BIN\$(BRCC32) $(RFLAGS) -I$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -fo$@ $<



# ---------------------------------------------------------------------------
