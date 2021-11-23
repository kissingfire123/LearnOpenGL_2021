#!/bin/bash

echo Platform is macOS,generate for XCode...
echo First clean the 'build' directory...
build_dir="`pwd`/build"
rm -rf  ${build_dir}
mkdir ${build_dir}
pushd ${build_dir}
echo Then start cmake configure...
cmake  -G Xcode  \
	-DBUILD_STATIC_LIBS=ON  \
    -DBUILD_SHARED_LIBS=ON  \
    -DGLFW_LIBRARY_TYPE=STATIC \
    -DGLFW_BUILD_DOCS=OFF \
    ..
popd ${build_dir}

echo "Configure and generate done !🍺🍺"
