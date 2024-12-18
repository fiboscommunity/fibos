##!/bin/bash

ver=$1

if [ -z "$ver" ]; then
    ver=10.04
fi

echo ""
echo "test fibos in ubuntu:${ver}"

PWD=$(pwd)
docker run -it --rm -v /home:/home ubuntu:${ver} bash -c "cd ${PWD}; bin/Linux_amd64_release/fibos test"
