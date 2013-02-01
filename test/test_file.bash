#!/bin/bash

VERBOSE=false
while getopts "vf:" OPT; do
    case $OPT in
	v) VERBOSE=true;;
	f) FILENAME=$OPTARG;;
    esac
done

if $VERBOSE; then
    bin/scanner $FILENAME
else
    bin/scanner $FILENAME 1>/dev/null 2>/dev/null
fi

exit $?