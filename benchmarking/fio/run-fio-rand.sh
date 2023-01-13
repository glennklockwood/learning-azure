#!/bin/bash

for inputf in fio-rand-write.fio fio-rand-read.fio fio-rand-rewrite.fio
do
  sudo bash -c 'echo 3 > /proc/sys/vm/drop_caches'
  outputf="${inputf%.fio}.out"
  if [ "$inputf" == "$outputf" ]; then
    echo "inputf==outputf; aborting" >&2
    exit 1
  fi
  echo "Using $inputf and outputting to $outputf"
  sudo bash -c "sync; echo 3 > /proc/sys/vm/drop_caches" 
  fio --group_reporting \
      --output="$outputf" \
      "${inputf}"
  sync
  sleep 2
done
