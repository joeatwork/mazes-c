#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
SIZE=${SIZE:-32}

while true
do
    (cat <<EOF && $DIR/mazes --width=$SIZE --height=$SIZE --format=png --seed=$RANDOM) | nc -l 20202 > /dev/null
HTTP/1.1 200 OK
Content-Type: image/png
Connection: close

EOF
done
