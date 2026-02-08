#!/bin/bash
# Test script for continuous integration.

# Stop script early on any error; check variables
set -eu

# Paths to tools installed by ci-install.sh
MAIN_DIR=${PWD}
BUILD_DIR=${PWD}/ci_build
PYTHON=${BUILD_DIR}/python-env/bin/python


######################################################################
# Section grouping output message helpers
######################################################################

start_test()
{
    echo "::group::=============== $1 $2"
    set -x
}

finish_test()
{
    set +x
    echo "=============== Finished $2"
    echo "::endgroup::"
}


######################################################################
# Run update src/configs/Creality/K1*_defconfig
######################################################################

for TARGET in src/configs/Creality/K1*_defconfig ; do
 start_test Update "$TARGET"
 make clean
 make distclean
 unset CC
 cp ${TARGET} .config
 make olddefconfig
 sed -e '/^#/d' -e '/^$/d' .config > "$TARGET"
 #printf "\n" >> "$TARGET"
 finish_test Update "$TARGET"
done
