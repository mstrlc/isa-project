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

// Type Definitions
using ber_bytes = std::vector<unsigned char>;
