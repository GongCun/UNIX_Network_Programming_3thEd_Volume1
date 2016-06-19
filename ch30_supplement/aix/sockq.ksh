#!/usr/bin/ksh

ps -e -o pid | sed '1d' | while read pid; do
  str="`./sockq $pid | grep -v 'Recv-Q: 0; Send-Q: 0'`"
  [ ! -z "$str" ] && printf "%s: %s\n" $pid "$str"
done


