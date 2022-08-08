#!/usr/bin/bash

KERNEL_DIR=./temp/timed-printer-kernel/
USER_DIR=./temp/timed-printer-user/ 
ZIP_FILE=timed_printer.zip

rm -f $ZIP_FILE

mkdir -p $KERNEL_DIR
cp -r src $KERNEL_DIR
cp Makefile $KERNEL_DIR

mkdir -p $USER_DIR
cp tprinter-client.py $USER_DIR

(cd temp && zip -r ../$ZIP_FILE .)

rm -rf temp


