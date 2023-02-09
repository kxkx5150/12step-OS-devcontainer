#!/usr/bin/env bash


if [ -e bootload/Makefile ]; then
    MAKEFLAGBOOT="make"
fi

if [ "${MAKEFLAGBOOT}" = "make" ] ; then
    echo build bootload
    (cd bootload && make -j$(nproc))
fi

if [ -e os/Makefile ]; then
    MAKEFLAGOS="make"
fi

if [ "${MAKEFLAGOS}" = "make" ] ; then
    echo build os
    (cd os && make -j$(nproc))
fi

