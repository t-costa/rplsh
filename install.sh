#!/bin/bash

#rm -rf build

echo "starting install.sh"

if [ "$1" == "fresh" ]; then
    echo "Starting a fresh installation"
    rm -rf build
fi

if [ ! -d build ]; then
    echo "Creating build directory"
    mkdir build
else
  echo "Recompiling, existing directory"
fi

cxx="g++"
build_type="Release"

if [ "$1" == "debug" ]
then
    build_type="Debug"
    echo "Compiling in debug mode"
fi

if [ "$1" == "exp" ]  || [ "$2" == "exp" ]
then
   EXP="-DEXPANSION=ON"
   echo "Compiling with expansion"
fi

cd build && cmake ../rpl-shell -DCMAKE_CXX_COMPILER=$cxx -DCMAKE_BUILD_TYPE=$build_type $EXP
make
