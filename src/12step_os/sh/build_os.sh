#!/usr/bin/env bash


if [ -e os/Makefile ]; then
    MAKEFLAG="make"
fi

if [ "${MAKEFLAG}" = "make" ] ; then
    echo build os
    (cd os && make -j$(nproc))
fi


