@echo off
echo "Do not forget to update the version information in this batchfile"
echo "======================USE CTRL-C TO CANCEL======================="
PAUSE

cd TRowGrid
del *.obj TRowGrid.lib TRowGrid.bpl TRowGrid.bpi
make -DUSERDEFINES=DEMO -DDESCRIPTION="RowGrid based on units (demo)" -f TRowGrid.mak
tlib TRowGrid.lib /0 +TRowGrid.obj +RowGrid.obj
copy TRowGrid.bpl demo
copy TRowGrid.bpi demo
copy TRowGrid.lib demo
copy filelist.txt demo
copy license.txt demo
copy readme.txt demo
copy RowGrid.res demo
copy RowGrid.h demo\rowgrid.h
copy Help\TRowGrid.hlp demo\Help
copy Help\TRowGrid.cnt demo\Help

cd ActiveX
del *.obj RowGrid.lib RowGrid.ocx
make -DUSERDEFINES=DEMO -DLIBPATH="..\demo" -f RowGrid.mak
copy RowGrid.ocx ..\demo
copy RowGrid.lib ..\demo
echo.> ..\demo\RowGrid.lic

rem echo "RowGrid installer: welcome.dfm/license.dfm files need to be updated for RowGrid demo"
rem PAUSE

cd ..\install
del *.obj install.exe
make -DUSERDEFINES=PRODUCT=\\"RowGrid\\";VERSION=\\"1.0.2.64\\";DEMO -f install.mak
copy install.exe ..\demo

rem echo "RowGrid license.txt needs to updated for RowGrid demo"
rem PAUSE

cd ..\..
del *.obj planner.exe TDataRows\*.obj
del FastMM4BCB\*.dcu FastMM4BCB\*.obj
make -DUSERDEFINES=ACCESS;_USE_OLD_RW_STL;USE_INDY -f planner.mak
copy planner.exe demo\CapPlan.exe
copy planner.cfg demo\CapPlan.cfg
copy planner.NLD demo\CapPlan.NLD
rem copy filelist.txt demo
copy license.txt demo
