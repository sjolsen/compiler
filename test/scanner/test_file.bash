#!/bin/bash

TEST_SCANNER="$(dirname "$0")"

VERBOSE=false
while getopts "vf:" OPT; do
    case $OPT in
	v) VERBOSE=true;;
	f) FILENAME=$OPTARG;;
    esac
done

if $VERBOSE; then
    $TEST_SCANNER/../../bin/scanner $FILENAME 2>&1
else
    $TEST_SCANNER/../../bin/scanner $FILENAME 1>/dev/null 2>/dev/null
fi

exit $?