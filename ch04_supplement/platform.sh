#!/bin/sh

PID=$$
OS=`uname -s | tr '[:lower:]' '[:upper:]'`

CFLAGS="-D_${OS} -Wall" # suppose gcc

if test "X$OS" = "XAIX"; then
    unset CFLAGS

    # xlc or gcc ?
    echo "main(){}" >${PID}.c
    cc -o ${PID}.o -c ${PID}.c -qlist >/dev/null 2>&1
    if head -1 ${PID}.lst 2>/dev/null | grep -i 'XL C for AIX' >/dev/null 2>&1; then
        CFLAGS="-qflag=w:w"
        if ls -l /unix | grep unix_64 >/dev/null 2>&1; then
            CFLAGS="-D_AIX64 -q64 $CFLAGS"
        else
            CFLAGS="-D_AIX $CFLAGS"
        fi
    else
        CFLAGS="-Wall" # suppose gcc
        if ls -l /unix | grep unix_64 >/dev/null 2>&1; then
            CFLAGS="-D_AIX64 -maix64 $CFLAGS"
        else
            CFLAGS="-D_AIX $CFLAGS"
        fi
    fi
    rm -f ${PID}.* >/dev/null 2>&1
fi

echo "$OS%$CFLAGS"

