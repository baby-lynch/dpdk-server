#!/bin/bash

APP=main

case "$1" in
"-d") sudo gdb ./build/${APP} -l 3-7 ;;
"") sudo ./build/${APP} -l 3-7 ;;
esac
