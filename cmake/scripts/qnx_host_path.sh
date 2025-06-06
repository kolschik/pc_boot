#!/bin/bash

qconfig -n "`qconfig | grep $1 | grep Install | awk  -F ": " '{print $2}'`" -e | grep QNX_HOST | cut -d "=" -f2 | sed 's/;//g' | sed 's/\"//g'

