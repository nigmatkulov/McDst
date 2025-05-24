#!/bin/bash

# assuming that this macro is sitting at its original location,
# its path can be used to set the mcDst enviroment

filnam=$BASH_SOURCE            # filename of this (sourced) macro
absnam=`readlink -f $filnam`   # absolute filename
pather=`dirname $absnam`       # path to the config directory

export MCDST=$pather

export PATH=${MCDST}':'${PATH}
export LD_LIBRARY_PATH=${MCDST}':'${LD_LIBRARY_PATH}

echo MCDST = $MCDST
