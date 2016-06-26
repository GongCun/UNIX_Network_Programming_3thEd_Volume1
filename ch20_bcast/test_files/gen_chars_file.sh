#!/bin/bash

START=$1
END=$2
[ -z "$START" -o -z "$END" ] && {
echo "Usage: $0 <record-start> <record-end>" >&2
exit 1
}

count=1
>${END}line
while [ $count -le $END ]; do
    echo -n a >> ${END}line
    if [ $count -ge $START -a $count -ne $END ]; then
        cp -p ${END}line ${count}line
    fi
    count=`expr $count + 1`
done
