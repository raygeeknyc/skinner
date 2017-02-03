#!/bin/bash
logger "$0"
cd /home/raymond/Documents/workspace/skinner
git pull
rc=$?
if [[ $rc -eq 0 ]]; then
  echo "pulled new code"
fi
cat /dev/null > nohup.out
#nohup python imageprocessor.py equalize
nohup python imageprocessor.py
