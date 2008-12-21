#! /bin/bash

source ../patch.sh

patch '10i\ typedef void* GDataTestFunc;'  testman.h
