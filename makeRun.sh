#!/bin/bash

printf "Running unit tests..."
cd UnitTests
./makeRun.sh > ../unitTests.log 2> ../unitTestsError.log
rc=$?;
if [[ $rc != 0 ]];
then
    printf "Failed!\n"
    exit $rc;
fi
printf "Passed!\n\n"
cd ..

printf "Running examples:\n"
cd Examples
./makeRun.sh
rc=$?;
if [[ $rc != 0 ]];
then
    exit $rc;
fi

printf "\nEverything passed!\n"
exit 0;
