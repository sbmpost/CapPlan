@echo off
echo "When compiling, you must make 3 times!! to produce the ENU file!"
echo "Also check that file PLAN_ENU.rc does not have a date in the future"
pause
del /Q enu
rmdir enu
del /Q plan_enu.res planner_drc.res
del /Q *.~* *.obj *.dsk
copy planner.bak.cpp planner.cpp
copy planner.bak.bpr planner.bpr
