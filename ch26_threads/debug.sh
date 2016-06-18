port=$1
[ -z "$port" ] && {
  echo "no port number" >&2
  exit 1
}
./main_cli 127.0.0.1 $port </etc/services >./x1 &
./main_cli 127.0.0.1 $port </etc/services >./x2 &
./main_cli 127.0.0.1 $port </etc/services >./x3 &
wait

echo finished >&2
