#!/bin/bash
for FILE in output/*; do
  rm -f $FILE
done
for FILE in input/*; do
  echo "Executing input file" $FILE;
  fileName=`cut -d / -f 2 <<< $FILE`
  ../bin/main $FILE > output/$fileName
done
echo "\n*****Outputs are generated in output/*****"