# Makefile
# Project: ISA -- LDAP server
# Author: Matyas Strelec xstrel03

CC=g++
CFLAGS=-std=c++20 -Wall -Wextra -pedantic -fcommon
DBFLAGS=-std=c++20 -Wall -Wextra -pedantic -fcommon -g -fsanitize=address

SRC_DIR := src
INC_DIR := include
DOC_DIR := doc

EXE := isa-ldapserver
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:.cpp=.o)
DOC := doc.pdf

.PHONY: all debug doc run clean

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

doc/manual.pdf: doc/manual.md
	pandoc -V geometry:margin=2cm --variable urlcolor=blue $< -o $@

debug: $(OBJ)
	$(CC) $(DBFLAGS) -o $(EXE) $^ -g

doc: doc/manual.pdf

run:
	make
	./$(EXE)

clean:
	rm -f $(OBJ) $(EXE) $(DOC)