#!/bin/bash

printf "Running solar system..."
cd SolarSystem
./makeRun.sh > /dev/null 2> /dev/null
rc=$?;
if [[ $rc != 0 ]];
then
    printf "Failed!\n"
    exit $rc;
fi
printf "Passed!\n"

exit 0;
