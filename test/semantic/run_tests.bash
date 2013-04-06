#!/bin/bash

TEST_SEMANTIC="$(dirname "$0")"

for file in $(find $TEST_SEMANTIC/good/ -name '*.mC'); do
    echo "Testing" $file "..."
    $TEST_SEMANTIC/test_file.bash $@ -f $file
    STATUS=$?

    if [[ $STATUS == 0 ]]; then
	echo "Pass"
    else
	echo "Fail"
	exit $?
    fi
done

for file in $(find $TEST_SEMANTIC/bad/ -name '*.mC'); do
    echo "Testing" $file "..."
    $TEST_SEMANTIC/test_file.bash $@ -f $file
    STATUS=$?

    if [[ $STATUS != 0 ]]; then
	echo "Pass"
    else
	echo "Fail"
	exit $?
    fi
done