OS=`uname -s | awk '{print toupper($0)}'`
gcc -I../lib -g -O2 -D_REENTRANT -DHAVE_PSELECT_PROTO -Wall   -c -o test_getifaddrs.o test_getifaddrs.c -D$OS
gcc -I../lib -g -O2 -D_REENTRANT -Wall   -c -o adv_inet_masktop.o adv_inet_masktop.c -D$OS
gcc -I../lib -g -O2 -D_REENTRANT -Wall -o test_getifaddrs test_getifaddrs.o adv_inet_masktop.o ../libunp.a -lpthread -D$OS
