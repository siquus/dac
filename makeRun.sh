#!/bin/bash

printf "\n\nBuilding Code Generator...\n\n"
cd UnitTests/Playground
make clean
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
make -j3
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

printf "\n\nRunning Code generator...\n\n"
build/main.out
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

printf "\n\nBuilding generated Code...\n\n"
cd Program
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
