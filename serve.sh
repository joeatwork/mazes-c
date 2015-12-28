#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
SIZE=${SIZE:-32}

echo "DIR: $DIR SIZE: $SIZE SEED: $RANDOM"
while true
do
    (cat <<EOF && $DIR/grid --width=$SIZE --height=$SIZE | $DIR/maze --seed=$RANDOM | $DIR/color --seed=$RANDOM | $DIR/png) | nc -l 20202 > /dev/null
HTTP/1.1 200 OK
Content-Type: image/png
Connection: close

EOF
done
