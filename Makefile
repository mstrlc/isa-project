# Makefile
# Project: ISA -- LDAP server
# Author: Matyas Strelec xstrel03

CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -fcommon
DBFLAGS=-std=c99 -Wall -Wextra -pedantic -fcommon -g -fsanitize=address

SRC_DIR := src
INC_DIR := include

EXE := main
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:.c=.o)
DOC := doc.pdf

.PHONY: all debug doc clean

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

doc/doc.pdf: doc/doc.md
	pandoc -V geometry:margin=2cm --variable urlcolor=blue $< -o $@

debug: $(OBJ)
	$(CC) $(DBFLAGS) -o $(EXE) $^ -g

doc: doc/doc.pdf

run:
	./$(EXE)

clean:
	rm -f $(OBJ) $(EXE) $(DOC)