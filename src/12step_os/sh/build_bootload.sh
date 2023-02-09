#!/usr/bin/env bash


if [ -e bootload/Makefile ]; then
    MAKEFLAG="make"
fi

if [ "${MAKEFLAG}" = "make" ] ; then
    echo build bootload
    (cd bootload && make -j$(nproc))
fi

