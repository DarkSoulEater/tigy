#!/usr/bin/env bash

echo 'Running tests'
for file in ../test/*.tiger
do
  echo "Running $file"
  ../cmake-build-debug/tigy "$file"
  printf 'Done running %s\n\n' "$file"
done
echo 'Done testing'
