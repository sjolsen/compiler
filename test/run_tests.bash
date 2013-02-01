#!/bin/bash

for file in $(find test/good/ -name '*.mC'); do
    echo "Testing" $file "..."
    test/test_file.bash $@ -f $file
    STATUS=$?

    if [[ $STATUS == 0 ]]; then
	echo "Pass"
    else
	echo "Fail"
	exit $?
    fi
done

for file in $(find test/bad/ -name '*.mC'); do
    echo "Testing" $file "..."
    test/test_file.bash $@ -f $file
    STATUS=$?

    if [[ $STATUS != 0 ]]; then
	echo "Pass"
    else
	echo "Fail"
	exit $?
    fi
done