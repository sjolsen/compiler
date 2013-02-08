#!/bin/bash

TEST_SCANNER="$(dirname "$0")"

for file in $(find $TEST_SCANNER/good/ -name '*.mC'); do
    echo "Testing" $file "..."
    $TEST_SCANNER/test_file.bash $@ -f $file
    STATUS=$?

    if [[ $STATUS == 0 ]]; then
	echo "Pass"
    else
	echo "Fail"
	exit $?
    fi
done

for file in $(find $TEST_SCANNER/bad/ -name '*.mC'); do
    echo "Testing" $file "..."
    $TEST_SCANNER/test_file.bash $@ -f $file
    STATUS=$?

    if [[ $STATUS != 0 ]]; then
	echo "Pass"
    else
	echo "Fail"
	exit $?
    fi
done