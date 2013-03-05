#!/bin/bash

TEST_PARSER="$(dirname "$0")"

for file in $(find $TEST_PARSER/good/ -name '*.mC'); do
    echo "Testing" $file "..."
    $TEST_PARSER/test_file.bash $@ -f $file
    STATUS=$?

    if [[ $STATUS == 0 ]]; then
	echo "Pass"
    else
	echo "Fail"
	exit $?
    fi
done

for file in $(find $TEST_PARSER/bad/ -name '*.mC'); do
    echo "Testing" $file "..."
    $TEST_PARSER/test_file.bash $@ -f $file
    STATUS=$?

    if [[ $STATUS != 0 ]]; then
	echo "Pass"
    else
	echo "Fail"
	exit $?
    fi
done