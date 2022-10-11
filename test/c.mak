ROOT_DIR=$(shell dirname $(shell pwd))

CC?=gcc 
CFLAGS:=-Wall -g 
CFLAGS+=-fsanitize=address
CFLAGS+=-I$(ROOT_DIR)/include/seasched


SRC:=test_pqueue.c
SRC+=$(ROOT_DIR)/src/seasched/pqueue.c
OUT=main


all:$(SRC)
	$(CC) -o $(OUT) $^  $(CFLAGS)

.PHONY:clean
clean:
	rm -rf $(OUT)