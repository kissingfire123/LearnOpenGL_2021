@echo off
echo First clean the 'build' directory...
rd /s /q build
mkdir build
cd build 
echo Then start cmake configure...
cmake ..
cd ..
pause