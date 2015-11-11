#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

while true
do
    (cat <<EOF && $DIR/mazes --width=32 --height=32 --format=png) | nc -l 20202 > /dev/null
HTTP/1.1 200 OK
Content-Type: image/png
Connection: close

EOF
done
