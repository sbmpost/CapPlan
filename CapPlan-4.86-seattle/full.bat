@echo off
echo "Do not forget to update the version information in this batchfile"
echo "======================USE CTRL-C TO CANCEL======================="
PAUSE

cd TRowGrid
del *.obj TRowGrid.lib TRowGrid.bpl TRowGrid.bpi
make -DUSERDEFINES= -DDESCRIPTION="RowGrid based on units (freeware)" -f TRowGrid.mak
tlib TRowGrid.lib /0 +TRowGrid.obj +RowGrid.obj
copy TRowGrid.bpl full
copy TRowGrid.bpi full
copy TRowGrid.lib full
copy filelist.txt full
copy license.txt full
copy readme.txt full
copy RowGrid.res full
copy RowGrid.h full\rowgrid.h
copy Help\TRowGrid.hlp full\Help
copy Help\TRowGrid.cnt full\Help

cd ActiveX
del *.obj RowGrid.lib RowGrid.ocx
make -DUSERDEFINES= -DLIBPATH="..\full" -f RowGrid.mak
copy RowGrid.ocx ..\full
copy RowGrid.lib ..\full
copy RowGrid.lic ..\full

cd ..\install
del *.obj install.exe
make -DUSERDEFINES=PRODUCT=\\"RowGrid\\";VERSION=\\"1.0.2.64\\" -f install.mak
copy install.exe ..\full

cd ..\..\install
del *.obj install.exe
rem make -DUSERDEFINES=PRODUCT=\\"CapPlan\\";VERSION=\\"4.86\\";MS_SQL;NO_EXHANDLER -f install.mak
make -DUSERDEFINES=PRODUCT=\\"CapPlan\\";VERSION=\\"4.86\\";INNODB;NO_EXHANDLER -f install.mak
copy install.exe ..\full

cd ..
del *.obj planner.exe TDataRows\*.obj
del FastMM4BCB\*.dcu FastMM4BCB\*.obj
rem make -DUSERDEFINES=MS_SQL;MS_SQL_LOCK_TIMEOUT=\\"0\\";_USE_OLD_RW_STL -f planner.mak
make -DUSERDEFINES=INNODB;_USE_OLD_RW_STL;STRICT -f planner.mak
copy planner.exe full\CapPlan.exe
copy planner.NLD full\CapPlan.NLD
rem copy filelist.txt full
copy license.txt full
