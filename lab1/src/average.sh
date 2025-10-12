#!/bin/bash
count=$#
sum=0
for arg in "$@" ; do
sum=$((sum + arg))
done
echo $count
avg=$((sum/count))
echo $avg 
