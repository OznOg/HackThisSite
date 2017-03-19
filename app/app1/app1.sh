#!/bin/bash
set -euo pipefail


if [ ! $# -eq 1 ]
then 
   echo Please provide binary to test as input
   exit 1
fi

FILE_TYPE=$(file --brief $1)

if [[ $FILE_TYPE =~ ^ELF ]]
then
   echo Password is $(strings $1 | grep -a IVanna)
fi

if [[ $FILE_TYPE =~ ^PE ]]
then
  echo Enter serial $(strings $1 | grep 1414) in application
  echo "(then it should give you smashth... as site password)"
fi
