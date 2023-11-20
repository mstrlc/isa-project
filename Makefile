# Makefile
# Project: ISA -- LDAP server
# Author: Matyas Strelec xstrel03

CC=g++
CFLAGS=-std=c++20 -Wall -Wextra -pedantic -O0 -g

SRC_DIR := src
DOC_DIR := doc

EXE := isa-ldapserver
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:.cpp=.o)
DOC := doc/manual.pdf

.PHONY: all debug doc run clean

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

doc/manual.pdf: doc/manual.md
	pandoc -V geometry:margin=2cm --variable urlcolor=blue $< -o $@

doc: doc/manual.pdf

clean:
	rm -f $(OBJ) $(EXE) $(DOC)
	rm -rf $(SRC_DIR)/*.dSYM