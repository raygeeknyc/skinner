#!/bin/bash
logger "$0"
cd /home/raymond/Documents/workspace/skinner
git pull
rc=$?
if [[ $rc -eq 0 ]]; then
  logger "$0 pulled new code"
  echo "pulled new code"
else
  logger "$0 Error pulling new code"
  echo "Error pulling new code"
fi
cat /dev/null > nohup.out
nohup python imageprocessor.py
