#!/bin/bash

while [ 1 ]; do
    nc -zu $1 80
done
