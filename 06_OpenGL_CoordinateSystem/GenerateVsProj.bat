@echo off
echo First clean the 'build' directory...
rd /s /q build
mkdir build
cd build 
echo Then start cmake configure...
echo Tip : the third libs was built by architecture-x86 ~_~ 
cmake  -Thost=x86 -A Win32 ..
cd ..
pause