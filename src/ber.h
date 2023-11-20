/*
 * ber.h
 * 2023-11-20
 *
 * ISA project 2023
 * LDAP server
 *
 * Matyas Strelec xstrel03
 */

#pragma once

#include <string>
#include <tuple>
#include <vector>

// Constants
const unsigned char BER_INTEGER = 0x02;
const unsigned char BER_BOOLEAN = 0x01;
const unsigned char BER_OCTET_STRING = 0x04;
const unsigned char BER_SEQUENCE = 0x30;
const unsigned char BER_SET = 0x31;
const unsigned char BER_ENUMERATED = 0x0A;

// Filter constants
#define FILTER_AND 0xa0
#define FILTER_OR 0xa1
#define FILTER_NOT 0xa2
#define FILTER_EQUALITY_MATCH 0xa3
#define FILTER_SUBSTRINGS 0xa4

// Type Definitions
using ber_bytes = std::vector<unsigned char>;

