#!/bin/bash

echo "//#define CASTRATE" > revision.h
echo "#define MY_VERSION    \"0.9.1b\"" >> revision.h
echo "#define REVISION      \"[`git rev-parse --short HEAD`]\"" >> revision.h
echo "#define RELEASE_DATE  \"[`date +"%Y-%m-%d"`]\"" >> revision.h

