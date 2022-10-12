#!/bin/bash

APP=dpdkserv

case "$1" in
"-d") sudo gdb ./build/${APP} --file-prefix app2 -l 25-31 ;;
"") sudo ./build/${APP} --file-prefix app2 -l 25-31 ;;
esac
