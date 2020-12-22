#!/bin/bash

COMPILED='a.out'
RESULT_FILE='tests/result.txt'

rm $RESULT_FILE

for input in "$@"
  do
    printf "TEST $input\n" | tee -a $RESULT_FILE >/dev/null
    cat "tests/$input" | "./$COMPILED" | tee -a $RESULT_FILE >/dev/null
    printf '\n' | tee -a $RESULT_FILE >/dev/null
  done
