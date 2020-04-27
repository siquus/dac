#!/bin/bash

printf "\n\nBuilding Code Generator...\n\n"
cd Generator
make clean
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
make -j3
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

printf "\n\nRunning Code generator...\n\n"
build/main.out -s 1.0 -i 10000
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

printf "\n\nBuilding generated Code...\n\n"
cd ../Executor
make clean
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
make -j3
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

printf "\n\nRunning generated Code...\n\n"
build/main.out
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
cd ..
cd ..
cd ..
