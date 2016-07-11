#!/bin/sh -u
<<<<<<< HEAD
if [ -z "$BUILDTYPE" ] || [[ "$BUILDTYPE" == "Release" ]]; then
    export BINARY_TAG=x86_64-slc6-gcc49-opt
    export BUILDTYPE="Release"
else
    export BINARY_TAG=x86_64-slc6-gcc49-dbg
    export BUILDTYPE="Debug"
fi
source /afs/cern.ch/exp/fcc/sw/0.7/init_fcc_stack.sh afs

if [[ "x$ACTS_DIR" == "x" ]]; then 
    echo "Need to set the ACTS_DIR environment variable to install directory."
    return 1
else 
    echo "ACTS_DIR   :    $ACTS_DIR"
fi


export EIGEN_INCLUDE_DIR=/afs/cern.ch/sw/lcg/releases/eigen/3.2.7-292e1/x86_64-slc6-gcc49-opt/include/eigen3/
echo "Set eigen directory to $EIGEN_INCLUDE_DIR"
=======
source /afs/cern.ch/exp/fcc/sw/0.7/init_fcc_stack.sh $1
>>>>>>> anna/testCellId
