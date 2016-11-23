#!/bin/sh
dir=`dirname $0`
cd $dir

typeset CC=gcc
PID=$$
HEADER=./check.h
CFILE=./$PID.c
EFILE=./$PID.exe
>$HEADER

as_cr_letters='abcdefghijklmnopqrstuvwxyz'
as_cr_LETTERS='ABCDEFGHIJKLMNOPQRSTUVWXYZ'
as_cr_Letters=$as_cr_letters$as_cr_LETTERS
as_cr_digits='0123456789'
as_cr_alnum=$as_cr_Letters$as_cr_digits

as_tr_cpp="sed y%*$as_cr_letters%P$as_cr_LETTERS%;s%[^_$as_cr_alnum]%_%g"

for ac_header in ifaddrs.h; do

echo "#include <${ac_header}>" >$CFILE

if ${CC} -E $CFILE >/dev/null 2>&1; then
cat >>$HEADER <<_EOF
#define `echo "HAVE_$ac_header" | $as_tr_cpp` 1
_EOF
else
cat >>$HEADER <<_EOF
/* #undef `echo "HAVE_$ac_header" | $as_tr_cpp` */
_EOF

fi

done

for ac_func in getifaddrs; do
cat $HEADER >./$CFILE
cat >>./$CFILE <<_EOF
#include <sys/types.h>
#ifdef HAVE_IFADDRS_H
#include <ifaddrs.h>
#endif

char (*f)() = $ac_func;

int main() {
return f != $ac_func;
return 0;
}
_EOF

if ${CC} -o $EFILE $CFILE >/dev/null 2>&1 && $EFILE 2>/dev/null; then
cat >>$HEADER <<_EOF
#define `echo "HAVE_$ac_func" | $as_tr_cpp` 1
_EOF
else
cat >>$HEADER <<_EOF
/* #undef `echo "HAVE_$ac_func" | $as_tr_cpp` */
_EOF
fi

done

for ac_struct in ifaddrs; do
cat $HEADER >$CFILE
cat >>$CFILE <<_EOF
#include <sys/types.h>
#ifdef HAVE_IFADDRS_H
#include <ifaddrs.h>
#endif

int
main ()
{
if ((struct ${ac_struct} *) 0)
return 0;
if (sizeof (struct ${ac_struct}))
return 0;
return 0;
}
_EOF

if ${CC} -o $EFILE $CFILE >/dev/null 2>&1 && $EFILE 2>/dev/null; then
cat >>$HEADER <<_EOF
#define `echo "HAVE_${ac_struct}_STRUCT" | $as_tr_cpp` 1
_EOF
else
cat >>$HEADER <<_EOF
/* #undef `echo "HAVE_${ac_struct}_STRUCT" | $as_tr_cpp` */
_EOF
fi


done


rm -f $CFILE $EFILE 2>/dev/null

