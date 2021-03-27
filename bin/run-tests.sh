#!/usr/bin/env bash

echo 'Running tests...'
for file in ../test/*.tiger
do
  echo "Testing $file..."
  ../cmake-build-debug/tigy "$file"
  echo "Done testing $file"
done
echo 'Done running tests'
