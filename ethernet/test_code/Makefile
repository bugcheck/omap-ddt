###############################################################################
# (C) Copyright Texas Instruments - All Rights Reserved
#
# Name of the file: Makefile
#
# Brief description of the contents of the file:
# Top Makefile for Ethernet Test Suite 
#
# To compile execute the following command:
#
# make \
# CROSS_COMPILE=<toolchain.prefix> \
# KDIR=<kernel.path> \
# HOST=<host.prefix> 
#
# Author: Abraham Arce
# Created on: Mon Mar 10 23:02:20 CDT 2008
#
###############################################################################
OBJS := utilities install

all:$(OBJS)

utilities:
	make -C utils

install:
	mkdir -p bin
	mkdir -p scripts/test
	mkdir -p scripts/tmp
	make -C utils install
clean:
	make -C utils clean
	rm -rf scripts/log*
	rm -rf scripts/tmp
	rm -rf scripts/test
	rm -rf bin/
