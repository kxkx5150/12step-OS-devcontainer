#!/usr/bin/env bash


if [ -e bootload/Makefile ]; then
    MAKEFLAGBOOT="make"
fi

if [ "${MAKEFLAGBOOT}" = "make" ] ; then
    echo build clean
    (cd bootload && make clean)
fi

if [ -e os/Makefile ]; then
    MAKEFLAGOS="make"
fi

if [ "${MAKEFLAGOS}" = "make" ] ; then
    echo build clean
    (cd os && make clean)
fi


