#!/bin/bash

for file in $(find test/scanner/good/ -name '*.mC'); do
    echo "Testing" $file "..."
    test/scanner/test_file.bash $@ -f $file
    STATUS=$?

    if [[ $STATUS == 0 ]]; then
	echo "Pass"
    else
	echo "Fail"
	exit $?
    fi
done

for file in $(find test/scanner/bad/ -name '*.mC'); do
    echo "Testing" $file "..."
    test/scanner/test_file.bash $@ -f $file
    STATUS=$?

    if [[ $STATUS != 0 ]]; then
	echo "Pass"
    else
	echo "Fail"
	exit $?
    fi
done