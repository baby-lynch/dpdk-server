# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2010-2014 Intel Corporation

ROOT_DIR=$(shell dirname $(shell pwd))

# binary name
APP = main

# all source are stored in SRCS-y
SRCS-y := test_util.c

# includes
CFLAGS+=-I$(ROOT_DIR)/include


PKGCONF ?= pkg-config

# Build using pkg-config variables if possible
ifneq ($(shell $(PKGCONF) --exists libdpdk && echo 0),0)
$(error "no installation of DPDK found")
endif


PC_FILE := $(shell $(PKGCONF) --path libdpdk 2>/dev/null)
CFLAGS += -O3 $(shell $(PKGCONF) --cflags libdpdk)
LDFLAGS = $(shell $(PKGCONF) --libs libdpdk)

ifeq ($(MAKECMDGOALS),static)
# check for broken pkg-config
ifeq ($(shell echo $(LDFLAGS_STATIC) | grep 'whole-archive.*l:lib.*no-whole-archive'),)
$(warning "pkg-config output list does not contain drivers between 'whole-archive'/'no-whole-archive' flags.")
$(error "Cannot generate statically-linked binaries with this version of pkg-config")
endif
endif

CFLAGS += -DALLOW_EXPERIMENTAL_API

all:$(SRCS-y)
	$(CC) -o $(APP) $^ $(CFLAGS)  $(LDFLAGS)


.PHONY: clean
clean:
	rm -f $(APP) 