@echo off
echo First clean the 'build' directory...
rd /s /q build
mkdir build
cd build 
echo Then start cmake configure...
echo Tip : the third libs was built by architecture-x86 ~_~ 
cmake  -Thost=x86 -A Win32   ^
    -DBUILD_STATIC_LIBS=ON  ^
    -DBUILD_SHARED_LIBS=OFF  ^
    -DGLFW_LIBRARY_TYPE=STATIC^
    -DGLFW_BUILD_DOCS=OFF ^
	-DBUILD_UTILS=OFF ^
    ..
cd ..
pause