rem -----compilation file for C++Builder 6.0-----

cd TRowGrid
del *.obj TRowGrid.lib TRowGrid.bpl TRowGrid.bpi
make -DUSERDEFINES= -DDESCRIPTION="RowGrid based on units (freeware)" -f TRowGrid.mak
tlib TRowGrid.lib /0 +TRowGrid.obj +RowGrid.obj
copy TRowGrid.bpl full
copy TRowGrid.bpi full
copy TRowGrid.lib full
copy RowGrid.res full
copy RowGrid.h full\rowgrid.h

cd ActiveX
del *.obj RowGrid.lib RowGrid.ocx
make -DUSERDEFINES= -DLIBPATH="..\full" -f RowGrid.mak
copy RowGrid.ocx ..\full
copy RowGrid.lib ..\full
copy RowGrid.lic ..\full
cd ..\..
