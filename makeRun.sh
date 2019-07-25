#!/bin/bash

cd UnitTests/Playground
make clean
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
make -j3
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
build/main.out
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
cd Program
make clean
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
make -j3
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
build/main.out
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
cd ..
cd ..
cd ..
